#include "ConfigManager.hpp"
#include "mini/ini.h"
#include "dirent.h"

#include <iostream>

ConfigManager::ConfigManager(std::string originalConfigPath, std::string userConfigPath)
{
    update(originalConfigPath, userConfigPath);
}

ConfigManager::~ConfigManager()
{
}

void ConfigManager::update(std::string originalConfigPath, std::string userConfigPath)
{
    // Clear the sets
    differentFilePaths.clear();
    identicalFilePaths.clear();
    defaultFilenames.clear();


    // Get the filenames of the original config
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(originalConfigPath.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            std::string filename = ent->d_name;
            if (filename != "." && filename != "..")
            {
                defaultFilenames.insert(filename);
            }
        }
        closedir(dir);
    }
    else
    {
        exit(18886);
    }

    // Compare the files
    for (auto &filename : defaultFilenames)
    {
        std::string originalFile = originalConfigPath + filename;
        std::string userFile = userConfigPath + filename;


        if (compareFiles(originalFile, userFile))
        {
            identicalFilePaths.insert(filename);
        }
        else
        {
            differentFilePaths.insert(filename);
        }
    }        
}

bool ConfigManager::compareFiles(std::string originalFilePath, std::string userFilePath)
{
    std::fstream originalFileStream(originalFilePath);
    std::fstream userFileStream(userFilePath);

    std::string originalFile((std::istreambuf_iterator<char>(originalFileStream)), std::istreambuf_iterator<char>());
    std::string userFile((std::istreambuf_iterator<char>(userFileStream)), std::istreambuf_iterator<char>());

    std::hash<std::string> hasher;

    return hasher(originalFile) == hasher(userFile);
}

std::set<std::string> ConfigManager::getDifferentFilePaths()
{
    return differentFilePaths;
}
std::set<std::string> ConfigManager::getIdenticalFilePaths()
{
    return identicalFilePaths;
}