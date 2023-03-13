#pragma once

#include <string>
#include <vector>

#include "candle.hpp"
#include "mainWorker.hpp"

/* ERROR COLORING NOTE: may not work on all terminals! */
#define REDSTART	"\033[1;31m"
#define GREENSTART	"\033[1;32m"
#define YELLOWSTART "\033[1;33m"
#define RESETTEXT	"\033[0m"

#define RED(x)		REDSTART x RESETTEXT
#define RED_(x)		REDSTART + x + RESETTEXT

#define GREEN(x)	GREENSTART x RESETTEXT
#define GREEN_(x)	GREENSTART + x + RESETTEXT

#define YELLOW(x)	YELLOWSTART x RESETTEXT
#define YELLOW_(x)	YELLOWSTART + x + RESETTEXT

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
bool getCalibrationAuxConfirmation();
void printPosition(int id, float pos);
void printPositionAndVelocity(int id, float pos, float velocity);
void printDriveInfo(int id, float pos, float vel, float torque, float temperature, uint16_t error, mab::CANdleBaudrate_E baud);
void printScanOutput(mab::Candle* candle);
void printFoundDrives(std::vector<uint16_t> ids);
void printUnableToFindCfgFile(std::string path);
void printDriveInfoExtended(mab::Md80& drive, bool printAll);
void printAllErrors(mab::Md80& drive);
void printErrorDetails(uint32_t error, const std::vector<std::string>& errorList);
void printParameterOutOfBounds(std::string category, std::string field);
void printFailedToSetupMotor(mab::Md80Reg_E regId);

/* these are only used to light up the values in */
constexpr float outputEncoderStdDevMax = 0.05f;
constexpr float outputEncoderMaxError = 0.18f;

constexpr float mainEncoderStdDevMax = 0.012f;
constexpr float mainEncoderMaxError = 0.05f;

const std::vector<std::string> encoderTypes = {"NONE", "AS5047_CENTER", "AS5047_OFFAXIS"};
const std::vector<std::string> encoderModes = {"NONE", "STARTUP", "MOTION", "REPORT"};
const std::vector<std::string> encoderCalibrationModes = {"FULL", "DIRONLY"};
const std::vector<std::string> motorCalibrationModes = {"FULL", "NOPPDET"};
const std::vector<std::string> errorVectorList = {"ERROR_BRIDGE_OCP",
												  "ERROR_BRIDGE_FAULT",
												  "ERROR_OUT_ENCODER_E",
												  "ERROR_OUT_ENCODER_COM_E",
												  "ERROR_PARAM_IDENT",
												  "ERROR_MOTOR_SETUP",
												  "ERROR_MOTOR_POLE_PAIR_DET",
												  "ERROR_EMPTY6",
												  "ERROR_UNDERVOLTAGE",
												  "ERROR_OVERVOLTAGE",
												  "ERROR_MOTOR_TEMP",
												  "ERROR_MOSFET_TEMP",
												  "ERROR_CALIBRATION",
												  "ERROR_OCD",
												  "CAN_WD_TRIGGERED",
												  "ERROR_LOOPBACK"};

const std::vector<std::string> encoderErrorList = {"ERROR_COMMUNICATION",
												   "ERROR_WRONG_DIRECTION",
												   "ERROR_EMPTY_LUT",
												   "ERROR_FAULTY_LUT",
												   "ERROR_CALIBRATION_FAILED"};

const std::vector<std::string> calibrationErrorList = {"ERROR_OFFSET_CAL",
													   "ERROR_RESISTANCE_IDENT",
													   "ERROR_INDUCTANCE_IDENT",
													   "ERROR_POLE_PAIR_CAL",
													   "ERROR_SETUP"};

const std::vector<std::string> bridgeErrorList = {"ERROR_BRIDGE_COM",
												  "ERROR_BRIDGE_OC",
												  "ERROR_BRIDGE_GENERAL_FAULT"};

const std::vector<std::string> hardwareErrorList = {"ERROR_OVER_CURRENT",
													"ERROR_OVER_VOLTAGE",
													"ERROR_UNDER_VOLTAGE",
													"ERROR_MOTOR_TEMP",
													"ERROR_MOSFET_TEMP",
													"ERROR_ADC_CURRENT_OFFSETS"};

const std::vector<std::string> communicationErrorList = {"ERROR_CAN_WD"};

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