#include "ui.hpp"

#include <streambuf>
#include <iostream>

namespace ui
{

class mystreambuf: public std::streambuf {    };
mystreambuf nostreambuf;
std::ostream nocout(&nostreambuf);
#define vout (std::cout)    //For easy customization later on

void printTooFewArgs()
{
    vout << "Not enought arguments!" << std::endl;
    printHelp();
}

void printUnknownCmd(std::string arg = "")
{
    vout << "Command '" << arg << "' unknown!" << std::endl;
    printHelp();
}

void printHelp()
{
    vout << std::endl;
    vout << "Usage: " << std::endl;
    vout << "\tmd_tools <command> [arguments | options]" << std::endl;
    vout << std::endl;
    vout << "Supported commands: " << std::endl;
    vout << "\t ping \t\t\t\t discovers all drives available on FDCAN bus." << std::endl;
    vout << "\t config [options] [arguments] \t sets configuration options. use `md_tools config` for more info." << std::endl;
    vout << "\t setup [options] \t\t launches a setup procedues. Use `md_tools setup` for more info." << std::endl;
    vout << "\t test [id] [position] \t\t simple test movement from current location to [position]. [position] should be <-10, 10> rad" << std::endl;
    vout << "\t blink [id] \t\t\t blink LEDs on driver board." << std::endl;
    vout << std::endl;
    vout << "Add '-sv' after arguments to suppress outout" << std::endl;
}
void printHelpConfig()
{
    vout << std::endl;
    vout << "Usage: " << std::endl;
    vout << "\tmd_tools config [options] [arguments]" << std::endl;
    vout << std::endl;
    vout << "Example: " << std::endl;
    vout << "\tmd_tools config zero 14" << std::endl;
    vout << "\tmd_tools config can 100 105 8M 100" << std::endl;
    vout << "\tmd_tools config save 105" << std::endl;
    vout << "\tmd_tools config current 105 12.5" << std::endl;
    vout << std::endl;
    vout << "Supported options: " << std::endl;
    vout << "\t zero [id] \t\t\t\t sets current drive position as zero reference position." << std::endl;
    vout << "\t can [id] [new_id] [baudrate] [timeout]  \t changes FDCAN parameters of the drive. [id] - currend drive id, [new_id] - new id to be set " <<
        "[baudrate] - can be either 1M, 2M, 5M or 8M, [timeout] - FDCAN communication watchdog timeout in ms." << std::endl;
    vout << "\t save [id] \t\t\t\t saves current, can(FDCAN) and calibration config to flash, if changed." << std::endl;
    vout << "\t current [id] [current] \t\t sets max phase current the drive will outout. Check md80 docs for more info. [current] - current limit in Amps." << std::endl;
}
void printHelpSetup()
{
    vout << std::endl;
    vout << "Usage: " << std::endl;
    vout << "\tmd_tools setup [options] [arguments]" << std::endl;
    vout << std::endl;
    vout << "Example: " << std::endl;
    vout << "\tmd_tools setup calibration 74" << std::endl;
    vout << "\tmd_tools setup diagnostic 211" << std::endl;
    vout << std::endl;
    vout << "Supported options: " << std::endl;
    vout << "\t calibration [id] \t\t\t\t Starts motor calibration procedure." << std::endl;
    vout << "\t diagnostic  [id] \t\t\t\t Prints diagnostic information." << std::endl;
}
bool getCalibrationConfirmation()
{
    vout << "This step will start drive calibration. If calibration is done incorrectly or fails, it will result in undefined behaviour of the drive." << std::endl;
    vout << "The process takes around 40-50 seconds, and should not be cancelled or stopped." << std::endl;
    vout << "Ensure that the power supply's voltage is stable @24V and it's supply is capable to deliver >1A." << std::endl;
    vout << "For proper calibration, there mustn't be any load at the drives output shaft (rotor), ideally there shouldn't be anything attached to the outout shaft." << std::endl;
    vout << "Are you sure you want to start calibration? [Y/n]" << std::endl;
    char x;
    std::cin >> x;
    if(x != 'Y')
    {
        vout << "Calibration abandoned." << std::endl;
        return false;
    }
    return true;
}
void printPosition(int id, float pos)
{
    vout << "Drive " << id << " Position: " << pos << std::endl;
}

}