#pragma once

#include <string>
#include <vector>

#include "candle.hpp"
#include "mainWorker.hpp"

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
void printPosition(int id, float pos);
void printPositionAndVelocity(int id, float pos, float velocity);
void printDriveInfo(int id, float pos, float vel, float torque, float temperature, unsigned short error, mab::CANdleBaudrate_E baud);
void printScanOutput(mab::Candle* candle);
void printFoundDrives(std::vector<uint16_t> ids);
void printUnableToFindCfgFile(std::string path);
void printDriveInfoExtended(mab::Md80& drive);
void printErrorDetails(unsigned short error);
void printParameterOutOfBounds(std::string category, std::string field);
void printFailedToSetupMotor();

const std::vector<std::string> encoderTypes = {"NONE", "AMS5047_CENTER", "AMS5047_OFFAXIS"};
const std::vector<std::string> encoderModes = {"NONE", "STARTUP", "MOTION", "REPORT"};

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