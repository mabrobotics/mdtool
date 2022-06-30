#pragma once

#include <string>

namespace ui
{
    void printTooFewArgs();
    void printTooFewArgsNoHelp();
    void printUnknownCmd(std::string arg);

    void printHelp();
    void printHelpConfig();
    void printHelpSetup();

    bool getCalibrationConfirmation();
    void printPosition(int id, float pos);
    void printPositionAndVelocity(int id, float pos, float velocity);
    void printDriveInfo(int id, float pos, float vel, float torque, float temperature, unsigned short error);
}