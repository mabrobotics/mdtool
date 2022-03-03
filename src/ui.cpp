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
    vout << "\tmd_tools config zero 14 15 16 17" << std::endl;
    vout << "\tmd_tools config can 100 105 8M 100" << std::endl;
    vout << std::endl;
    vout << "Supported options: " << std::endl;
    vout << "\t zero [ids] \t\t\t\t sets current drive position as zero reference position." << std::endl;
    vout << "\t can [id] [new_id] [baudrate] [timeout]  \t changes FDCAN parameters of the drive. [id] - currend drive id, [new_id] - new id to be set " <<
        "[baudrate] - can be either 1M, 2M, 5M or 8M, [timeout] - FDCAN communication watchdog timeout in ms." << std::endl;
    vout << "\t TBD" << std::endl;
}
void printHelpSetup()
{
    vout << "This is Setup HELP menu..." << std::endl;
}

}