#pragma once

#include "candle.hpp"

class MainWorker
{
public:
    MainWorker(std::vector<std::string>&args);

private:
    mab::Candle*candle;

    bool printVerbose = false;

    void ping();
    void configCan(std::vector<std::string>&args);
    void configSave(std::vector<std::string>&args);
    void configZero(std::vector<std::string>&args);
    void configCurrent(std::vector<std::string>&args);
    void setupCalibration(std::vector<std::string>&args);
    void setupDiagnostic(std::vector<std::string>&args);

    void testMove(std::vector<std::string>&args);
    void blink(std::vector<std::string>&args);
};
