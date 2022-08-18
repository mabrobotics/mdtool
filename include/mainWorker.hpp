#pragma once

#include "bus.hpp"
#include "candle.hpp"

class MainWorker
{
   public:
	MainWorker(std::vector<std::string>& args);

   private:
	const std::string path = "/.config/MDtool.ini";
	std::string pathFull = "";

	mab::Candle* candle;

	mab::CANdleBaudrate_E CurrentBaudrate;
	bool printVerbose = false;

	void ping(std::vector<std::string>& args);
	void configCan(std::vector<std::string>& args);
	void configSave(std::vector<std::string>& args);
	void configZero(std::vector<std::string>& args);
	void configCurrent(std::vector<std::string>& args);
	void setupCalibration(std::vector<std::string>& args);
	void setupDiagnostic(std::vector<std::string>& args);

	void testMove(std::vector<std::string>& args);
	void blink(std::vector<std::string>& args);
	void encoder(std::vector<std::string>& args);
	void bus(std::vector<std::string>& args);

	void changeDefaultConfig(std::string bus);
	mab::CANdleBaudrate_E checkSpeedForId(uint16_t id);
};
