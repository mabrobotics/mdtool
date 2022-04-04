#include "mainWorker.hpp"

#include "ui.hpp"

#include <unistd.h>

enum class toolsCmd_E
{
    NONE,
    PING,
    CONFIG,
    SETUP,
    TEST
};
enum class toolsOptions_E
{
    NONE,
    CURRENT,
    CAN,
    SAVE,
    ZERO,
    CALIBRATION,
    DIAGNOSTIC
};
toolsOptions_E str2option(std::string&opt)
{
    if(opt == "current")
        return toolsOptions_E::CURRENT;
    if(opt == "can")
        return toolsOptions_E::CAN;
    if(opt == "zero")
        return toolsOptions_E::ZERO;
    if(opt == "save")
        return toolsOptions_E::SAVE;
    if(opt == "calibration")
        return toolsOptions_E::CALIBRATION;
    if(opt == "diagnostic")
        return toolsOptions_E::DIAGNOSTIC;
    return toolsOptions_E::NONE;
}
toolsCmd_E str2cmd(std::string&cmd)
{
    if(cmd == "ping")
        return toolsCmd_E::PING;
    if(cmd == "config")
        return toolsCmd_E::CONFIG;
    if(cmd == "setup")
        return toolsCmd_E::SETUP;
    if(cmd == "test")
        return toolsCmd_E::TEST;
    return toolsCmd_E::NONE;
}

mab::CANdleBaudrate_E str2baud(std::string&baud)
{
    if(baud == "1M")
        return mab::CANdleBaudrate_E::CAN_BAUD_1M;
    if(baud == "2M")
        return mab::CANdleBaudrate_E::CAN_BAUD_2M;
    if(baud == "5M")
        return mab::CANdleBaudrate_E::CAN_BAUD_5M;
    if(baud == "8M")
        return mab::CANdleBaudrate_E::CAN_BAUD_8M;
    return mab::CANdleBaudrate_E::CAN_BAUD_1M;
}

MainWorker::MainWorker(std::vector<std::string>&args)
{
    toolsCmd_E cmd = toolsCmd_E::NONE;
    if(args.size() > 1)
        cmd = str2cmd(args[1]);
    if (cmd == toolsCmd_E::NONE)
    {
        ui::printUnknownCmd(args[1]);
        return;
    }

    printVerbose = true;

    for(auto& arg : args)
    {
        if(arg == "-sv")
            printVerbose = false;
    }

    candle = new mab::Candle(mab::CANdleBaudrate_E::CAN_BAUD_1M, printVerbose);

    toolsOptions_E option = toolsOptions_E::NONE;
    if(args.size() > 2) 
        option = str2option(args[2]);
    switch (cmd)
    {
    case toolsCmd_E::PING:
        ping();
        break;
    case toolsCmd_E::CONFIG:
    {
        if (option == toolsOptions_E::CAN)
            configCan(args);
        if (option == toolsOptions_E::SAVE)
            configSave(args);
        if (option == toolsOptions_E::ZERO)
            configZero(args);
        if (option == toolsOptions_E::CURRENT)
            configCurrent(args);
        if (option == toolsOptions_E::NONE)
            ui::printHelpConfig();
        break;
    }
    case toolsCmd_E::SETUP:
    {
        if (option == toolsOptions_E::NONE)
            ui::printHelpSetup();
        if (option == toolsOptions_E::CALIBRATION)
            setupCalibration(args);
        if (option == toolsOptions_E::DIAGNOSTIC)
            setupDiagnostic(args);
        break;
    }
    case toolsCmd_E::TEST:
    {
        testMove(args);
        break;
    }
    default:
        return;
    }
    delete candle;
}

void MainWorker::ping()
{
    candle->setVebose(true);
    candle->ping();
    candle->setVebose(printVerbose);
}

void MainWorker::configCan(std::vector<std::string>&args)
{
    int id = atoi(args[3].c_str());
    int new_id = atoi(args[4].c_str());
    mab::CANdleBaudrate_E baud = str2baud(args[5]);
    int timeout = atoi(args[6].c_str());
    candle->configMd80Can(id, new_id, baud, timeout);
}
void MainWorker::configSave(std::vector<std::string>&args)
{
    int id = atoi(args[3].c_str());
    candle->configMd80Save(id);
}
void MainWorker::configZero(std::vector<std::string>&args)
{
    int id = atoi(args[3].c_str());
    candle->controlMd80SetEncoderZero(id);
}
void MainWorker::configCurrent(std::vector<std::string>&args)
{
    int id = atoi(args[3].c_str());
    float currentLimit = atof(args[4].c_str());
    candle->configMd80SetCurrentLimit(id, currentLimit);
}
void MainWorker::setupCalibration(std::vector<std::string>&args)
{
    if (!ui::getCalibrationConfirmation())
        return;
    int id = atoi(args[3].c_str());
    candle->setupMd80Calibration(id);
}
void MainWorker::setupDiagnostic(std::vector<std::string>&args)
{
    int id = atoi(args[3].c_str());
    candle->setupMd80Diagnostic(id);
}

void MainWorker::testMove(std::vector<std::string>&args)
{
    int id = atoi(args[2].c_str());
    float targetPos = atof(args[3].c_str());
    if(targetPos > 10.0f)
        targetPos = 10.0f;
    if(targetPos < -10.0f)
        targetPos = -10.0f;
    
    if(!candle->addMd80(id))
        return;
    candle->controlMd80SetEncoderZero(id);
    candle->controlMd80Mode(id, mab::Md80Mode_E::IMPEDANCE);
    candle->controlMd80Enable(id, true);    
    candle->begin();
    usleep(100000);

    float dp = (targetPos + candle->md80s[0].getPosition()) / 300;
    float pos = 0.0f;
    for(int i = 0; i < 300; i++)
    {
        pos+=dp;
        candle->md80s[0].setTargetPosition(pos);
        usleep(10000);
        ui::printPosition(id, candle->md80s[0].getPosition());
    }

    candle->end();
    candle->controlMd80Enable(id, false);
}