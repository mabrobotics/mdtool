#include "ui.hpp"

#include <iomanip>
#include <iostream>
#include <streambuf>

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
	vout << "[MDTOOL] Not enough arguments!" << std::endl;
	printHelp();
}

void printTooFewArgsNoHelp()
{
	vout << "[MDTOOL] Not enough arguments!" << std::endl;
}

void printWrongArgumentsSpecified()
{
	vout << "[MDTOOL] Wrong arguments specified!" << std::endl;
}

void printUnknownCmd(std::string arg = "")
{
	vout << "[MDTOOL] Command '" << arg << "' unknown!" << std::endl;
	printHelp();
}

void printHelp()
{
	vout << std::endl;
	vout << "Usage: " << std::endl;
	vout << "\tmdtool <command> [options | arguments]" << std::endl;
	vout << std::endl;
	vout << "Supported commands: " << std::endl;
	vout << "\t ping [can speed] \t\t discovers all drives available on FDCAN bus at [can speed] (1M/2M/5M/8M), use 'all' keyword for scanning all speeds at once." << std::endl;
	vout << "\t config [options] [arguments] \t sets configuration options. use `mdtool config` for more info." << std::endl;
	vout << "\t setup [options] [arguments] \t launches a setup procedure. Use `mdtool setup` for more info." << std::endl;
	vout << "\t test [options] [arguments] \t tests the setup. use 'mdtool test' for more info" << std::endl;
	vout << "\t blink [id] \t\t\t blink LEDs on driver board." << std::endl;
	vout << "\t encoder [id] \t\t\t prints current position and velocity in a loop." << std::endl;
	vout << "\t bus [type] [device]\t\t changes default CANdle CAN bus [type] (SPI/USB/UART) and optionally [device] if default is not suitable (applies to UART and SPI)." << std::endl;
	vout << std::endl;
	vout << std::endl;
	vout << "For more information please refer to the manual:" << GREEN(" https://mabrobotics.pl/servos/manual");
	vout << std::endl;
	vout << std::endl;
}
void printHelpConfig()
{
	vout << std::endl;
	vout << "Usage: " << std::endl;
	vout << "\tmdtool config [options] [arguments]" << std::endl;
	vout << std::endl;
	vout << "Example: " << std::endl;
	vout << "\tmdtool config zero 14" << std::endl;
	vout << "\tmdtool config can 100 105 8M 100 0" << std::endl;
	vout << "\tmdtool config save 105" << std::endl;
	vout << "\tmdtool config current 105 12.5" << std::endl;
	vout << "\tmdtool config bandwidth 105 500" << std::endl;
	vout << std::endl;
	vout << "Supported options: " << std::endl;
	vout << "\t zero [id] \t\t\t\t sets current drive position as zero reference position." << std::endl;
	vout << "\t can [id] [new_id] [baudrate] [timeout] [termination] changes FDCAN parameters of the drive. [id] - currend drive id, [new_id] - new id to be set."
		 << "[baudrate] - can be either 1M, 2M, 5M or 8M, [timeout] - FDCAN communication watchdog timeout in ms, [termination] - works from HW V2.0 - 1 to turn on, 0 to turn off" << std::endl;
	vout << "\t save [id] \t\t\t\t saves parameters to flash memory." << std::endl;
	vout << "\t current [id] [current] \t\t sets max phase current the drive will output. Check MD80 docs for more info. [current] - current limit in Amps." << std::endl;
	vout << "\t bandwidth [id] \t\t\t sets the torque bandwidth parameter" << std::endl;
	vout << std::endl;
	vout << "For more information please refer to the manual:" << GREEN(" https://mabrobotics.pl/servos/manual") << std::endl;
	vout << std::endl;
}
void printHelpSetup()
{
	vout << std::endl;
	vout << "Usage: " << std::endl;
	vout << "\tmdtool setup [options] [arguments]" << std::endl;
	vout << std::endl;
	vout << "Example: " << std::endl;
	vout << "\tmdtool setup calibration 100" << std::endl;
	vout << "\tmdtool setup calibration_out 100" << std::endl;
	vout << "\tmdtool setup motor AK80-9.cfg 100" << std::endl;
	vout << "\tmdtool setup info 100" << std::endl;
	vout << std::endl;
	vout << "Supported options: " << std::endl;
	vout << "\t calibration [id] \t\t\tstarts motor calibration procedure." << std::endl;
	vout << "\t calibration_out [id] \t\t\tstarts output encoder calibration procedure." << std::endl;
	vout << "\t motor [*.cfg] [id] \t\t\tloads selected motor config." << std::endl;
	vout << "\t info  [id] \t\t\t\tprints detailed info about controller." << std::endl;
	vout << std::endl;
	vout << "For more information please refer to the manual:" << GREEN(" https://mabrobotics.pl/servos/manual") << std::endl;
	vout << std::endl;
}
void printHelpTest()
{
	vout << std::endl;
	vout << "Usage: " << std::endl;
	vout << "\tmdtool test [options] [arguments]" << std::endl;
	vout << std::endl;
	vout << "Example: " << std::endl;
	vout << "\tmdtool test move 100 5" << std::endl;
	vout << "\tmdtool test latency 1M" << std::endl;
	vout << "\tmdtool test encoder main 100" << std::endl;
	vout << "\tmdtool test encoder output 100" << std::endl;
	vout << std::endl;
	vout << "Supported options: " << std::endl;
	vout << "\t move [id] [position] \t\t\tsimple test movement from current location to [position]. [position] should be <-10, 10> rad." << std::endl;
	vout << "\t latency  [baudrate] \t\t\ttests the overall TX message frequency. [baudrate] should be the baudrate of actuators on the CAN bus." << std::endl;
	vout << "\t encoder  [type] [id] \t\t\ttests the encoder [type] (main/output)" << std::endl;
	vout << std::endl;
	vout << "For more information please refer to the manual:" << GREEN("https://mabrobotics.pl/servos/manual") << std::endl;
	vout << std::endl;
}

void printLatencyTestResult(uint8_t actuatorCount, float average, float stdev, std::string bus)
{
	vout << std::fixed;
	vout << "******************************************************************************************************************************" << std::endl;
	vout << std::endl;
	vout << "Communication speed results during 10s test for " << unsigned(actuatorCount) << " actuators and " << bus << " bus" << std::endl;
	vout << "Average speed: " << std::setprecision(2) << average << "Hz" << std::endl;
	vout << "Standard deviation: " << std::setprecision(2) << stdev << "Hz" << std::endl;
	vout << std::endl;
	vout << "Please note: the result is highly dependent on your PC hardware and reflects the PC <> CANdle rather than PC <> MD80 communication speed." << std::endl;
	vout << "For more information on this test please refer to the manual: " << GREEN("https://mabrobotics.pl/servos/manual") << std::endl;
	vout << std::endl;
	vout << "******************************************************************************************************************************" << std::endl;
}

bool getCalibrationConfirmation()
{
	vout << "This step will start drive calibration. If calibration is done incorrectly or fails the drive will not move. In such case please rerun the calibration and if the problem persists contact MABRobotics." << std::endl;
	vout << "The process takes around 40-50 seconds, and should not be cancelled or stopped." << std::endl;
	vout << "Ensure that the power supply's voltage is stable @24V and it is able to deliver more than 1A of current." << std::endl;
	vout << "For proper calibration, there mustn't be any load on the actuator's output shaft, ideally there shouldn't be anything attached to the output shaft." << std::endl;
	vout << std::endl;
	vout << "For more information please refer to the manual: " << GREEN("https://mabrobotics.pl/servos/manual") << std::endl;
	vout << std::endl;
	vout << "Are you sure you want to start the calibration? [Y/n]" << std::endl;
	char x;
	std::cin >> x;
	if (x != 'Y')
	{
		vout << "Calibration abandoned." << std::endl;
		return false;
	}
	return true;
}

bool getCalibrationAuxConfirmation()
{
	vout << "This step will start output encoder calibration." << std::endl;
	vout << "The process takes around 40-50 seconds, and should not be cancelled or stopped." << std::endl;
	vout << "Ensure that the power supply's voltage is stable and it is able to deliver more than 1A of current." << std::endl;
	vout << "The actuator output shaft (after the gearbox) will move - make sure it is able to rotate for at least two full rotations." << std::endl;
	vout << std::endl;
	vout << "For more information please refer to the manual: " << GREEN("https://mabrobotics.pl/servos/manual") << std::endl;
	vout << std::endl;
	vout << "Are you sure you want to start the calibration? [Y/n]" << std::endl;
	char x;
	std::cin >> x;
	if (x != 'Y')
	{
		vout << "Output encoder calibration abandoned." << std::endl;
		return false;
	}
	return true;
}

void printPositionVelocity(int id, float pos)
{
	vout << "Drive " << id << " Position: " << pos << std::endl;
}
void printPositionAndVelocity(int id, float pos, float velocity)
{
	vout << "Drive " << id << " Position: " << pos << "\tVelocity: " << velocity << std::endl;
}
void printDriveInfo(int id, float pos, float vel, float torque, float temperature, uint16_t error, mab::CANdleBaudrate_E baud)
{
	vout << "Drive " << id << ":" << std::endl;
	vout << "- CAN speed: " << baud << "M" << std::endl;
	vout << "- position: " << pos << " rad" << std::endl;
	vout << "- velocity: " << vel << " rad/s" << std::endl;
	vout << "- torque: " << torque << " Nm" << std::endl;
	vout << "- temperature: " << temperature << " *C" << std::endl;
	vout << "- error: 0x" << std::hex << (unsigned short)error << std::dec;
	printErrorDetails(error, ui::bridgeErrorList);
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

void printDriveInfoExtended(mab::Md80& drive)
{
	auto getStringBuildDate = [](uint32_t date)
	{ return std::to_string(date % 100) + '.' + std::to_string((date / 100) % 100) + '.' + "20" + std::to_string(date / 10000); };

	auto getHardwareVersion = [](uint8_t version)
	{
		switch (version)
		{
			case 0:
				return "HV13";
			case 1:
				return "HW11";
			case 2:
				return "HW20";
			default:
				return "UNKNOWN";
		}
	};

	vout << std::fixed;
	vout << "Drive " << drive.getId() << ":" << std::endl;
	vout << "- actuator name: " << drive.getReadReg().RW.motorName << std::endl;
	vout << "- CAN speed: " << drive.getReadReg().RW.canBaudrate / 1000000 << " M" << std::endl;
	vout << "- CAN termination resistor: " << ((drive.getReadReg().RW.canTermination == true) ? "enabled" : "disabled") << std::endl;
	vout << "- gear ratio: " << std::setprecision(3) << drive.getReadReg().RW.gearRatio << std::endl;
	vout << "- firmware version: V" << drive.getReadReg().RO.firmwareVersion / 10 << "." << drive.getReadReg().RO.firmwareVersion % 10 << std::endl;
	vout << "- hardware version: " << getHardwareVersion(drive.getReadReg().RO.hardwareVersion) << std::endl;
	vout << "- build date: " << getStringBuildDate(drive.getReadReg().RO.buildDate) << std::endl;
	vout << "- commit hash: " << drive.getReadReg().RO.commitHash << std::endl;
	vout << "- max current: " << std::setprecision(1) << drive.getReadReg().RW.iMax << " A" << std::endl;
	vout << "- bridge type: " << std::to_string(drive.getReadReg().RO.bridgeType) << std::endl;
	vout << "- pole pairs: " << std::to_string(drive.getReadReg().RW.polePairs) << std::endl;
	vout << "- KV rating: " << std::to_string(drive.getReadReg().RW.motorKV) << " rpm/V" << std::endl;
	vout << "- motor shutdown temperature: " << std::to_string(drive.getReadReg().RW.motorShutdownTemp) << " *C" << std::endl;
	vout << "- motor torque constant: " << std::setprecision(4) << drive.getReadReg().RW.motorKt << " Nm/A" << std::endl;
	vout << "- motor stiction: " << std::setprecision(3) << drive.getReadReg().RW.stiction << " Nm" << std::endl;
	vout << "- motor friction: " << std::setprecision(3) << drive.getReadReg().RW.friction << " Nm" << std::endl;
	vout << "- d-axis resistance: " << std::setprecision(3) << drive.getReadReg().RO.resistance << " Ohm" << std::endl;
	vout << "- d-axis inductance: " << std::setprecision(6) << drive.getReadReg().RO.inductance << " H" << std::endl;
	vout << "- torque bandwidth: " << drive.getReadReg().RW.torqueBandwidth << " Hz" << std::endl;
	vout << "- CAN watchdog: " << drive.getReadReg().RW.canWatchdog << " ms" << std::endl;

	float stddevE = drive.getReadReg().RO.calMainEncoderStdDev;
	float minE = drive.getReadReg().RO.calMainEncoderMinE;
	float maxE = drive.getReadReg().RO.calMainEncoderMaxE;
	vout << "- main encoder last check error stddev: " << (stddevE < mainEncoderStdDevMax ? std::to_string(stddevE) : YELLOW_(std::to_string(stddevE))) << " rad" << std::endl;
	vout << "- main encoder last check min error " << (minE > -mainEncoderMaxError ? std::to_string(minE) : YELLOW_(std::to_string(minE))) << " rad" << std::endl;
	vout << "- main encoder last check max error: " << (maxE < mainEncoderMaxError ? std::to_string(maxE) : YELLOW_(std::to_string(maxE))) << " rad" << std::endl;

	vout << "- output encoder: " << (drive.getReadReg().RW.outputEncoder ? encoderTypes[drive.getReadReg().RW.outputEncoder] : "no") << std::endl;

	if (drive.getReadReg().RW.outputEncoder != 0)
	{
		float stddevE = drive.getReadReg().RO.calOutputEncoderStdDev;
		float minE = drive.getReadReg().RO.calOutputEncoderMinE;
		float maxE = drive.getReadReg().RO.calOutputEncoderMaxE;
		vout << "- output encoder mode: " << encoderModes[drive.getReadReg().RW.outputEncoderMode] << std::endl;
		vout << "- output encoder position: " << drive.getReadReg().RO.outputEncoderPosition << " rad" << std::endl;
		vout << "- output encoder velocity: " << drive.getReadReg().RO.outputEncoderVelocity << " rad/s" << std::endl;
		vout << "- output encoder last check error stddev: " << (stddevE < outputEncoderStdDevMax ? std::to_string(stddevE) : YELLOW_(std::to_string(stddevE))) << " rad" << std::endl;
		vout << "- output encoder last check min error " << (minE > -outputEncoderMaxError ? std::to_string(minE) : YELLOW_(std::to_string(minE))) << " rad" << std::endl;
		vout << "- output encoder last check max error: " << (maxE < outputEncoderMaxError ? std::to_string(maxE) : YELLOW_(std::to_string(maxE))) << " rad" << std::endl;
	}
	vout << "- position: " << std::setprecision(2) << drive.getPosition() << " rad" << std::endl;
	vout << "- velocity: " << std::setprecision(2) << drive.getVelocity() << " rad/s" << std::endl;
	vout << "- torque: " << std::setprecision(2) << drive.getTorque() << " Nm" << std::endl;
	vout << "- MOSFET temperature: " << std::setprecision(2) << drive.getReadReg().RO.mosfetTemperature << " *C" << std::endl;
	vout << "- motor temperature: " << std::setprecision(2) << drive.getReadReg().RO.motorTemperature << " *C" << std::endl;
	vout << std::endl;

	vout << "***** ERRORS *****" << std::endl;
	vout << "- general error: 	0x" << std::hex << (unsigned short)drive.getReadReg().RO.errorVector << std::dec;
	printErrorDetails(drive.getReadReg().RO.errorVector, errorVectorList);
	vout << "- main encoder error: 	0x" << std::hex << (unsigned short)drive.getReadReg().RO.mainEncoderErrors << std::dec;
	printErrorDetails(drive.getReadReg().RO.mainEncoderErrors, encoderErrorList);

	if (drive.getReadReg().RW.outputEncoder != 0)
	{
		vout << "- output encoder error: 0x" << std::hex << (unsigned short)drive.getReadReg().RO.auxEncoderErrors << std::dec;
		printErrorDetails(drive.getReadReg().RO.auxEncoderErrors, encoderErrorList);
	}

	vout << "- calibration error: 	0x" << std::hex << (unsigned short)drive.getReadReg().RO.calibrationErrors << std::dec;
	printErrorDetails(drive.getReadReg().RO.calibrationErrors, calibrationErrorList);
	vout << "- bridge error: 	0x" << std::hex << (unsigned short)drive.getReadReg().RO.bridgeErrors << std::dec;
	printErrorDetails(drive.getReadReg().RO.bridgeErrors, bridgeErrorList);
	vout << "- hardware error: 	0x" << std::hex << (unsigned short)drive.getReadReg().RO.hardwareErrors << std::dec;
	printErrorDetails(drive.getReadReg().RO.hardwareErrors, hardwareErrorList);
	vout << "- communication error: 	0x" << std::hex << (unsigned short)drive.getReadReg().RO.communicationErrors << std::dec;
	printErrorDetails(drive.getReadReg().RO.communicationErrors, communicationErrorList);
}

void printErrorDetails(uint16_t error, const std::vector<std::string>& errorList)
{
	vout << "	(";
	if (error == 0)
	{
		vout << GREEN("ALL OK") << ")" << std::endl;
		return;
	}

	for (uint32_t i = 0; i < errorList.size(); i++)
	{
		if (error & (1 << i))
			vout << RED_(errorList[i]) << ", ";
	}
	vout << ")";
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
