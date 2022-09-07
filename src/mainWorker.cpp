#include "mainWorker.hpp"

#include <unistd.h>

#include <filesystem>

#include "mini/ini.h"
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
	DIAGNOSTIC_EX,
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
	if (opt == "diagnostic_ex")
		return toolsOptions_E::DIAGNOSTIC_EX;
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
	toolsCmd_E cmd = toolsCmd_E::NONE;
	/* defaults */
	mab::BusType_E busType = mab::BusType_E::USB;
	mab::CANdleBaudrate_E baud = mab::CANdleBaudrate_E::CAN_BAUD_1M;

	/* get user home directory */
	pathFull = (std::string(getenv("HOME")) + mdtoolConfigPath);

	/* create .config directory if doesnt exist already */
	std::filesystem::create_directory(std::string(getenv("HOME")) + mdtoolConfigPath);

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

	mINI::INIFile file(pathFull + mdtoolConfigFileName);
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
			if (option == toolsOptions_E::DIAGNOSTIC_EX)
				setupDiagnosticExtended(args);
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
	if (args.size() != 5)
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
	candle->setupMd80Calibration(id, (uint16_t)atoi(args[4].c_str()));
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

	std::string path = (std::string(getenv("HOME")) + motorConfigPath + args[4].c_str());

	/* create motor configs directory if it doesnt exist already */
	std::filesystem::create_directory(std::string(getenv("HOME")) + motorConfigPath);

	mINI::INIFile motorCfg(path);
	mINI::INIStructure cfg;

	if (!motorCfg.read(cfg))
	{
		ui::printUnableToFindCfgFile(path);
		return;
	}

	motorConfig_ut motorConfig;
	memset(motorConfig.bytes, 0, sizeof(motorConfig));

	memcpy(motorConfig.s.motorName, (cfg["motor"]["name"]).c_str(), sizeof(motorConfig.s.motorName));
	motorConfig.s.polePairs = atoi(cfg["motor"]["pole pairs"].c_str());
	motorConfig.s.motorKt = atof(cfg["motor"]["torque constant"].c_str());
	motorConfig.s.gearRatio = atof(cfg["motor"]["gear ratio"].c_str());
	motorConfig.s.iMax = atoi(cfg["motor"]["max current"].c_str());
	motorConfig.s.motorKt_a = atof(cfg["motor"]["torque constant a"].c_str());
	motorConfig.s.motorKt_b = atof(cfg["motor"]["torque constant b"].c_str());
	motorConfig.s.motorKt_c = atof(cfg["motor"]["torque constant c"].c_str());
	motorConfig.s.torqueBandwidth = atoi(cfg["motor"]["torque bandwidth"].c_str());

	motorConfig.s.outputEncoder = atoi(cfg["output encoder"]["output encoder"].c_str());
	motorConfig.s.outputEncoderDir = atoi(cfg["output encoder"]["output encoder dir"].c_str());

	char txBuffer[64] = {0};
	char rxBuffer[64] = {0};

	static_assert(sizeof(motorConfig.bytes) <= 63);

	txBuffer[0] = mab::FRAME_MOTOR_CONFIG;
	memcpy(&txBuffer[1], motorConfig.bytes, sizeof(motorConfig.bytes));

	if (!candle->sengGenericFDCanFrame(id, 64, txBuffer, rxBuffer, 100))
	{
		std::cout << "FAILED to setup motor" << std::endl;
	}

	motorMotionConfig_ut motorMotionConfig;
	memset(motorMotionConfig.bytes, 0, sizeof(motorMotionConfig));

	motorMotionConfig.s.impedancePdGains.kp = atof(cfg["impedance PD"]["kp"].c_str());
	motorMotionConfig.s.impedancePdGains.kd = atof(cfg["impedance PD"]["kd"].c_str());
	motorMotionConfig.s.impedancePdGains.outMax = atof(cfg["impedance PD"]["max out"].c_str());

	motorMotionConfig.s.positionPidGains.kp = atof(cfg["position PID"]["kp"].c_str());
	motorMotionConfig.s.positionPidGains.ki = atof(cfg["position PID"]["ki"].c_str());
	motorMotionConfig.s.positionPidGains.kd = atof(cfg["position PID"]["kd"].c_str());
	motorMotionConfig.s.positionPidGains.outMax = atof(cfg["position PID"]["max out"].c_str());
	motorMotionConfig.s.positionPidGains.intWindup = atof(cfg["position PID"]["windup"].c_str());

	motorMotionConfig.s.velocityPidGains.kp = atof(cfg["velocity PID"]["kp"].c_str());
	motorMotionConfig.s.velocityPidGains.ki = atof(cfg["velocity PID"]["ki"].c_str());
	motorMotionConfig.s.velocityPidGains.kd = atof(cfg["velocity PID"]["kd"].c_str());
	motorMotionConfig.s.velocityPidGains.outMax = atof(cfg["velocity PID"]["max out"].c_str());
	motorMotionConfig.s.velocityPidGains.intWindup = atof(cfg["velocity PID"]["windup"].c_str());

	static_assert(sizeof(motorMotionConfig.bytes) <= 63);

	txBuffer[0] = mab::FRAME_MOTOR_MOTION_CONFIG;
	memcpy(&txBuffer[1], motorMotionConfig.bytes, sizeof(motorMotionConfig.bytes));

	if (!candle->sengGenericFDCanFrame(id, 64, txBuffer, rxBuffer, 100))
	{
		std::cout << "FAILED to setup motion motor" << std::endl;
	}

	candle->configMd80Save(id);
}

void MainWorker::setupDiagnosticExtended(std::vector<std::string>& args)
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

	mab::motorParameters_ut motorParameters;
	memset(motorParameters.bytes, 0, sizeof(motorParameters.bytes));

	candle->setupMd80DiagnosticExtended(id, &motorParameters);
	ui::printDriveInfoExtended(&motorParameters, id, candle->md80s[0].getPosition(), candle->md80s[0].getVelocity(), candle->md80s[0].getTorque(), candle->md80s[0].getTemperature(), candle->md80s[0].getErrorVector(), candle->getCurrentBaudrate());
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
	mINI::INIFile file(pathFull);
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
