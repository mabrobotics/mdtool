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

	mab::CANdleBaudrate_E CurrentBaudrate;
	bool printVerbose = false;

	void ping(std::vector<std::string>& args);
	void configCan(std::vector<std::string>& args);
	void configSave(std::vector<std::string>& args);
	void configZero(std::vector<std::string>& args);
	void configCurrent(std::vector<std::string>& args);
	void configBandwidth(std::vector<std::string>& args);

	void setupCalibration(std::vector<std::string>& args);
	void setupCalibrationOutput(std::vector<std::string>& args);
	void setupDiagnostic(std::vector<std::string>& args);
	void setupMotor(std::vector<std::string>& args);
	void setupInfo(std::vector<std::string>& args);

	void testMove(std::vector<std::string>& args);
	void testMoveAbs(std::vector<std::string>& args);
	void testLatency(std::vector<std::string>& args);
	void testEncoderOutput(std::vector<std::string>& args);
	void testEncoderMain(std::vector<std::string>& args);
	void testHoming(std::vector<std::string>& args);
	void blink(std::vector<std::string>& args);
	void encoder(std::vector<std::string>& args);
	void bus(std::vector<std::string>& args);

	void changeDefaultConfig(std::string bus, std::string device);
	mab::CANdleBaudrate_E checkSpeedForId(uint16_t id);

	uint8_t getNumericParamFromList(std::string& param, const std::vector<std::string>& list);

	bool checkErrors(uint16_t canId);

	template <class T>
	bool getField(mINI::INIStructure& cfg, mINI::INIStructure& ini, std::string category, std::string field, T& value);
};
