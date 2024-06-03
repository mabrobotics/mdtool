#include "mainWorker.hpp"

#include <unistd.h>

#include <filesystem>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <limits.h>

#include "ConfigManager.hpp"
#include "ui.hpp"

enum class toolsCmd_E
{
	NONE,
	BLINK,
	BUS,
	CLEAR,
	CONFIG,
	ENCODER,
	PING,
	REGISTER,
	RESET,
	SETUP,
	TEST,
};
enum class toolsOptions_E
{
	NONE,
	ABSOLUTE,
	BANDWIDTH,
	CALIBRATION,
	CALIBRATION_OUTPUT,
	CAN,
	CLEAR,
	CURRENT,
	DIAGNOSTIC,
	ENCODER,
	ERROR,
	HOMING,
	INFO,
	LATENCY,
	MAIN,
	MOTOR,
	MOVE,
	OUTPUT,
	READ,
	READ_CONFIG,
	SAVE,
	WARNING,
	WRITE,
	ZERO,
};
toolsCmd_E str2cmd(std::string& cmd)
{
	if (cmd == "blink")
		return toolsCmd_E::BLINK;
	if (cmd == "bus")
		return toolsCmd_E::BUS;
	if (cmd == "clear")
		return toolsCmd_E::CLEAR;
	if (cmd == "config")
		return toolsCmd_E::CONFIG;
	if (cmd == "encoder")
		return toolsCmd_E::ENCODER;
	if (cmd == "ping")
		return toolsCmd_E::PING;
	if (cmd == "register")
		return toolsCmd_E::REGISTER;
	if (cmd == "reset")
		return toolsCmd_E::RESET;
	if (cmd == "setup")
		return toolsCmd_E::SETUP;
	if (cmd == "test")
		return toolsCmd_E::TEST;
	return toolsCmd_E::NONE;
}
toolsOptions_E str2option(std::string& opt)
{
	if (opt == "absolute")
		return toolsOptions_E::ABSOLUTE;
	if (opt == "bandwidth")
		return toolsOptions_E::BANDWIDTH;
	if (opt == "calibration")
		return toolsOptions_E::CALIBRATION;
	if (opt == "calibration_out")
		return toolsOptions_E::CALIBRATION_OUTPUT;
	if (opt == "can")
		return toolsOptions_E::CAN;
	if (opt == "clear")
		return toolsOptions_E::CLEAR;
	if (opt == "current")
		return toolsOptions_E::CURRENT;
	if (opt == "encoder")
		return toolsOptions_E::ENCODER;
	if (opt == "error")
		return toolsOptions_E::ERROR;
	if (opt == "homing")
		return toolsOptions_E::HOMING;
	if (opt == "info")
		return toolsOptions_E::INFO;
	if (opt == "latency")
		return toolsOptions_E::LATENCY;
	if (opt == "main")
		return toolsOptions_E::MAIN;
	if (opt == "motor")
		return toolsOptions_E::MOTOR;
	if (opt == "move")
		return toolsOptions_E::MOVE;
	if (opt == "output")
		return toolsOptions_E::OUTPUT;
	if (opt == "read")
		return toolsOptions_E::READ;
	if (opt == "read_config")
		return toolsOptions_E::READ_CONFIG;
	if (opt == "save")
		return toolsOptions_E::SAVE;
	if (opt == "warning")
		return toolsOptions_E::WARNING;
	if (opt == "write")
		return toolsOptions_E::WRITE;
	if (opt == "zero")
		return toolsOptions_E::ZERO;
	return toolsOptions_E::NONE;
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
	mdtoolBaseDir =
		std::string(getenv("HOME")) + "/" + mdtoolHomeConfigDirName + "/" + mdtoolDirName;
	mdtoolIniFilePath = mdtoolBaseDir + "/" + mdtoolIniFileName;

	/* copy motors configs directory - not the best practice to use system() but std::filesystem is
	 * not available until C++17 */
	struct stat info;
	int			result = 0;

	if (stat(mdtoolBaseDir.c_str(), &info) != 0)
		result =
			system(("cp -r " + mdtoolConfigPath + mdtoolDirName + " " + mdtoolBaseDir).c_str());
	else /* if the directory is not empty we should only copy the ini file (only if the version is
			newer) and all default config files */
	{
		mINI::INIFile	   file(mdtoolIniFilePath);
		mINI::INIStructure ini;
		file.read(ini);

		if (ini["general"]["version"] != getVersion())
		{
			result = system(("cp " + mdtoolConfigPath + mdtoolDirName + "/" + mdtoolIniFileName +
							 " " + mdtoolBaseDir + "/")
								.c_str());
			file.read(ini);
			ini["general"]["version"] = getVersion();
			file.write(ini);
		}
	}

	/* defaults */
	toolsCmd_E			  cmd	  = toolsCmd_E::NONE;
	mab::BusType_E		  busType = mab::BusType_E::USB;
	mab::CANdleBaudrate_E baud	  = mab::CANdleBaudrate_E::CAN_BAUD_1M;

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

	mINI::INIFile	   file(mdtoolIniFilePath);
	mINI::INIStructure ini;
	file.read(ini);

	busString = ini["communication"]["bus"];

	if (busString == "SPI")
		busType = mab::BusType_E::SPI;
	else if (busString == "UART")
		busType = mab::BusType_E::UART;
	else if (busString == "USB")
		busType = mab::BusType_E::USB;

	std::string& device = ini["communication"]["device"];

	try
	{
		if (device != "" && busType != mab::BusType_E::USB)
			candle = std::make_unique<mab::Candle>(baud, printVerbose, busType, device);
		else
			candle = std::make_unique<mab::Candle>(baud, printVerbose, busType);
	}
	catch (const char* e)
	{
		return;
	}

	toolsOptions_E option  = toolsOptions_E::NONE;
	toolsOptions_E option2 = toolsOptions_E::NONE;

	if (args.size() > 2)
		option = str2option(args[2]);
	if (args.size() > 3)
		option2 = str2option(args[3]);

	switch (cmd)
	{
		case toolsCmd_E::BLINK:
		{
			blink(args);
			break;
		}
		case toolsCmd_E::CLEAR:
		{
			if (option == toolsOptions_E::ERROR)
				clearErrors(args);
			else if (option == toolsOptions_E::WARNING)
				clearWarnings(args);
			else
				ui::printHelpTest();
			break;
		}
		case toolsCmd_E::CONFIG:
		{
			if (option == toolsOptions_E::BANDWIDTH)
				configBandwidth(args);
			else if (option == toolsOptions_E::CAN)
				configCan(args);
			else if (option == toolsOptions_E::CLEAR)
				configClear(args);
			else if (option == toolsOptions_E::CURRENT)
				configCurrent(args);
			else if (option == toolsOptions_E::SAVE)
				configSave(args);
			else if (option == toolsOptions_E::ZERO)
				configZero(args);
			else
				ui::printHelpConfig();
			break;
		}
		case toolsCmd_E::ENCODER:
		{
			encoder(args);
			break;
		}
		case toolsCmd_E::PING:
		{
			ping(args);
			break;
		}
		case toolsCmd_E::REGISTER:
		{
			if (option == toolsOptions_E::READ)
				registerRead(args);
			else if (option == toolsOptions_E::WRITE)
				registerWrite(args);
			else
				ui::printHelpTest();
			break;
		}
		case toolsCmd_E::RESET:
		{
			reset(args);
			break;
		}
		case toolsCmd_E::SETUP:
		{
			if (option == toolsOptions_E::CALIBRATION)
				setupCalibration(args);
			else if (option == toolsOptions_E::CALIBRATION_OUTPUT)
				setupCalibrationOutput(args);
			else if (option == toolsOptions_E::HOMING)
				setupHoming(args);
			else if (option == toolsOptions_E::INFO)
				setupInfo(args);
			else if (option == toolsOptions_E::MOTOR)
				setupMotor(args);
			else if (option == toolsOptions_E::READ_CONFIG)
				setupReadConfig(args);
			else
				ui::printHelpSetup();
			break;
		}
		case toolsCmd_E::TEST:
		{
			if (option == toolsOptions_E::ENCODER)
			{
				if (option2 == toolsOptions_E::MAIN)
					testEncoderMain(args);
				else if (option2 == toolsOptions_E::OUTPUT)
					testEncoderOutput(args);
				else
					ui::printHelpTest();
			}
			else if (option == toolsOptions_E::LATENCY)
				testLatency(args);
			else if (option == toolsOptions_E::MOVE)
			{
				if (option2 == toolsOptions_E::ABSOLUTE)
					testMoveAbsolute(args);
				else
					testMove(args);
			}
			else
				ui::printHelpTest();
			break;
		}
		default:
			return;
	}
}

std::string MainWorker::getVersion()
{
	return mab::getVersionString({MDTOOL_VTAG, MDTOOL_VREVISION, MDTOOL_VMINOR, MDTOOL_VMAJOR});
}

void MainWorker::bus(std::vector<std::string>& args)
{
	if (args.size() < 3 || args.size() > 4)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}

	if (args[2] != "SPI" && args[2] != "UART" && args[2] != "USB")
	{
		ui::printWrongArgumentsSpecified();
		return;
	}

	changeDefaultConfig(args[2], args[3]);
}

void MainWorker::blink(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 3, 2);
	if (id == -1)
		return;
	candle->configMd80Blink(id);
}

void MainWorker::clearErrors(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 4, 3);
	if (id == -1)
		return;
	candle->setupMd80ClearErrors(id);
}

void MainWorker::clearWarnings(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 4, 3);
	if (id == -1)
		return;
	candle->setupMd80ClearWarnings(id);
}

void MainWorker::configBandwidth(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 5, 3);
	if (id == -1)
		return;
	candle->configMd80TorqueBandwidth(id, atof(args[4].c_str()));
}

void MainWorker::configCan(std::vector<std::string>& args)
{
	if (!checkArgs(args, 7))
		return;
	int id = atoi(args[3].c_str());
	checkSpeedForId(id);
	int					  new_id   = atoi(args[4].c_str());
	mab::CANdleBaudrate_E new_baud = str2baud(args[5]);
	int					  timeout  = atoi(args[6].c_str());
	bool canTermination			   = (args.size() > 7 && atoi(args[7].c_str()) > 0) ? true : false;

	candle->configMd80Can(id, new_id, new_baud, timeout, canTermination);
}

void MainWorker::configClear(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 4, 3);
	if (id == -1)
		return;

	if (candle->writeMd80Register(id, mab::Md80Reg_E::runRestoreFactoryConfig, true))
		std::cout << "[MDTOOL] Config reverted to factory state!" << std::endl;
	else
		std::cout << "[MDTOOL] Error reverting config to factory state!" << std::endl;
}

void MainWorker::configCurrent(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 5, 3);
	if (id == -1)
		return;
	candle->configMd80SetCurrentLimit(id, atof(args[4].c_str()));
}

void MainWorker::configSave(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 4, 3);
	if (id == -1)
		return;
	candle->configMd80Save(id);
}

void MainWorker::configZero(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 4, 3);
	if (id == -1)
		return;
	candle->controlMd80SetEncoderZero(id);
}

void MainWorker::encoder(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 3, 2);
	if (id == -1)
		return;
	candle->controlMd80Mode(id, mab::Md80Mode_E::IDLE);
	candle->controlMd80Enable(id, true);
	candle->begin();

	while (1)
	{
		ui::printPositionAndVelocity(
			id, candle->md80s[0].getPosition(), candle->md80s[0].getVelocity());
		usleep(100000);
	}
	candle->end();
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
		ui::printScanOutput(candle.get());
	}
	else
	{
		candle->setVebose(true);
		candle->ping(baud);
		candle->setVebose(printVerbose);
	}
}

void MainWorker::registerRead(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 5, 3);
	if (id == -1)
		return;

	mab::Md80Reg_E regId = static_cast<mab::Md80Reg_E>(std::strtol(args[4].c_str(), nullptr, 16));

	std::string value = "";

	switch (mab::Register::getType(regId))
	{
		case mab::Register::type::U8:
			readRegisterToString<uint8_t>(id, regId, value);
			break;
		case mab::Register::type::I8:
			readRegisterToString<int8_t>(id, regId, value);
			break;
		case mab::Register::type::U16:
			readRegisterToString<uint16_t>(id, regId, value);
			break;
		case mab::Register::type::I16:
			readRegisterToString<int16_t>(id, regId, value);
			break;
		case mab::Register::type::U32:
			readRegisterToString<uint32_t>(id, regId, value);
			break;
		case mab::Register::type::I32:
			readRegisterToString<int32_t>(id, regId, value);
			break;
		case mab::Register::type::F32:
			readRegisterToString<float>(id, regId, value);
			break;
		case mab::Register::type::STR:
		{
			char str[24]{};
			candle->readMd80Register(id, regId, str);
			value = std::string(str);
			break;
		}
		case mab::Register::type::UNKNOWN:
			std::cout << "[MDTOOL] Unknown register! Please check the ID and try again"
					  << std::endl;
			break;
	}

	std::cout << "[MDTOOL] Register value: " << value << std::endl;
}

void MainWorker::registerWrite(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 6, 3);
	if (id == -1)
		return;

	mab::Md80Reg_E regId = static_cast<mab::Md80Reg_E>(std::strtol(args[4].c_str(), nullptr, 16));
	uint32_t	   regValue = atoi(args[5].c_str());

	bool success = false;

	switch (mab::Register::getType(regId))
	{
		case mab::Register::type::U8:
			success = candle->writeMd80Register(id, regId, static_cast<uint8_t>(regValue));
			break;
		case mab::Register::type::I8:
			success = candle->writeMd80Register(id, regId, static_cast<int8_t>(regValue));
			break;
		case mab::Register::type::U16:
			success = candle->writeMd80Register(id, regId, static_cast<uint16_t>(regValue));
			break;
		case mab::Register::type::I16:
			success = candle->writeMd80Register(id, regId, static_cast<int16_t>(regValue));
			break;
		case mab::Register::type::U32:
			success = candle->writeMd80Register(id, regId, static_cast<uint32_t>(regValue));
			break;
		case mab::Register::type::I32:
			success = candle->writeMd80Register(id, regId, static_cast<int32_t>(regValue));
			break;
		case mab::Register::type::F32:
			success = candle->writeMd80Register(
				id, regId, static_cast<float>(std::atof(args[5].c_str())));
			break;
		case mab::Register::type::STR:
		{
			char str[24]{};
			memcpy(str, args[5].c_str(), sizeof(str));
			success = candle->writeMd80Register(id, regId, str);
			break;
		}
		case mab::Register::type::UNKNOWN:
			std::cout << "[MDTOOL] Unknown register! Please check the ID and try again"
					  << std::endl;
	}
	if (success)
		std::cout << "[MDTOOL] Writing register successful!" << std::endl;
	else
		std::cout << "[MDTOOL] Writing register failed!" << std::endl;
}

void MainWorker::reset(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 3, 2);
	if (id == -1)
		return;
	candle->setupMd80PerformReset(id);
}

void MainWorker::setupCalibration(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 4, 3);
	if (id == -1)
		return;

	if (!ui::getCalibrationConfirmation() || checkSetupError(id))
		return;

	candle->setupMd80Calibration(id);
}

void MainWorker::setupCalibrationOutput(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 4, 3);
	if (id == -1)
		return;

	if (!ui::getCalibrationOutputConfirmation() || checkSetupError(id))
		return;

	uint16_t outputEncoder = 0;
	candle->readMd80Register(id, mab::Md80Reg_E::outputEncoder, outputEncoder);

	if (!outputEncoder)
	{
		std::cout << "[MDTOOL] No output encoder is configured! " << RED("[FAILED]") << std::endl;
		return;
	}

	candle->setupMd80CalibrationOutput(id);
}

void MainWorker::setupHoming(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 4, 3);
	if (id == -1)
		return;
	candle->setupMd80PerformHoming(id);
}

void MainWorker::setupInfo(std::vector<std::string>& args)
{
	bool printAll = false;

	if (args.size() < 4)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}
	if (args.size() == 5)
	{
		if (args[4] == "all")
			printAll = true;
	}
	int id = atoi(args[3].c_str());
	checkSpeedForId(id);
	if (!candle->addMd80(id))
		return;

	candle->setupMd80DiagnosticExtended(id);
	ui::printDriveInfoExtended(candle->getMd80FromList(id), printAll);
}

void MainWorker::setupMotor(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 5, 3);
	if (id == -1)
		return;

	ConfigManager configManager(args[4].c_str());

	std::string path	 = configManager.getConfigPath();
	std::string filename = configManager.getConfigName();

	if (configManager.isConfigDefault() && configManager.isConifgDifferent())
	{
		if (ui::getDifferentConfigsConfirmation(filename))
		{
			configManager.copyDefaultConfig(filename);
		}
	}

	if (!configManager.isConfigValid())
	{
		if (ui::getUpdateConfigConfirmation(filename))
		{
			path = configManager.validateConfig();
		}
	}

	mINI::INIFile	   motorCfg(path);
	mINI::INIStructure cfg;

	mINI::INIFile	   file(mdtoolIniFilePath);
	mINI::INIStructure ini;
	file.read(ini);

	if (!motorCfg.read(cfg))
	{
		ui::printUnableToFindCfgFile(path);
		return;
	}

	mab::regWrite_st& regW = candle->getMd80FromList(id).getWriteReg();
	mab::regRead_st&  regR = candle->getMd80FromList(id).getReadReg();

	/* add a field here only if you want to test it against limits form the mdtool.ini file */
	memcpy(
		regW.RW.motorName, (cfg["motor"]["name"]).c_str(), strlen((cfg["motor"]["name"]).c_str()));
	if (!getField(cfg, ini, "motor", "pole pairs", regW.RW.polePairs))
		return;
	if (!getField(cfg, ini, "motor", "torque constant", regW.RW.motorKt))
		return;
	if (!getField(cfg, ini, "motor", "KV", regW.RW.motorKV))
		return;
	if (!getField(cfg, ini, "motor", "gear ratio", regW.RW.gearRatio))
		return;
	if (!getField(cfg, ini, "motor", "max current", regW.RW.iMax))
		return;
	if (!getField(cfg, ini, "motor", "torque constant a", regW.RW.motorKt_a))
		return;
	if (!getField(cfg, ini, "motor", "torque constant b", regW.RW.motorKt_b))
		return;
	if (!getField(cfg, ini, "motor", "torque constant c", regW.RW.motorKt_c))
		return;
	if (!getField(cfg, ini, "motor", "torque bandwidth", regW.RW.torqueBandwidth))
		return;
	if (!getField(cfg, ini, "motor", "dynamic friction", regW.RW.friction))
		return;
	if (!getField(cfg, ini, "motor", "static friction", regW.RW.stiction))
		return;
	if (!getField(cfg, ini, "motor", "shutdown temp", regW.RW.motorShutdownTemp))
		return;
	if (!getField(cfg, ini, "limits", "max velocity", regW.RW.maxVelocity))
		return;
	if (!getField(cfg, ini, "limits", "max acceleration", regW.RW.maxAcceleration))
		return;
	if (!getField(cfg, ini, "limits", "max deceleration", regW.RW.maxDeceleration))
		return;

	auto checkFieldWriteIfPopulated =
		[&](const char* category, const char* field, auto& fieldVar, mab::Md80Reg_E reg) -> bool
	{
		if (cfg[category][field] == "")
			return true;

		if (!getField(cfg, ini, category, field, fieldVar))
			return false;

		if (!candle->writeMd80Register(id, reg, fieldVar))
		{
			ui::printFailedToSetupMotor(reg);
			return false;
		}
		return true;
	};

	if (!checkFieldWriteIfPopulated("hardware",
									"shunt resistance",
									regR.RO.shuntResistance,
									mab::Md80Reg_E::shuntResistance))
		return;

	if (!checkFieldWriteIfPopulated("motor",
									"reverse direction",
									regR.RW.reverseDirection,
									mab::Md80Reg_E::reverseDirection))
		return;

	regW.RW.motorCalibrationMode =
		getNumericParamFromList(cfg["motor"]["calibration mode"], ui::motorCalibrationModes);

	regW.RW.outputEncoderDefaultBaud =
		atoi(cfg["output encoder"]["output encoder default baud"].c_str());
	regW.RW.outputEncoder =
		getNumericParamFromList(cfg["output encoder"]["output encoder"], ui::encoderTypes);
	regW.RW.outputEncoderMode =
		getNumericParamFromList(cfg["output encoder"]["output encoder mode"], ui::encoderModes);
	regW.RW.outputEncoderCalibrationMode = getNumericParamFromList(
		cfg["output encoder"]["output encoder calibration mode"], ui::encoderCalibrationModes);
	regW.RW.homingMode = getNumericParamFromList(cfg["homing"]["mode"], ui::homingModes);
	regW.RW.brakeMode  = getNumericParamFromList(cfg["brake"]["mode"], ui::brakeModes);

	auto floatFromField = [&](const char* category, const char* field) -> float
	{ return atof(cfg[category][field].c_str()); };

	/* motor base config */
	if (!candle->writeMd80Register(id,
								   mab::Md80Reg_E::motorName,
								   regW.RW.motorName,
								   mab::Md80Reg_E::motorPolePairs,
								   regW.RW.polePairs,
								   mab::Md80Reg_E::motorKt,
								   regW.RW.motorKt,
								   mab::Md80Reg_E::motorKV,
								   regW.RW.motorKV,
								   mab::Md80Reg_E::motorGearRatio,
								   regW.RW.gearRatio,
								   mab::Md80Reg_E::motorIMax,
								   regW.RW.iMax,
								   mab::Md80Reg_E::motorTorgueBandwidth,
								   regW.RW.torqueBandwidth))
		ui::printFailedToSetupMotor(mab::Md80Reg_E::motorTorgueBandwidth);

	/* motor advanced config */
	if (!candle->writeMd80Register(id,
								   mab::Md80Reg_E::motorFriction,
								   regW.RW.friction,
								   mab::Md80Reg_E::motorStiction,
								   regW.RW.stiction,
								   mab::Md80Reg_E::motorKt_a,
								   regW.RW.motorKt_a,
								   mab::Md80Reg_E::motorKt_b,
								   regW.RW.motorKt_b,
								   mab::Md80Reg_E::motorKt_c,
								   regW.RW.motorKt_c,
								   mab::Md80Reg_E::outputEncoder,
								   regW.RW.outputEncoder,
								   mab::Md80Reg_E::outputEncoderMode,
								   regW.RW.outputEncoderMode,
								   mab::Md80Reg_E::outputEncoderDefaultBaud,
								   regW.RW.outputEncoderDefaultBaud))
		ui::printFailedToSetupMotor(mab::Md80Reg_E::outputEncoderDefaultBaud);

	/* motor motion config - Position and velocity PID*/
	if (!candle->writeMd80Register(id,
								   mab::Md80Reg_E::motorPosPidKp,
								   floatFromField("position PID", "kp"),
								   mab::Md80Reg_E::motorPosPidKi,
								   floatFromField("position PID", "ki"),
								   mab::Md80Reg_E::motorPosPidKd,
								   floatFromField("position PID", "kd"),
								   mab::Md80Reg_E::motorPosPidWindup,
								   floatFromField("position PID", "windup"),
								   mab::Md80Reg_E::motorVelPidKp,
								   floatFromField("velocity PID", "kp"),
								   mab::Md80Reg_E::motorVelPidKi,
								   floatFromField("velocity PID", "ki"),
								   mab::Md80Reg_E::motorVelPidKd,
								   floatFromField("velocity PID", "kd"),
								   mab::Md80Reg_E::motorVelPidWindup,
								   floatFromField("velocity PID", "windup")))
		ui::printFailedToSetupMotor(mab::Md80Reg_E::motorVelPidWindup);

	/* motor motion config - Impedance PD*/
	if (!candle->writeMd80Register(id,
								   mab::Md80Reg_E::motorImpPidKp,
								   floatFromField("impedance PD", "kp"),
								   mab::Md80Reg_E::motorImpPidKd,
								   floatFromField("impedance PD", "kd"),
								   mab::Md80Reg_E::motorShutdownTemp,
								   regW.RW.motorShutdownTemp))
		ui::printFailedToSetupMotor(mab::Md80Reg_E::motorShutdownTemp);

	if (!candle->writeMd80Register(
			id, mab::Md80Reg_E::outputEncoderCalibrationMode, regW.RW.outputEncoderCalibrationMode))
		ui::printFailedToSetupMotor(mab::Md80Reg_E::outputEncoderCalibrationMode);

	if (!candle->writeMd80Register(
			id, mab::Md80Reg_E::motorCalibrationMode, regW.RW.motorCalibrationMode))
		ui::printFailedToSetupMotor(mab::Md80Reg_E::motorCalibrationMode);

	if (!candle->writeMd80Register(id,
								   mab::Md80Reg_E::homingMode,
								   regW.RW.homingMode,
								   mab::Md80Reg_E::homingMaxTravel,
								   floatFromField("homing", "max travel"),
								   mab::Md80Reg_E::homingTorque,
								   floatFromField("homing", "max torque"),
								   mab::Md80Reg_E::homingVelocity,
								   floatFromField("homing", "max velocity")))
		ui::printFailedToSetupMotor(mab::Md80Reg_E::homingMode);

	if (!candle->writeMd80Register(id,
								   mab::Md80Reg_E::maxTorque,
								   floatFromField("limits", "max torque"),
								   mab::Md80Reg_E::maxAcceleration,
								   regW.RW.maxAcceleration,
								   mab::Md80Reg_E::maxDeceleration,
								   regW.RW.maxDeceleration,
								   mab::Md80Reg_E::maxVelocity,
								   regW.RW.maxVelocity,
								   mab::Md80Reg_E::positionLimitMin,
								   floatFromField("limits", "min position"),
								   mab::Md80Reg_E::positionLimitMax,
								   floatFromField("limits", "max position")))
		ui::printFailedToSetupMotor(mab::Md80Reg_E::positionLimitMin);

	if (!candle->writeMd80Register(id,
								   mab::Md80Reg_E::profileAcceleration,
								   floatFromField("profile", "acceleration"),
								   mab::Md80Reg_E::profileDeceleration,
								   floatFromField("profile", "deceleration"),
								   mab::Md80Reg_E::quickStopDeceleration,
								   floatFromField("profile", "quick stop deceleration"),
								   mab::Md80Reg_E::profileVelocity,
								   floatFromField("profile", "velocity")))
		ui::printFailedToSetupMotor(mab::Md80Reg_E::positionLimitMin);

	if (!candle->writeMd80Register(id, mab::Md80Reg_E::brakeMode, regW.RW.brakeMode))
		ui::printFailedToSetupMotor(mab::Md80Reg_E::brakeMode);

	candle->configMd80Save(id);

	/* wait for a full reboot */
	sleep(3);
}

void MainWorker::setupReadConfig(std::vector<std::string>& args)
{
	std::cout << "Read config." << std::endl;

	int32_t id = checkArgsAndGetId(args, 4, 3);
	if (id == -1)
		return;

	mab::regRead_st& regR = candle->getMd80FromList(id).getReadReg();

	char motorNameChar[24];

	candle->readMd80Register(id, mab::Md80Reg_E::motorName, motorNameChar);

	std::string configName = std::string(motorNameChar) + "_read.cfg";

	bool saveConfig = ui::getSaveMotorConfigConfirmation(configName);

	std::string defaultConfigPath =
		std::string(getenv("HOME")) + "/.config/mdtool/mdtool_motors/default.ini";
	mINI::INIFile	   defaultFile(defaultConfigPath);
	mINI::INIStructure readIni;
	defaultFile.read(readIni);

	candle->readMd80Register(id,
							 mab::Md80Reg_E::motorPolePairs,
							 regR.RW.polePairs,
							 mab::Md80Reg_E::motorKV,
							 regR.RW.motorKV,
							 mab::Md80Reg_E::motorKt,
							 regR.RW.motorKt,
							 mab::Md80Reg_E::motorGearRatio,
							 regR.RW.gearRatio,
							 mab::Md80Reg_E::motorIMax,
							 regR.RW.iMax,
							 mab::Md80Reg_E::motorTorgueBandwidth,
							 regR.RW.torqueBandwidth,
							 mab::Md80Reg_E::motorShutdownTemp,
							 regR.RW.motorShutdownTemp);

	readIni["motor"]["name"]			 = std::string(motorNameChar);
	readIni["motor"]["pole pairs"]		 = floatToString(regR.RW.polePairs);
	readIni["motor"]["KV"]				 = floatToString(regR.RW.motorKV);
	readIni["motor"]["torque constant"]	 = floatToString(regR.RW.motorKt);
	readIni["motor"]["gear ratio"]		 = floatToString(regR.RW.gearRatio);
	readIni["motor"]["max current"]		 = floatToString(regR.RW.iMax);
	readIni["motor"]["torque bandwidth"] = floatToString(regR.RW.torqueBandwidth);
	readIni["motor"]["shutdown temp"]	 = floatToString(regR.RW.motorShutdownTemp);

	candle->readMd80Register(id,
							 mab::Md80Reg_E::maxTorque,
							 regR.RW.maxTorque,
							 mab::Md80Reg_E::maxVelocity,
							 regR.RW.maxVelocity,
							 mab::Md80Reg_E::positionLimitMax,
							 regR.RW.positionLimitMax,
							 mab::Md80Reg_E::positionLimitMin,
							 regR.RW.positionLimitMin,
							 mab::Md80Reg_E::maxAcceleration,
							 regR.RW.maxAcceleration,
							 mab::Md80Reg_E::maxDeceleration,
							 regR.RW.maxDeceleration);

	readIni["limits"]["max torque"]		  = floatToString(regR.RW.maxTorque);
	readIni["limits"]["max velocity"]	  = floatToString(regR.RW.maxVelocity);
	readIni["limits"]["max position"]	  = floatToString(regR.RW.positionLimitMax);
	readIni["limits"]["min position"]	  = floatToString(regR.RW.positionLimitMin);
	readIni["limits"]["max acceleration"] = floatToString(regR.RW.maxAcceleration);
	readIni["limits"]["max deceleration"] = floatToString(regR.RW.maxDeceleration);

	candle->readMd80Register(id,
							 mab::Md80Reg_E::profileAcceleration,
							 regR.RW.profileAcceleration,
							 mab::Md80Reg_E::profileDeceleration,
							 regR.RW.profileDeceleration,
							 mab::Md80Reg_E::profileVelocity,
							 regR.RW.profileVelocity);

	readIni["profile"]["acceleration"] = floatToString(regR.RW.profileAcceleration);
	readIni["profile"]["deceleration"] = floatToString(regR.RW.profileDeceleration);
	readIni["profile"]["velocity"]	   = floatToString(regR.RW.profileVelocity);

	candle->readMd80Register(id,
							 mab::Md80Reg_E::outputEncoder,
							 regR.RW.outputEncoder,
							 mab::Md80Reg_E::outputEncoderMode,
							 regR.RW.outputEncoderMode);

	if (regR.RW.outputEncoder == 0.f)
		readIni["output encoder"]["output encoder"] = 0.f;
	else
		readIni["output encoder"]["output encoder"] = ui::encoderTypes[regR.RW.outputEncoder];

	if (regR.RW.outputEncoderMode == 0.f)
		readIni["output encoder"]["output encoder mode"] = 0.f;
	else
		readIni["output encoder"]["output encoder mode"] =
			ui::encoderModes[regR.RW.outputEncoderMode];

	float kp = 0.f, ki = 0.f, kd = 0.f, windup = 0.f;

	candle->readMd80Register(id,
							 mab::Md80Reg_E::motorPosPidKp,
							 kp,
							 mab::Md80Reg_E::motorPosPidKi,
							 ki,
							 mab::Md80Reg_E::motorPosPidKd,
							 kd,
							 mab::Md80Reg_E::motorPosPidWindup,
							 windup);

	readIni["position PID"]["kp"]	  = floatToString(kp);
	readIni["position PID"]["ki"]	  = floatToString(ki);
	readIni["position PID"]["kd"]	  = floatToString(kd);
	readIni["position PID"]["windup"] = floatToString(windup);

	kp = ki = kd = windup = 0.f;

	candle->readMd80Register(id,
							 mab::Md80Reg_E::motorVelPidKp,
							 kp,
							 mab::Md80Reg_E::motorVelPidKi,
							 ki,
							 mab::Md80Reg_E::motorVelPidKd,
							 kd,
							 mab::Md80Reg_E::motorVelPidWindup,
							 windup);

	readIni["velocity PID"]["kp"]	  = floatToString(kp);
	readIni["velocity PID"]["ki"]	  = floatToString(ki);
	readIni["velocity PID"]["kd"]	  = floatToString(kd);
	readIni["velocity PID"]["windup"] = floatToString(windup);

	kp = ki = kd = windup = 0.f;

	candle->readMd80Register(
		id, mab::Md80Reg_E::motorImpPidKp, kp, mab::Md80Reg_E::motorImpPidKd, kd);

	readIni["impedance PD"]["kp"] = floatToString(kp);
	readIni["impedance PD"]["kd"] = floatToString(kd);

	candle->readMd80Register(id,
							 mab::Md80Reg_E::homingMode,
							 regR.RW.homingMode,
							 mab::Md80Reg_E::homingMaxTravel,
							 regR.RW.homingMaxTravel,
							 mab::Md80Reg_E::homingTorque,
							 regR.RW.homingTorque,
							 mab::Md80Reg_E::homingVelocity,
							 regR.RW.homingVelocity);

	readIni["homing"]["mode"]		  = ui::homingModes[regR.RW.homingMode];
	readIni["homing"]["max travel"]	  = floatToString(regR.RW.homingMaxTravel);
	readIni["homing"]["max torque"]	  = floatToString(regR.RW.homingTorque);
	readIni["homing"]["max velocity"] = floatToString(regR.RW.homingVelocity);

	if (saveConfig)
	{
		std::string saveConfigPath;
		char		buffer[PATH_MAX];

		if (getcwd(buffer, sizeof(buffer)) != NULL)
		{
			saveConfigPath = buffer;
		}
		else
		{
			perror("getcwd() error");
		}
		saveConfigPath += "/" + configName;
		mINI::INIFile configFile(saveConfigPath);
		configFile.write(readIni);
	}
	ui::printMotorConfig(readIni);
}

void MainWorker::testEncoderMain(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 5, 4);
	if (id == -1)
		return;

	/* check if no critical errors are present */
	if (checkErrors(id))
	{
		std::cout << "[MDTOOL] Could not proceed due to errors: " << std::endl;
		ui::printAllErrors(candle->md80s[0]);
		return;
	}

	candle->setupMd80TestMainEncoder(id);
}

void MainWorker::testEncoderOutput(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 5, 4);
	if (id == -1)
		return;
	/* check if no critical errors are present */
	if (checkErrors(id))
	{
		std::cout << "[MDTOOL] Could not proceed due to errors: " << std::endl;
		ui::printAllErrors(candle->md80s[0]);
		return;
	}

	uint16_t outputEncoder = 0;
	candle->readMd80Register(id, mab::Md80Reg_E::outputEncoder, outputEncoder);

	if (!outputEncoder)
	{
		std::cout << "[MDTOOL] No output encoder is configured! " << RED("[FAILED]") << std::endl;
		return;
	}

	candle->setupMd80TestOutputEncoder(id);
}

void MainWorker::testLatency(std::vector<std::string>& args)
{
	struct sched_param sp;
	memset(&sp, 0, sizeof(sp));
	sp.sched_priority = 99;
	sched_setscheduler(0, SCHED_FIFO, &sp);

	if (args.size() != 4)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}

	auto ids = candle->ping(str2baud(args[3]));

	if (ids.size() == 0)
		return;

	checkSpeedForId(ids[0]);

	for (auto& id : ids)
	{
		candle->addMd80(id);
		candle->controlMd80Mode(id, mab::Md80Mode_E::IMPEDANCE);
	}

	candle->begin();

	std::vector<uint32_t> samples;
	const uint32_t		  timelen = 10;

	sleep(1);

	for (uint32_t i = 0; i < timelen; i++)
	{
		sleep(1);
		samples.push_back(candle->getActualCommunicationFrequency());
		std::cout << "Current average communication speed: " << samples[i] << " Hz" << std::endl;
	}

	/* calculate mean and stdev */
	float sum = std::accumulate(std::begin(samples), std::end(samples), 0.0);
	float m	  = sum / samples.size();

	float accum = 0.0;
	std::for_each(
		std::begin(samples), std::end(samples), [&](const float d) { accum += (d - m) * (d - m); });

	float stdev = sqrt(accum / (samples.size() - 1));

	ui::printLatencyTestResult(ids.size(), m, stdev, busString);

	candle->end();
}

void MainWorker::testMoveAbsolute(std::vector<std::string>& args)
{
	if (args.size() < 6)
	{
		ui::printTooFewArgsNoHelp();
		return;
	}
	uint16_t id = atoi(args[4].c_str());

	if (!tryAddMD80(id))
		return;

	float targetPos = std::stof(args[5].c_str());

	/* check if no critical errors are present */
	if (checkErrors(id))
	{
		std::cout << "[MDTOOL] Could not proceed due to errors: " << std::endl;
		ui::printAllErrors(candle->md80s[0]);
		return;
	}

	if (args.size() > 6)
		candle->writeMd80Register(id, mab::Md80Reg_E::profileVelocity, std::stof(args[6].c_str()));
	if (args.size() > 7)
		candle->writeMd80Register(
			id, mab::Md80Reg_E::profileAcceleration, std::stof(args[7].c_str()));
	if (args.size() > 8)
		candle->writeMd80Register(
			id, mab::Md80Reg_E::profileDeceleration, std::stof(args[8].c_str()));

	candle->controlMd80Mode(id, mab::Md80Mode_E::POSITION_PROFILE);
	candle->controlMd80Enable(id, true);
	candle->begin();

	candle->md80s[0].setTargetPosition(targetPos);
	while (!candle->md80s[0].isTargetPositionReached())
	{
		sleep(1);
	};
	std::cout << "[MDTOOL] TARGET REACHED!" << std::endl;

	candle->end();
}

void MainWorker::testMove(std::vector<std::string>& args)
{
	int32_t id = checkArgsAndGetId(args, 5, 3);
	if (id == -1)
		return;

	float targetPos = atof(args[4].c_str());
	if (targetPos > 10.0f)
		targetPos = 10.0f;
	if (targetPos < -10.0f)
		targetPos = -10.0f;

	/* check if no critical errors are present */
	if (checkErrors(id))
	{
		std::cout << "[MDTOOL] Could not proceed due to errors: " << std::endl;
		ui::printAllErrors(candle->md80s[0]);
		return;
	}

	candle->controlMd80SetEncoderZero(id);
	candle->controlMd80Mode(id, mab::Md80Mode_E::IMPEDANCE);
	candle->controlMd80Enable(id, true);
	candle->begin();
	usleep(100000);

	float dp  = (targetPos + candle->md80s[0].getPosition()) / 300;
	float pos = 0.0f;
	for (int i = 0; i < 300; i++)
	{
		pos += dp;
		candle->md80s[0].setTargetPosition(pos);
		usleep(10000);
		ui::printPositionAndVelocity(
			id, candle->md80s[0].getPosition(), candle->md80s[0].getVelocity());
	}
	std::cout << std::endl;

	candle->end();
}

void MainWorker::changeDefaultConfig(std::string bus, std::string device)
{
	mINI::INIFile	   file(mdtoolIniFilePath);
	mINI::INIStructure ini;
	file.read(ini);
	if (!bus.empty())
		ini["communication"]["bus"] = bus;
	if (!device.empty() && (bus == "SPI" || bus == "UART"))
		ini["communication"]["device"] = device;
	else
		ini["communication"]["device"] = "";
	file.write(ini);
}

mab::CANdleBaudrate_E MainWorker::checkSpeedForId(uint16_t id)
{
	std::initializer_list<mab::CANdleBaudrate_E> bauds = {mab::CANdleBaudrate_E::CAN_BAUD_1M,
														  mab::CANdleBaudrate_E::CAN_BAUD_2M,
														  mab::CANdleBaudrate_E::CAN_BAUD_5M,
														  mab::CANdleBaudrate_E::CAN_BAUD_8M};

	for (auto& baud : bauds)
	{
		candle->configCandleBaudrate(baud);
		if (candle->checkMd80ForBaudrate(id))
			return baud;
	}

	return mab::CANdleBaudrate_E::CAN_BAUD_1M;
}

uint8_t MainWorker::getNumericParamFromList(std::string&					param,
											const std::vector<std::string>& list)
{
	int i = 0;
	for (auto& type : list)
	{
		if (type == param)
			return i;
		i++;
	}
	return 0;
}

bool MainWorker::checkErrors(uint16_t canId)
{
	candle->setupMd80DiagnosticExtended(canId);

	if (candle->getMd80FromList(canId).getReadReg().RO.mainEncoderErrors & 0x0000ffff ||
		candle->getMd80FromList(canId).getReadReg().RO.outputEncoderErrors & 0x0000ffff ||
		candle->getMd80FromList(canId).getReadReg().RO.calibrationErrors & 0x0000ffff ||
		candle->getMd80FromList(canId).getReadReg().RO.hardwareErrors & 0x0000ffff ||
		candle->getMd80FromList(canId).getReadReg().RO.bridgeErrors & 0x0000ffff ||
		candle->getMd80FromList(canId).getReadReg().RO.communicationErrors & 0x0000ffff)
		return true;

	return false;
}

/* gets field only if the value is within bounds form the ini file */
template <class T>
bool MainWorker::getField(mINI::INIStructure& cfg,
						  mINI::INIStructure& ini,
						  std::string		  category,
						  std::string		  field,
						  T&				  value)
{
	T min = 0;
	T max = 0;

	if (std::is_same<T, std::uint16_t>::value || std::is_same<T, std::uint8_t>::value ||
		std::is_same<T, std::int8_t>::value || std::is_same<T, std::uint32_t>::value)
	{
		value = atoi(cfg[category][field].c_str());
		min	  = atoi(ini["limit min"][field].c_str());
		max	  = atoi(ini["limit max"][field].c_str());
	}
	else if (std::is_same<T, std::float_t>::value)
	{
		value = strtof(cfg[category][field].c_str(), nullptr);
		min	  = strtof(ini["limit min"][field].c_str(), nullptr);
		max	  = strtof(ini["limit max"][field].c_str(), nullptr);
	}

	if (ui::checkParamLimit(value, min, max))
		return true;
	else
	{
		ui::printParameterOutOfBounds(category, field);
		return false;
	}
}

bool MainWorker::checkArgs(std::vector<std::string>& args, uint32_t size)
{
	if (args.size() < size)
	{
		ui::printTooFewArgsNoHelp();
		return false;
	}
	return true;
}

bool MainWorker::tryAddMD80(uint16_t id)
{
	checkSpeedForId(id);

	if (!candle->addMd80(id))
		return false;
	return true;
}

int MainWorker::checkArgsAndGetId(std::vector<std::string>& args, uint32_t size, uint32_t idPos)
{
	if (!checkArgs(args, size))
		return -1;

	int id = atoi(args[idPos].c_str());
	if (!tryAddMD80(id))
		return -1;

	return id;
}

bool MainWorker::checkSetupError(uint16_t id)
{
	uint32_t calibrationStatus;
	candle->readMd80Register(id, mab::Md80Reg_E::calibrationErrors, calibrationStatus);

	if (calibrationStatus & (1 << ui::calibrationErrorList.at(std::string("ERROR_SETUP"))))
	{
		std::cout << "[MDTOOL] Could not proceed due to " << RED("ERROR_SETUP")
				  << ". Please call mdtool setup motor <ID> <cfg> first." << std::endl;
		return true;
	}

	return false;
}

std::string MainWorker::floatToString(float value)
{
	std::stringstream ss;
	ss << std::fixed;

	if (static_cast<int>(value) == value)
	{
		ss << std::setprecision(1);
		ss << value;
		return ss.str();
	}
	else
	{
		ss << std::setprecision(7);
		ss << value;
		std::string str = ss.str();
		return str.substr(0, str.find_last_not_of('0') + 1);
	}
}
