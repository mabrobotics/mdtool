#include "mainWorker.hpp"
#include "ui.hpp"

int main(int argc, char**argv)
{
    std::vector<std::string> args;
    for(int i = 0 ; i < argc; i++)
        args.emplace_back(argv[i]);
        
    if(args.size() < 2)
        ui::printTooFewArgs();
    
    MainWorker program(args);
    
    return EXIT_SUCCESS;
}