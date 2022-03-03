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
    void setup();
};
