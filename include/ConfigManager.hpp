#pragma once

#include <set>
#include <string>

class ConfigManager
{
   public:
	ConfigManager(std::string originalConfigPath, std::string userConfigPath);
	~ConfigManager();

	bool performUpdate();
	std::set<std::string> getDifferentFilePaths();
	std::set<std::string> getIdenticalFilePaths();
	bool static compareFiles(std::string originalFilePath, std::string userFilePath);
	bool static doesFileExists(std::string filePath);
	bool isConfigDefault(std::string configName);
	bool isConifgDiffrent(std::string configName);

   private:
	std::set<std::string> differentFilePaths, identicalFilePaths, defaultFilenames;

	std::string originalConfigPath, userConfigPath;

	void update();

};