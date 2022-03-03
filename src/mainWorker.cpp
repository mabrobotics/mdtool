#include "mainWorker.hpp"

#include "ui.hpp"

enum class toolsCmd_E
{
    NONE,
    PING,
    CONFIG,
    SETUP
};
enum class toolsOptions_E
{
    NONE,
    CURRENT,
    CAN,
    SAVE
};
toolsOptions_E str2option(std::string&opt)
{
    if(opt == "ping")
        return toolsOptions_E::CURRENT;
    if(opt == "can")
        return toolsOptions_E::CAN;
    if(opt == "save")
        return toolsOptions_E::SAVE;
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
    toolsCmd_E cmd = str2cmd(args[1]);
    if (cmd == toolsCmd_E::NONE)
    {
        ui::printUnknownCmd(args[1]);
        return;
    }

    for(auto& arg : args)
    {
        if(arg == "-v")
            printVerbose = true;
    }

    candle = new mab::Candle(mab::CANdleBaudrate_E::CAN_BAUD_1M, printVerbose);
    switch (cmd)
    {
    case toolsCmd_E::PING:
        ping();
        break;
    case toolsCmd_E::CONFIG:
    {
        toolsOptions_E option = str2option(args[2]);
        if (option == toolsOptions_E::CAN)
            configCan(args);
        break;
    }
    case toolsCmd_E::SETUP:
        break;
    default:
        return;
    }
}

void MainWorker::ping()
{
    candle->setVebose(true);
    candle->ping();
    candle->setVebose(printVerbose);
}

void MainWorker::configCan(std::vector<std::string>&args)
{
    candle->setVebose(true);
    int id = atoi(args[3].c_str());
    int new_id = atoi(args[4].c_str());
    mab::CANdleBaudrate_E baud = str2baud(args[5]);
    int timeout = atoi(args[6].c_str());
    candle->configMd80Can(id, new_id, baud, timeout);
    candle->setVebose(printVerbose);
}
void MainWorker::setup()
{

}