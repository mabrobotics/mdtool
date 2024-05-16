#pragma once

#include <map>
#include <string>
#include <vector>

#include "candle.hpp"
#include "mainWorker.hpp"

/* ERROR COLORING NOTE: may not work on all terminals! */
#define REDSTART	"\033[1;31m"
#define GREENSTART	"\033[1;32m"
#define YELLOWSTART "\033[1;33m"
#define RESETTEXT	"\033[0m"

#define RED(x)	REDSTART x RESETTEXT
#define RED_(x) REDSTART + x + RESETTEXT

#define GREEN(x)  GREENSTART x RESETTEXT
#define GREEN_(x) GREENSTART + x + RESETTEXT

#define YELLOW(x)  YELLOWSTART x RESETTEXT
#define YELLOW_(x) YELLOWSTART + x + RESETTEXT

namespace ui
{

	void printTooFewArgs();
	void printTooFewArgsNoHelp();
	void printWrongArgumentsSpecified();
	void printUnknownCmd(std::string arg);

	void printHelp();
	void printHelpConfig();
	void printHelpSetup();
	void printHelpTest();
	void printLatencyTestResult(uint8_t actuatorCount, float average, float stdev, std::string bus);

	bool getCalibrationConfirmation();
	bool getCalibrationOutputConfirmation();
	void printPosition(int id, float pos);
	void printPositionAndVelocity(int id, float pos, float velocity);
	void printDriveInfo(int id, float pos, float vel, float torque, float temperature,
						uint16_t error, mab::CANdleBaudrate_E baud);
	void printScanOutput(mab::Candle* candle);
	void printFoundDrives(std::vector<uint16_t> ids);
	void printUnableToFindCfgFile(std::string path);
	void printDriveInfoExtended(mab::Md80& drive, bool printAll);
	void printAllErrors(mab::Md80& drive);
	void printErrorDetails(uint32_t error, const std::map<std::string, uint8_t>& errorMap);
	void printParameterOutOfBounds(std::string category, std::string field);
	void printFailedToSetupMotor(mab::Md80Reg_E regId);

	bool getDifferentConfigsConfirmation(std::string configName);
	bool getUpdateConfigConfirmation(std::string configName);

	/* these are only used to light up the values in */
	constexpr float outputEncoderStdDevMax = 0.05f;
	constexpr float outputEncoderMaxError = 0.18f;

	constexpr float mainEncoderStdDevMax = 0.012f;
	constexpr float mainEncoderMaxError = 0.05f;

	const std::vector<std::string> encoderTypes = {
		"NONE",		  "AS5047_CENTER", "AS5047_OFFAXIS", "MB053SFA17BENT00",
		"CM_OFFAXIS", "M24B_CENTER",   "M24B_OFFAXIS"};
	const std::vector<std::string> encoderModes = {"NONE", "STARTUP", "MOTION", "REPORT", "MAIN"};
	const std::vector<std::string> encoderCalibrationModes = {"FULL", "DIRONLY"};
	const std::vector<std::string> motorCalibrationModes = {"FULL", "NOPPDET"};
	const std::vector<std::string> homingModes = {"OFF", "SENSORLESS"};
	const std::vector<std::string> brakeModes = {"OFF", "AUTO", "MANUAL"};
	const std::vector<std::string> errorVectorList = {
		"ERROR_BRIDGE_OCP",			 "ERROR_BRIDGE_FAULT",
		"ERROR_OUT_ENCODER_E",		 "ERROR_OUT_ENCODER_COM_E",
		"ERROR_PARAM_IDENT",		 "ERROR_MOTOR_SETUP",
		"ERROR_MOTOR_POLE_PAIR_DET", "ERROR_EMPTY6",
		"ERROR_UNDERVOLTAGE",		 "ERROR_OVERVOLTAGE",
		"ERROR_MOTOR_TEMP",			 "ERROR_MOSFET_TEMP",
		"ERROR_CALIBRATION",		 "ERROR_OCD",
		"CAN_WD_TRIGGERED",			 "ERROR_LOOPBACK"};

	const std::map<std::string, uint8_t> encoderErrorList = {
		{"ERROR_COMMUNICATION", 0}, {"ERROR_WRONG_DIRECTION", 1},	 {"ERROR_EMPTY_LUT", 2},
		{"ERROR_FAULTY_LUT", 3},	{"ERROR_CALIBRATION_FAILED", 4}, {"ERROR_POSITON_INVALID", 5},
		{"ERROR_INIT", 6},			{"WARNING_LOW_ACCURACY", 30}};

	const std::map<std::string, uint8_t> calibrationErrorList = {{"ERROR_OFFSET_CAL", 0},
																 {"ERROR_RESISTANCE_IDENT", 1},
																 {"ERROR_INDUCTANCE_IDENT", 2},
																 {"ERROR_POLE_PAIR_CAL", 3},
																 {"ERROR_SETUP", 4}};

	const std::map<std::string, uint8_t> bridgeErrorList = {
		{"ERROR_BRIDGE_COM", 0}, {"ERROR_BRIDGE_OC", 1}, {"ERROR_BRIDGE_GENERAL_FAULT", 2}};

	const std::map<std::string, uint8_t> hardwareErrorList = {
		{"ERROR_OVER_CURRENT", 0}, {"ERROR_OVER_VOLTAGE", 1}, {"ERROR_UNDER_VOLTAGE", 2},
		{"ERROR_MOTOR_TEMP", 3},   {"ERROR_MOSFET_TEMP", 4},  {"ERROR_ADC_CURRENT_OFFSETS", 5}};

	const std::map<std::string, uint8_t> communicationErrorList = {{"WARNING_CAN_WD", 30}};

	const std::map<std::string, uint8_t> homingErrorList = {{"ERROR_HOMING_LIMIT_REACHED", 0},
															{"ERROR_HOMING_SEQUENCE", 1},
															{"ERROR_HOMING_REQUIRED", 2},
															{"ERROR_HOMING_SETUP", 3},
															{"ERROR_HOMING_ABORTED", 4}};

	const std::map<std::string, uint8_t> motionErrorList = {
		{"ERROR_POSITION_OUTSIDE_LIMITS", 0}, {"ERROR_VELOCITY_OUTSIDE_LIMITS", 1},
		{"WARNING_ACCELERATION_CLIPPED", 24}, {"WARNING_TORQUE_CLIPPED", 25},
		{"WARNING_VELOCITY_CLIPPED", 26},	  {"WARNING_POSITION_CLIPPED", 27}};

	template <class T>
	bool checkParamLimit(T value, T min, T max)
	{
		if (value > max)
			return false;
		if (value < min)
			return false;
		return true;
	}
}  // namespace ui
