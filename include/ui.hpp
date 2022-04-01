#pragma once

#include <string>

namespace ui
{
    void printTooFewArgs();
    void printUnknownCmd(std::string arg);

    void printHelp();
    void printHelpConfig();
    void printHelpSetup();

    bool getCalibrationConfirmation();
}