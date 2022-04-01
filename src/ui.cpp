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
    vout << "\t config [options] [arguments] \t sets configuration options. use md_tools config --help for more info." << std::endl;
    vout << "\t setup [options] \t\t launches a setup procedues. Use md_tools setup --help for more info." << std::endl;
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
    vout << std::endl;
    vout << "Supported options: " << std::endl;
    vout << "\t zero [ids] \t\t\t\t sets current drive position as zero reference position." << std::endl;
    vout << "\t can [id] [new_id] [baudrate] [timeout]  \t changes FDCAN parameters of the drive. [id] - currend drive id, [new_id] - new id to be set " <<
        "[baudrate] - can be either 1M, 2M, 5M or 8M, [timeout] - FDCAN communication watchdog timeout in ms." << std::endl;
    vout << "\t save [ids] \t\t\t\t saves current can (FDCAN) config to flash." << std::endl;
    vout << "\t TBD" << std::endl;
}
void printHelpSetup()
{
    vout << "This is Setup HELP menu..." << std::endl;
}
bool getCalibrationConfirmation()
{
    vout << "This step will start drive calibration. If calibration is done incorrectly or fails, it will result in undefined behaviour of the drive." << std::endl;
    vout << "The process takes around 40-50 seconds, and should not be cancelled or stopped." << std::endl;
    vout << "Ensure that the power supply's voltage is stable (between 23.5V and 24.5V) and it's current capacity is >1A." << std::endl;
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
}