#include "mainWorker.hpp"
#include "ui.hpp"

const std::string version = "v1.1";

int main(int argc, char** argv)
{
    std::cout << "[MDTOOL] MDtool version: " << version << std::endl;
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++)
        args.emplace_back(argv[i]);

    if (args.size() < 2)
    {
        ui::printTooFewArgs();
        return 2;
    }

    MainWorker program(args);

    return EXIT_SUCCESS;
}