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