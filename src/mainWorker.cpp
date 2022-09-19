#include "mainWorker.hpp"

#include <unistd.h>

#include <filesystem>

#include "ui.hpp"

enum class toolsCmd_E
{
	NONE,
	PING,
	CONFIG,
	SETUP,
	TEST,
	BLINK,
	ENCODER,
	BUS
};
enum class toolsOptions_E
{
	NONE,
	CURRENT,
	CAN,
	SAVE,
	ZERO,
	BANDWIDTH,
	CALIBRATION,
	DIAGNOSTIC,
	MOTOR,
	INFO,
};
toolsOptions_E str2option(std::string& opt)
{
	if (opt == "current")
		return toolsOptions_E::CURRENT;
	if (opt == "can")
		return toolsOptions_E::CAN;
	if (opt == "zero")
		return toolsOptions_E::ZERO;
	if (opt == "save")
		return toolsOptions_E::SAVE;
	if (opt == "bandwidth")
		return toolsOptions_E::BANDWIDTH;
	if (opt == "calibration")
		return toolsOptions_E::CALIBRATION;
	if (opt == "diagnostic")
		return toolsOptions_E::DIAGNOSTIC;
	if (opt == "motor")
		return toolsOptions_E::MOTOR;
	if (opt == "info")
		return toolsOptions_E::INFO;
	return toolsOptions_E::NONE;
}
toolsCmd_E str2cmd(std::string& cmd)
{
	if (cmd == "ping")
		return toolsCmd_E::PING;
	if (cmd == "config")
		return toolsCmd_E::CONFIG;
	if (cmd == "setup")
		return toolsCmd_E::SETUP;
	if (cmd == "test")
		return toolsCmd_E::TEST;
	if (cmd == "blink")
		return toolsCmd_E::BLINK;
	if (cmd == "encoder")
		return toolsCmd_E::ENCODER;
	if (cmd == "bus")
		return toolsCmd_E::BUS;
	return toolsCmd_E::NONE;
}

mab::CANdleBaudrate_E str2baud(std::string& baud)
{
	if (baud == "1M")
		return mab::CANdleBaudrate_E::CAN_BAUD_1M;
	if (baud == "2M")
		return mab::CANdleBaudrate_E::CAN_BAUD_2M;
	if (baud == "5M")
		return mab::CANdleBaudrate_E::CAN_BAUD_5M;
	if (baud == "8M")
		return mab::CANdleBaudrate_E::CAN_BAUD_8M;
	return mab::CANdleBaudrate_E::CAN_BAUD_1M;
}

MainWorker::MainWorker(std::vector<std::string>& args)
{
	mdtoolBaseDir = std::string(getenv("HOME")) + "/" + mdtoolHomeConfigDirName + "/" + mdtoolDirName;
	mdtoolIniFilePath = mdtoolBaseDir + "/" + mdtoolIniFileName;

	/* copy motors configs directory - not the best practice to use system() but std::filesystem is not available until C++17 */
	struct stat info;
	if (stat(mdtoolBaseDir.c_str(), &info) != 0)
		system(("cp -r " + mdtoolConfigPath + mdtoolDirName + " " + mdtoolBaseDir).c_str());

	/* defaults */
	toolsCmd_E cmd = toolsCmd_E::NONE;
	mab::BusType_E busType = mab::BusType_E::USB;
	mab::CANdleBaudrate_E baud = mab::CANdleBaudrate_E::CAN_BAUD_1M;

	if (args.size() > 1)
		cmd = str2cmd(args[1]);
	if (cmd == toolsCmd_E::NONE)
	{
		ui::printUnknownCmd(args[1]);
		return;
	}
	else if (cmd == toolsCmd_E::BUS)
		bus(args);

	printVerbose = true;

	for (auto& arg : args)
	{
		if (arg == "-sv")
			printVerbose = false;
	}

	mINI::INIFile file(mdtoolIniFilePath);
	mINI::INIStructure ini;
	file.read(ini);

	std::string& busString = ini["communication"]["bus"];

	if (busString == "SPI")
		busType = mab::BusType_E::SPI;
	else if (busString == "UART")
		busType = mab::BusType_E::UART;
	else if (busString == "USB")
		busType = mab::BusType_E::USB;

	candle = new mab::Candle(baud, printVerbose, mab::CANdleFastMode_E::NORMAL, true, busType);

	toolsOptions_E option = toolsOptions_E::NONE;
	if (args.size() > 2)
		option = str2option(args[2]);
	switch (cmd)
	{
		case toolsCmd_E::PING:
			ping(args);
			break;
		case toolsCmd_E::CONFIG:
		{
			if (option == toolsOptions_E::CAN)
				configCan(args);
			if (option == toolsOptions_E::SAVE)
				configSave(args);
			if (option == toolsOptions_E::ZERO)
				configZero(args);
			if (option == toolsOptions_E::CURRENT)
				configCurrent(args);
			if (option == toolsOptions_E::BANDWIDTH)
				configBandwidth(args);
			if (option == toolsOptions_E::NONE)
				ui::printHelpConfig();
			break;
		}
		case toolsCmd_E::SETUP:
		{
			if (option == toolsOptions_E::NONE)
				ui::printHelpSetup();
			if (option == toolsOptions_E::CALIBRATION)
				setupCalibration(args);
			if (option == toolsOptions_E::DIAGNOSTIC)
				setupDiagnostic(args);
			if (option == toolsOptions_E::MOTOR)
				setupMotor(args);
			if (option == toolsOptions_E::INFO)
				setupInfo(args);
			break;
		}
		case toolsCmd_E::TEST:
		{
			testMove(args);
			break;
		}
		case toolsCmd_E::BLINK:
		{
			blink(args);
			break;
		}
		case toolsCmd_E::ENCODER:
		{
			encoder(args);
			break;
		}
		default:
			return;
	}
}

void MainWorker::ping(std::vector<std::string>& args)
{
	bool performScan = false;

	if (args.size() < 2 || args.size() > 4)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}

	mab::CANdleBaudrate_E baud = candle->getCurrentBaudrate();

	if (args.size() == 3)
	{
		baud = str2baud(args[2]);
		if (args[2] == "all")
			performScan = true;
	}

	if (performScan)
	{
		candle->setVebose(false);
		ui::printScanOutput(candle);
	}
	else
	{
		candle->setVebose(true);
		candle->ping(baud);
		candle->setVebose(printVerbose);
	}
}

void MainWorker::configCan(std::vector<std::string>& args)
{
	if (args.size() != 7)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}

	int id = atoi(args[3].c_str());
	checkSpeedForId(id);

	int new_id = atoi(args[4].c_str());
	mab::CANdleBaudrate_E baud = str2baud(args[5]);
	int timeout = atoi(args[6].c_str());
	candle->configMd80Can(id, new_id, baud, timeout);
}
void MainWorker::configSave(std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}
	int id = atoi(args[3].c_str());
	checkSpeedForId(id);
	if (!candle->addMd80(id))
		return;
	candle->configMd80Save(id);
}
void MainWorker::configZero(std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}
	int id = atoi(args[3].c_str());
	checkSpeedForId(id);
	if (!candle->addMd80(id))
		return;
	candle->controlMd80SetEncoderZero(id);
}
void MainWorker::configCurrent(std::vector<std::string>& args)
{
	if (args.size() != 5)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}
	int id = atoi(args[3].c_str());
	checkSpeedForId(id);
	if (!candle->addMd80(id))
		return;
	candle->configMd80SetCurrentLimit(id, atof(args[4].c_str()));
}

void MainWorker::configBandwidth(std::vector<std::string>& args)
{
	if (args.size() != 5)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}
	int id = atoi(args[3].c_str());
	checkSpeedForId(id);
	if (!candle->addMd80(id))
		return;

	candle->configMd80TorqueBandwidth(id, atof(args[4].c_str()));
}

void MainWorker::setupCalibration(std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}

	if (!ui::getCalibrationConfirmation())
		return;
	int id = atoi(args[3].c_str());
	checkSpeedForId(id);
	if (!candle->addMd80(id))
		return;

	candle->setupMd80Calibration(id);
}
void MainWorker::setupDiagnostic(std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}
	int id = atoi(args[3].c_str());
	checkSpeedForId(id);
	if (!candle->addMd80(id))
		return;
	candle->setupMd80Diagnostic(id);
	ui::printDriveInfo(id, candle->md80s[0].getPosition(), candle->md80s[0].getVelocity(), candle->md80s[0].getTorque(), candle->md80s[0].getTemperature(), candle->md80s[0].getErrorVector(), candle->getCurrentBaudrate());
}

void MainWorker::setupMotor(std::vector<std::string>& args)
{
	int id = atoi(args[3].c_str());
	checkSpeedForId(id);
	if (!candle->addMd80(id))
		return;

	std::string path = (mdtoolBaseDir + "/" + mdtoolMotorCfgDirName + "/" + args[4].c_str());

	mINI::INIFile motorCfg(path);
	mINI::INIStructure cfg;

	mINI::INIFile file(mdtoolIniFilePath);
	mINI::INIStructure ini;
	file.read(ini);

	if (!motorCfg.read(cfg))
	{
		ui::printUnableToFindCfgFile(path);
		return;
	}

	mab::regWrite_st& regW = candle->getMd80FromList(id).getWriteReg();

	memcpy(regW.RW.motorName, (cfg["motor"]["name"]).c_str(), strlen((cfg["motor"]["name"]).c_str()));
	if (!getField(cfg, ini, "motor", "pole pairs", regW.RW.polePairs)) return;
	if (!getField(cfg, ini, "motor", "torque constant", regW.RW.motorKt)) return;
	if (!getField(cfg, ini, "motor", "gear ratio", regW.RW.gearRatio)) return;
	if (!getField(cfg, ini, "motor", "max current", regW.RW.iMax)) return;
	if (!getField(cfg, ini, "motor", "torque constant a", regW.RW.motorKt_a)) return;
	if (!getField(cfg, ini, "motor", "torque constant b", regW.RW.motorKt_b)) return;
	if (!getField(cfg, ini, "motor", "torque constant c", regW.RW.motorKt_c)) return;
	if (!getField(cfg, ini, "motor", "torque bandwidth", regW.RW.torqueBandwidth)) return;
	if (!getField(cfg, ini, "motor", "dynamic friction", regW.RW.friction)) return;
	if (!getField(cfg, ini, "motor", "static friction", regW.RW.stiction)) return;
	if (!getField(cfg, ini, "output encoder", "output encoder", regW.RW.outputEncoder)) return;
	if (!getField(cfg, ini, "output encoder", "output encoder dir", regW.RW.outputEncoderDir)) return;

	/* motor base config */
	if (!candle->writeMd80Register(id,
								   mab::Md80Reg_E::motorName, regW.RW.motorName,
								   mab::Md80Reg_E::motorPolePairs, regW.RW.polePairs,
								   mab::Md80Reg_E::motorKt, regW.RW.motorKt,
								   mab::Md80Reg_E::motorGearRatio, regW.RW.gearRatio,
								   mab::Md80Reg_E::motorIMax, regW.RW.iMax,
								   mab::Md80Reg_E::motorTorgueBandwidth, regW.RW.torqueBandwidth))
		ui::printFailedToSetupMotor();

	/* motor advanced config */
	if (!candle->writeMd80Register(id,
								   mab::Md80Reg_E::motorFriction, regW.RW.friction,
								   mab::Md80Reg_E::motorStiction, regW.RW.stiction,
								   mab::Md80Reg_E::motorKt_a, regW.RW.motorKt_a,
								   mab::Md80Reg_E::motorKt_b, regW.RW.motorKt_b,
								   mab::Md80Reg_E::motorKt_c, regW.RW.motorKt_c,
								   mab::Md80Reg_E::outputEncoder, regW.RW.outputEncoder,
								   mab::Md80Reg_E::outputEncoderDir, regW.RW.outputEncoderDir))
		ui::printFailedToSetupMotor();

	/* motor motion config - Position and velocity PID*/
	if (!candle->writeMd80Register(id,
								   mab::Md80Reg_E::motorPosPidKp, (float)atof(cfg["position PID"]["kp"].c_str()),
								   mab::Md80Reg_E::motorPosPidKi, (float)atof(cfg["position PID"]["ki"].c_str()),
								   mab::Md80Reg_E::motorPosPidKd, (float)atof(cfg["position PID"]["kd"].c_str()),
								   mab::Md80Reg_E::motorPosPidOutMax, (float)atof(cfg["position PID"]["max out"].c_str()),
								   mab::Md80Reg_E::motorPosPidWindup, (float)atof(cfg["position PID"]["windup"].c_str()),
								   mab::Md80Reg_E::motorVelPidKp, (float)atof(cfg["velocity PID"]["kp"].c_str()),
								   mab::Md80Reg_E::motorVelPidKi, (float)atof(cfg["velocity PID"]["ki"].c_str()),
								   mab::Md80Reg_E::motorVelPidKd, (float)atof(cfg["velocity PID"]["kd"].c_str()),
								   mab::Md80Reg_E::motorVelPidOutMax, (float)atof(cfg["velocity PID"]["max out"].c_str()),
								   mab::Md80Reg_E::motorVelPidWindup, (float)atof(cfg["velocity PID"]["windup"].c_str())))
		ui::printFailedToSetupMotor();

	/* motor motion config - Impedance PD*/
	if (!candle->writeMd80Register(id,
								   mab::Md80Reg_E::motorImpPidKp, (float)atof(cfg["impedance PD"]["kp"].c_str()),
								   mab::Md80Reg_E::motorImpPidKd, (float)atof(cfg["impedance PD"]["kd"].c_str()),
								   mab::Md80Reg_E::motorImpPidOutMax, (float)atof(cfg["impedance PD"]["max out"].c_str())))
		ui::printFailedToSetupMotor();

	candle->configMd80Save(id);
}

void MainWorker::setupInfo(std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}
	int id = atoi(args[3].c_str());
	checkSpeedForId(id);
	if (!candle->addMd80(id))
		return;

	candle->setupMd80DiagnosticExtended(id);
	ui::printDriveInfoExtended(candle->getMd80FromList(id));
}

void MainWorker::testMove(std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}

	int id = atoi(args[2].c_str());
	checkSpeedForId(id);
	float targetPos = atof(args[3].c_str());
	if (targetPos > 10.0f)
		targetPos = 10.0f;
	if (targetPos < -10.0f)
		targetPos = -10.0f;

	if (!candle->addMd80(id))
		return;
	candle->controlMd80SetEncoderZero(id);
	candle->controlMd80Mode(id, mab::Md80Mode_E::IMPEDANCE);
	candle->controlMd80Enable(id, true);
	candle->begin();
	usleep(100000);

	float dp = (targetPos + candle->md80s[0].getPosition()) / 300;
	float pos = 0.0f;
	for (int i = 0; i < 300; i++)
	{
		pos += dp;
		candle->md80s[0].setTargetPosition(pos);
		usleep(10000);
		ui::printPosition(id, candle->md80s[0].getPosition());
	}

	candle->end();
	candle->controlMd80Enable(id, false);
}
void MainWorker::blink(std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}

	int id = atoi(args[2].c_str());
	checkSpeedForId(id);
	if (!candle->addMd80(id))
		return;
	candle->configMd80Blink(id);
}
void MainWorker::encoder(std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}

	int id = atoi(args[2].c_str());
	checkSpeedForId(id);
	if (!candle->addMd80(id))
		return;
	candle->controlMd80Mode(id, mab::Md80Mode_E::IDLE);
	candle->controlMd80Enable(id, true);
	candle->begin();

	while (1)
	{
		ui::printPositionAndVelocity(id, candle->md80s[0].getPosition(), candle->md80s[0].getVelocity());
		usleep(100000);
	}
	candle->end();
}
void MainWorker::bus(std::vector<std::string>& args)
{
	if (args.size() < 3 || args.size() > 3)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}

	if (args[2] != "SPI" && args[2] != "UART" && args[2] != "USB")
	{
		ui::printWrongArgumentsSpecified();
		return;
	}

	if (args.size() == 4)
		changeDefaultConfig(args[2]);
	else
		changeDefaultConfig(args[2]);
}

void MainWorker::changeDefaultConfig(std::string bus)
{
	mINI::INIFile file(mdtoolIniFilePath);
	mINI::INIStructure ini;
	file.read(ini);
	if (!bus.empty()) ini["communication"]["bus"] = bus;
	file.write(ini);
}

mab::CANdleBaudrate_E MainWorker::checkSpeedForId(uint16_t id)
{
	std::initializer_list<mab::CANdleBaudrate_E> bauds = {mab::CANdleBaudrate_E::CAN_BAUD_1M, mab::CANdleBaudrate_E::CAN_BAUD_2M, mab::CANdleBaudrate_E::CAN_BAUD_5M, mab::CANdleBaudrate_E::CAN_BAUD_8M};

	for (auto& baud : bauds)
	{
		candle->configCandleBaudrate(baud);
		if (candle->checkMd80ForBaudrate(id))
			return baud;
	}

	return mab::CANdleBaudrate_E::CAN_BAUD_1M;
}

/* gets field only if the value is within bounds form the ini file */
template <class T>
bool MainWorker::getField(mINI::INIStructure& cfg, mINI::INIStructure& ini, std::string category, std::string field, T& value)
{
	T min = 0;
	T max = 0;

	if (std::is_same<T, std::uint16_t>::value || std::is_same<T, std::uint8_t>::value || std::is_same<T, std::int8_t>::value || std::is_same<T, std::uint32_t>::value)
	{
		value = atoi(cfg[category][field].c_str());
		min = atoi(ini["limit min"][field].c_str());
		max = atoi(ini["limit max"][field].c_str());
	}
	else if (std::is_same<T, std::float_t>::value)
	{
		value = strtof(cfg[category][field].c_str(), nullptr);
		min = strtof(ini["limit min"][field].c_str(), nullptr);
		max = strtof(ini["limit max"][field].c_str(), nullptr);
	}

	if (ui::checkParamLimit(value, min, max))
		return true;
	else
	{
		ui::printParameterOutOfBounds(category, field);
		return false;
	}
}
