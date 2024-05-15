#pragma once

#include "bus.hpp"
#include "candle.hpp"
#include "mini/ini.h"

class MainWorker
{
  public:
	MainWorker(std::vector<std::string>& args);
	static std::string getVersion();

  private:
	const std::string mdtoolHomeConfigDirName = ".config";
	const std::string mdtoolDirName = "mdtool";
	const std::string mdtoolMotorCfgDirName = "mdtool_motors";
	const std::string mdtoolIniFileName = "mdtool.ini";

	const std::string mdtoolConfigPath = "/etc/";

	std::string mdtoolBaseDir;
	std::string mdtoolIniFilePath;

	std::unique_ptr<mab::Candle> candle;

	std::string busString;

	bool printVerbose = false;

	void ping(std::vector<std::string>& args);
	void configCan(std::vector<std::string>& args);
	void configSave(std::vector<std::string>& args);
	void configZero(std::vector<std::string>& args);
	void configCurrent(std::vector<std::string>& args);
	void configBandwidth(std::vector<std::string>& args);
	void configClear(std::vector<std::string>& args);

	void setupCalibration(std::vector<std::string>& args);
	void setupCalibrationOutput(std::vector<std::string>& args);
	void setupMotor(std::vector<std::string>& args);
	void setupInfo(std::vector<std::string>& args);
	void setupHoming(std::vector<std::string>& args);

	void testMove(std::vector<std::string>& args);
	void testMoveAbsolute(std::vector<std::string>& args);
	void testLatency(std::vector<std::string>& args);
	void testEncoderOutput(std::vector<std::string>& args);
	void testEncoderMain(std::vector<std::string>& args);
	void registerWrite(std::vector<std::string>& args);
	void registerRead(std::vector<std::string>& args);
	void blink(std::vector<std::string>& args);
	void encoder(std::vector<std::string>& args);
	void bus(std::vector<std::string>& args);

	void clearErrors(std::vector<std::string>& args);
	void clearWarnings(std::vector<std::string>& args);
	void reset(std::vector<std::string>& args);

	void changeDefaultConfig(std::string bus, std::string device);
	mab::CANdleBaudrate_E checkSpeedForId(uint16_t id);

	uint8_t getNumericParamFromList(std::string& param, const std::vector<std::string>& list);

	bool checkErrors(uint16_t canId);

	template <class T>
	bool getField(mINI::INIStructure& cfg, mINI::INIStructure& ini, std::string category,
				  std::string field, T& value);

	template <typename T>
	bool readRegisterToString(uint16_t id, mab::Md80Reg_E regId, std::string& str)
	{
		T value = 0;
		bool status = candle->readMd80Register(id, regId, value);
		str = std::to_string(value);
		return status;
	}

	bool checkArgs(std::vector<std::string>& args, uint32_t size);
	bool tryAddMD80(uint16_t id);
	int checkArgsAndGetId(std::vector<std::string>& args, uint32_t size, uint32_t idPos);
	bool checkSetupError(uint16_t id);
};
