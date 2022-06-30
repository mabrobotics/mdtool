#include "ui.hpp"

#include <streambuf>
#include <iostream>

#define ERROR_BRIDGE_OCP  		    0    // 1
#define ERROR_BRIDGE_FAULT 		    1    // 2
#define ERROR_OUT_ENCODER_E		    2    // 4
#define ERROR_OUT_ENCODER_COM_E 	3    // 8
#define ERROR_PARAM_IDENT 			4    // 16           
#define ERROR_EMPTY4 			    5    // 32           //NOT USED YET
#define ERROR_EMPTY5			    6    // 64           //NOT USED YET
#define ERROR_EMPTY6			    7    // 128          //NOT USED YET

#define ERROR_UNDERVOLTAGE 		    8 	 // 256
#define ERROR_OVERVOLTAGE 		    9 	 // 512
#define ERROR_TEMP_W 			    10	 // 1024         //NOT USED YET
#define ERROR_TEMP_SD 			    11 	 // 2048
#define ERROR_CALIBRATION 		    12	 // 4096         //NOT USED YET
#define ERROR_OCD 				    13	 // 8092
#define ERROR_CAN_WD 			    14	 // 16384
#define ERROR_EMPTY7			    15   // 32768        //NOT USED YET

namespace ui
{
class mystreambuf: public std::streambuf {    };
mystreambuf nostreambuf;
std::ostream nocout(&nostreambuf);
#define vout (std::cout)    //For easy customization later on

void printTooFewArgs()
{
    vout << "Not enough arguments!" << std::endl;
    printHelp();
}

void printTooFewArgsNoHelp()
{
    vout << "Not enough arguments!" << std::endl;
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
    vout << "\tmdtool <command> [arguments | options]" << std::endl;
    vout << std::endl;
    vout << "Supported commands: " << std::endl;
    vout << "\t ping \t\t\t\t discovers all drives available on FDCAN bus." << std::endl;
    vout << "\t config [options] [arguments] \t sets configuration options. use `mdtool config` for more info." << std::endl;
    vout << "\t setup [options] \t\t launches a setup procedure. Use `mdtool setup` for more info." << std::endl;
    vout << "\t test [id] [position] \t\t simple test movement from current location to [position]. [position] should be <-10, 10> rad" << std::endl;
    vout << "\t blink [id] \t\t\t blink LEDs on driver board." << std::endl;
    vout << "\t encoder [id] \t\t\t prints current position and velocity in a loop." << std::endl;
    vout << std::endl;
    vout << "Add '-sv' after arguments to suppress output" << std::endl;
}
void printHelpConfig()
{
    vout << std::endl;
    vout << "Usage: " << std::endl;
    vout << "\tmdtool config [options] [arguments]" << std::endl;
    vout << std::endl;
    vout << "Example: " << std::endl;
    vout << "\tmdtool config zero 14" << std::endl;
    vout << "\tmdtool config can 100 105 8M 100" << std::endl;
    vout << "\tmdtool config save 105" << std::endl;
    vout << "\tmdtool config current 105 12.5" << std::endl;
    vout << std::endl;
    vout << "Supported options: " << std::endl;
    vout << "\t zero [id] \t\t\t\t sets current drive position as zero reference position." << std::endl;
    vout << "\t can [id] [new_id] [baudrate] [timeout]  changes FDCAN parameters of the drive. [id] - currend drive id, [new_id] - new id to be set " <<
        "[baudrate] - can be either 1M, 2M, 5M or 8M, [timeout] - FDCAN communication watchdog timeout in ms." << std::endl;
    vout << "\t save [id] \t\t\t\t saves current, can(FDCAN) and calibration config to flash, if changed." << std::endl;
    vout << "\t current [id] [current] \t\t sets max phase current the drive will output. Check md80 docs for more info. [current] - current limit in Amps." << std::endl;
}
void printHelpSetup()
{
    vout << std::endl;
    vout << "Usage: " << std::endl;
    vout << "\tmdtool setup [options] [arguments]" << std::endl;
    vout << std::endl;
    vout << "Example: " << std::endl;
    vout << "\tmdtool setup calibration 74 500" << std::endl;
    vout << "\tmdtool setup diagnostic 211" << std::endl;
    vout << std::endl;
    vout << "Supported options: " << std::endl;
    vout << "\t calibration [id] [torque bandwidth] \t Starts motor calibration procedure for a given torque bandwidth in Hz. For more information please refer to the manual." << std::endl;
    vout << "\t diagnostic  [id] \t\t\t Prints diagnostic information." << std::endl;
}
bool getCalibrationConfirmation()
{
    vout << "This step will start drive calibration. If calibration is done incorrectly or fails, it will result in undefined behaviour of the drive." << std::endl;
    vout << "The process takes around 40-50 seconds, and should not be cancelled or stopped." << std::endl;
    vout << "Ensure that the power supply's voltage is stable @24V and it is able to deliver >1A." << std::endl;
    vout << "For proper calibration, there mustn't be any load at the drives output shaft (rotor), ideally there shouldn't be anything attached to the output shaft." << std::endl;
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
void printPositionAndVelocity(int id, float pos, float velocity)
{
    vout << "Drive " << id << " Position: " << pos << "\tVelocity: " << velocity <<std::endl;
}
void printDriveInfo(int id, float pos, float vel, float torque, float temperature, unsigned short error)
{
    vout << "Drive " << id << " Position: " << pos << "\tVelocity: " << vel << "\tTorque: " << torque <<std::endl;
    vout << "Drive " << id << " Temp: " << temperature << "*C\tError: 0x" << std::hex <<(unsigned short)error << std::dec << std::endl;
    if(error != 0)
    {
        vout << "Errors: ";
        if (error & (1 << ERROR_BRIDGE_OCP))
            vout << "ERROR_BRIDGE_OCP, ";
        if (error & (1 << ERROR_BRIDGE_FAULT))
            vout << "ERROR_BRIDGE_FAULT, ";
        if (error & (1 << ERROR_OUT_ENCODER_E))
            vout << "ERROR_OUT_ENCODER_E, ";
        if (error & (1 << ERROR_OUT_ENCODER_COM_E))
            vout << "ERROR_OUT_ENCODER_COM_E, ";
        if (error & (1 << ERROR_PARAM_IDENT))
            vout << "ERROR_PARAM_IDENT, ";
        if (error & (1 << ERROR_UNDERVOLTAGE))
            vout << "ERROR_UNDERVOLTAGE, ";
        if (error & (1 << ERROR_OVERVOLTAGE))
            vout << "ERROR_OVERVOLTAGE, ";
        if (error & (1 << ERROR_TEMP_W))
            vout << "ERROR_TEMP_W, ";
        if (error & (1 << ERROR_TEMP_SD))
            vout << "ERROR_TEMP_SD, ";
        if (error & (1 << ERROR_CALIBRATION))
            vout << "ERROR_CALIBRATION, ";
        if (error & (1 << ERROR_OCD))
            vout << "ERROR_OCD, ";
        if (error & (1 << ERROR_CAN_WD))
            vout << "ERROR_CAN_WD, ";
        vout << std::endl;
    }
}

}
