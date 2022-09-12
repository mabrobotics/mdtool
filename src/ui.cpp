#include "ui.hpp"

#include <iostream>
#include <streambuf>

#define ERROR_BRIDGE_OCP		0  // 1
#define ERROR_BRIDGE_FAULT		1  // 2
#define ERROR_OUT_ENCODER_E		2  // 4
#define ERROR_OUT_ENCODER_COM_E 3  // 8
#define ERROR_PARAM_IDENT		4  // 16
#define ERROR_EMPTY4			5  // 32           //NOT USED YET
#define ERROR_EMPTY5			6  // 64           //NOT USED YET
#define ERROR_EMPTY6			7  // 128          //NOT USED YET

#define ERROR_UNDERVOLTAGE		8	// 256
#define ERROR_OVERVOLTAGE		9	// 512
#define ERROR_TEMP_W			10	// 1024         //NOT USED YET
#define ERROR_TEMP_SD			11	// 2048
#define ERROR_CALIBRATION		12	// 4096         //NOT USED YET
#define ERROR_OCD				13	// 8092
#define ERROR_CAN_WD			14	// 16384
#define ERROR_EMPTY7			15	// 32768        //NOT USED YET

namespace ui
{
class mystreambuf : public std::streambuf
{
};
mystreambuf nostreambuf;
std::ostream nocout(&nostreambuf);
#define vout (std::cout)  // For easy customization later on

void printTooFewArgs()
{
	vout << "Not enough arguments!" << std::endl;
	printHelp();
}

void printTooFewArgsNoHelp()
{
	vout << "Not enough arguments!" << std::endl;
}

void printWrongArgumentsSpecified()
{
	vout << "Wrong arguments specified!" << std::endl;
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
	vout << "\t ping [can speed] \t\t discovers all drives available on FDCAN bus at [can speed] (1M/2M/5M/8M), use 'all' keyword for scanning all speeds at once." << std::endl;
	vout << "\t config [options] [arguments] \t sets configuration options. use `mdtool config` for more info." << std::endl;
	vout << "\t setup [options] \t\t launches a setup procedure. Use `mdtool setup` for more info." << std::endl;
	vout << "\t test [id] [position] \t\t simple test movement from current location to [position]. [position] should be <-10, 10> rad." << std::endl;
	vout << "\t blink [id] \t\t\t blink LEDs on driver board." << std::endl;
	vout << "\t encoder [id] \t\t\t prints current position and velocity in a loop." << std::endl;
	vout << "\t bus [type] \t\t\t changes default CANdle CAN bus [type] (SPI/USB/UART)." << std::endl;
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
	vout << "\tmdtool config bandwidth 105 500" << std::endl;
	vout << std::endl;
	vout << "Supported options: " << std::endl;
	vout << "\t zero [id] \t\t\t\t sets current drive position as zero reference position." << std::endl;
	vout << "\t can [id] [new_id] [baudrate] [timeout]  changes FDCAN parameters of the drive. [id] - currend drive id, [new_id] - new id to be set."
		 << "[baudrate] - can be either 1M, 2M, 5M or 8M, [timeout] - FDCAN communication watchdog timeout in ms." << std::endl;
	vout << "\t save [id] \t\t\t\t saves parameters to flash memory." << std::endl;
	vout << "\t current [id] [current] \t\t sets max phase current the drive will output. Check MD80 docs for more info. [current] - current limit in Amps." << std::endl;
	vout << "\t bandwidth [id] \t\t\t\t sets the torque bandwidth parameter" << std::endl;
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
	vout << "\tmdtool setup motor 100 AK80-9.cfg" << std::endl;
	vout << "\tmdtool setup info 100" << std::endl;
	vout << std::endl;
	vout << "Supported options: " << std::endl;
	vout << "\t calibration [id] [torque bandwidth] \t Starts motor calibration procedure for a given torque bandwidth in Hz. For more information please refer to the manual." << std::endl;
	vout << "\t diagnostic  [id] \t\t\t Prints diagnostic information." << std::endl;
	vout << "\t motor  [id] [*.cfg] \t\t\t loads selected config." << std::endl;
	vout << "\t info  [id] \t\t\t Prints detailed info about controller." << std::endl;
}
bool getCalibrationConfirmation()
{
	vout << "This step will start drive calibration. If calibration is done incorrectly or fails the drive will not move. In such case please rerun the calibration and if the problem persists contact MABRobotics." << std::endl;
	vout << "The process takes around 40-50 seconds, and should not be cancelled or stopped." << std::endl;
	vout << "Ensure that the power supply's voltage is stable @24V and it is able to deliver >1A." << std::endl;
	vout << "For proper calibration, there mustn't be any load at the drives output shaft (rotor), ideally there shouldn't be anything attached to the output shaft." << std::endl;
	vout << "Are you sure you want to start calibration? [Y/n]" << std::endl;
	char x;
	std::cin >> x;
	if (x != 'Y')
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
	vout << "Drive " << id << " Position: " << pos << "\tVelocity: " << velocity << std::endl;
}
void printDriveInfo(int id, float pos, float vel, float torque, float temperature, unsigned short error, mab::CANdleBaudrate_E baud)
{
	vout << "Drive " << id << ":" << std::endl;
	vout << "- CAN speed: " << baud << "M" << std::endl;
	vout << "- position: " << pos << " rad" << std::endl;
	vout << "- velocity: " << vel << " rad/s" << std::endl;
	vout << "- torque: " << torque << " Nm" << std::endl;
	vout << "- temperature: " << temperature << " *C" << std::endl;
	vout << "- error: 0x" << std::hex << (unsigned short)error << std::dec;
	printErrorDetails(error);
}

void printScanOutput(mab::Candle* candle)
{
	std::cout << "[CANDLE] Pinging drives at 1M CAN speed..." << std::endl;
	ui::printFoundDrives(candle->ping(mab::CANdleBaudrate_E::CAN_BAUD_1M));
	std::cout << "[CANDLE] Pinging drives at 2M CAN speed..." << std::endl;
	ui::printFoundDrives(candle->ping(mab::CANdleBaudrate_E::CAN_BAUD_2M));
	std::cout << "[CANDLE] Pinging drives at 5M CAN speed..." << std::endl;
	ui::printFoundDrives(candle->ping(mab::CANdleBaudrate_E::CAN_BAUD_5M));
	std::cout << "[CANDLE] Pinging drives at 8M CAN speed..." << std::endl;
	ui::printFoundDrives(candle->ping(mab::CANdleBaudrate_E::CAN_BAUD_8M));
}

void printFoundDrives(std::vector<uint16_t> ids)
{
	if (ids.size() == 0)
	{
		vout << "No drives found." << std::endl;
		return;
	}
	vout << "Found drives." << std::endl;
	for (size_t i = 0; i < ids.size(); i++)
	{
		if (ids[i] == 0)
			break;	// No more ids in the message

		vout << std::to_string(i + 1) << ": ID = " << ids[i] << " (0x" << std::hex << ids[i] << std::dec << ")" << std::endl;
		if (ids[i] > 2047)
		{
			vout << "Error! This ID is invalid! Probably two or more drives share same ID."
				 << "Communication will most likely be broken until IDs are unique! [FAILED] " << std::endl;
			std::vector<uint16_t> empty;
			return;
		}
	}
}

void printUnableToFindCfgFile(std::string path)
{
	vout << "Unable to find selected config file. Received location: " + path << std::endl;
}

void printDriveInfoExtended(mab::motorParameters_ut* motorParameters, int id, float pos, float vel, float torque, float temperature, unsigned short error, mab::CANdleBaudrate_E baud)
{
	auto getStringBuildDate = [](uint32_t date)
	{ return std::to_string(date % 100) + '.' + std::to_string((date / 100) % 100) + '.' + "20" + std::to_string(date / 10000); };

	vout << "Drive " << id << ":" << std::endl;
	vout << "- actuator name: " << motorParameters->s.motorName << std::endl;
	vout << "- firmware version: V" << motorParameters->s.firmwareVersion / 10 << "." << motorParameters->s.firmwareVersion % 10 << std::endl;
	vout << "- build date: " << getStringBuildDate(motorParameters->s.buildDate) << std::endl;
	vout << "- commit hash: " << motorParameters->s.commitHash << std::endl;
	vout << "- max current: " << motorParameters->s.iMax << " A" << std::endl;
	vout << "- bridge type: " << std::to_string(motorParameters->s.DRVType) << std::endl;
	vout << "- d-axis resistance: " << motorParameters->s.resistance << " Ohm" << std::endl;
	vout << "- d-axis inductance: " << motorParameters->s.inductance << " H" << std::endl;
	vout << "- torque bandwidth: " << motorParameters->s.torqueBandwidth << " Hz" << std::endl;
	vout << "- CAN speed: " << baud << " M" << std::endl;
	vout << "- position: " << pos << " rad" << std::endl;
	vout << "- velocity: " << vel << " rad/s" << std::endl;
	vout << "- torque: " << torque << " Nm" << std::endl;
	vout << "- temperature: " << temperature << " *C" << std::endl;
	vout << "- error: 0x" << std::hex << (unsigned short)error << std::dec;
	printErrorDetails(error);
}

void printErrorDetails(unsigned short error)
{
	if (error != 0)
	{
		vout << "  (";
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
		vout << ")";
	}
	vout << std::endl;
}

void printParameterOutOfBounds(std::string category, std::string field)
{
	vout << "Motor config parameter in category [" << category << "] named [" << field << "] is out of bounds!" << std::endl;
}

void printFailedToSetupMotor()
{
	vout << "Failed to setup motor!" << std::endl;
}

}  // namespace ui
