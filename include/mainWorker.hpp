#pragma once

#include "bus.hpp"
#include "candle.hpp"
#include "mini/ini.h"

class MainWorker
{
   public:
	MainWorker(std::vector<std::string>& args);

   private:
#pragma pack(push, 1)
	typedef struct
	{
		char motorName[18];
		uint16_t canId;
		uint32_t canBaudrate;
		uint16_t canWdgPeriod;
		uint8_t polePairs;
		float motorKt;
		float motorKt_a;
		float motorKt_b;
		float motorKt_c;
		float iMax;
		float gearRatio;
		int outputEncoder;
		float outputEncoderDir;
		uint16_t torqueBandwidth;
		float friction;
		float stiction;
	} motorConfig_t;

	typedef struct
	{
		float kp;
		float kd;
		float outMax;
	} ImpedanceControllerGains_t;

	typedef struct
	{
		float kp;
		float ki;
		float kd;
		float intWindup;
		float outMax;
	} PidControllerGains_t;

	typedef struct
	{
		ImpedanceControllerGains_t impedancePdGains;
		PidControllerGains_t velocityPidGains;
		PidControllerGains_t positionPidGains;
	} motorMotionConfig_t;

#pragma pack(pop)
	typedef union
	{
		motorConfig_t s;
		uint8_t bytes[sizeof(motorConfig_t)];
	} motorConfig_ut;

	typedef union
	{
		motorMotionConfig_t s;
		uint8_t bytes[sizeof(motorMotionConfig_t)];
	} motorMotionConfig_ut;

	const std::string mdtoolHomeConfigDirName = ".config";
	const std::string mdtoolDirName = "mdtool";
	const std::string mdtoolMotorCfgDirName = "mdtool_motors";
	const std::string mdtoolIniFileName = "mdtool.ini";

	const std::string mdtoolConfigPath = "/etc/";

	std::string mdtoolBaseDir;
	std::string mdtoolIniFilePath;

	mab::Candle* candle;

	mab::CANdleBaudrate_E CurrentBaudrate;
	bool printVerbose = false;

	void ping(std::vector<std::string>& args);
	void configCan(std::vector<std::string>& args);
	void configSave(std::vector<std::string>& args);
	void configZero(std::vector<std::string>& args);
	void configCurrent(std::vector<std::string>& args);
	void configBandwidth(std::vector<std::string>& args);

	void setupCalibration(std::vector<std::string>& args);
	void setupDiagnostic(std::vector<std::string>& args);
	void setupMotor(std::vector<std::string>& args);
	void setupInfo(std::vector<std::string>& args);

	void testMove(std::vector<std::string>& args);
	void blink(std::vector<std::string>& args);
	void encoder(std::vector<std::string>& args);
	void bus(std::vector<std::string>& args);

	void changeDefaultConfig(std::string bus);
	mab::CANdleBaudrate_E checkSpeedForId(uint16_t id);
	void checkConfigLimits(motorConfig_ut* motorConfig, motorMotionConfig_ut* motorMotionConfig);

	template <class T>
	bool getField(mINI::INIStructure& cfg, mINI::INIStructure& ini, std::string category, std::string field, T& value);
};
