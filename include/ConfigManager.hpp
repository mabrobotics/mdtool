#pragma once

#include <set>
#include <string>

class ConfigManager
{
   public:
	ConfigManager(std::string originalConfigPath, std::string userConfigPath);
	~ConfigManager();

	std::set<std::string> getDifferentFilePaths();
	std::set<std::string> getIdenticalFilePaths();
	bool static compareFiles(std::string originalFile, std::string userFile);

   private:
	std::set<std::string> differentFilePaths;
	std::set<std::string> identicalFilePaths;
	std::set<std::string> defaultFilenames;

	void update(std::string originalConfigPath, std::string userConfigPath);
};