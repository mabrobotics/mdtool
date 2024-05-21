#include "ConfigManager.hpp"

#include <iostream>

#include "dirent.h"

ConfigManager::ConfigManager(std::string originalConfigPath, std::string userConfigPath)
	: originalConfigPath(originalConfigPath), userConfigPath(userConfigPath)
{
	update();
}

void ConfigManager::update()
{
	// Clear the sets
	differentFilePaths.clear();
	identicalFilePaths.clear();
	defaultFilenames.clear();

	// Get the filenames of the original config
	DIR* dir;
	struct dirent* ent;
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

	for (auto& filename : defaultFilenames)
	{
		std::string originalFile = originalConfigPath + "/" + filename;
		std::string userFile = userConfigPath + "/" + filename;

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
	// check if files exist
	if (!doesFileExists(originalFilePath))
	{
		std::cout << "Original file path doesn't exist" << std::endl;
		return false;
	}

	if (!doesFileExists(userFilePath))
	{
		std::cout << "User file path doesn't exist" << std::endl;
		return false;
	}

	std::fstream originalFileStream(originalFilePath, std::ios::in);
	std::fstream userFileStream(userFilePath, std::ios::in);

	if (!originalFileStream.is_open() || !userFileStream.is_open())
	{
		std::cout << "Error opening file" << std::endl;
		return false;
	}

	std::string originalFile((std::istreambuf_iterator<char>(originalFileStream)),
							 std::istreambuf_iterator<char>());
	std::string userFile((std::istreambuf_iterator<char>(userFileStream)),
						 std::istreambuf_iterator<char>());

	return originalFile == userFile;
}

bool ConfigManager::doesFileExists(std::string filePath)
{
	std::ifstream fileStream(filePath);
	return fileStream.good();
}

std::set<std::string> ConfigManager::getDifferentFilePaths() { return differentFilePaths; }
std::set<std::string> ConfigManager::getIdenticalFilePaths() { return identicalFilePaths; }

bool ConfigManager::performUpdate()
{
	this->update();
	return true;
}

bool ConfigManager::isConfigDefault(std::string configName)
{
	return defaultFilenames.find(configName) != defaultFilenames.end();
}

bool ConfigManager::isConifgDifferent(std::string configName)
{
	if (!isConfigDefault(configName))
	{
		return false;
	}
	else
	{
		return differentFilePaths.find(configName) != differentFilePaths.end();
	}
}

bool ConfigManager::isConfigValid(std::string configName)
{
	// Read default config file.
	mINI::INIFile defaultFile(userConfigPath + "/" + defaultConfigFileName);
	mINI::INIStructure defaultIni;
	defaultFile.read(defaultIni);

	// Read user config file.
	mINI::INIFile userFile(userConfigPath + "/" + configName);
	mINI::INIStructure userIni;
	userFile.read(userIni);

	// Loop checks if all required fields are present in the config file.
	for (auto const& it : defaultIni)
	{
		auto const& section = it.first;
		if (!userIni.has(section))
			return false;
		auto const& collection = it.second;
		for (auto const& it2 : collection)
		{
			auto const& key = it2.first;
			auto const& value = it2.second;
			if (!userIni[section].has(key))
				return false;
		}
	}
	return true;
}

std::string ConfigManager::validateConfig(std::string configName)
{
	// Read default config file.
	mINI::INIFile defaultFile(userConfigPath + "/" + defaultConfigFileName);
	mINI::INIStructure defaultIni;
	defaultFile.read(defaultIni);

	// Read user config file.
	mINI::INIFile userFile(userConfigPath + "/" + configName);
	mINI::INIStructure userIni;
	userFile.read(userIni);

	std::string updatedConfigName =
		configName.substr(0, configName.find_last_of(".")) + "_updated.cfg";
	/* copy motors configs directory - not the best practice to use
	 * system() but std::filesystem is not available until C++17 */
	int result = 0;
	result = system(
		("cp " + userConfigPath + "/" + configName + " " + userConfigPath + "/" + updatedConfigName)
			.c_str());

	// Read updated config file.
	mINI::INIFile updatedFile(userConfigPath + "/" + updatedConfigName);
	mINI::INIStructure updatedIni;
	updatedFile.read(updatedIni);

	// Loop fills all lacking fields in the user's config file.
	for (auto const& it : defaultIni)
	{
		auto const& section = it.first;
		auto const& collection = it.second;
		for (auto const& it2 : collection)
		{
			auto const& key = it2.first;
			auto const& value = it2.second;
			if (!userIni[section].has(key))
				updatedIni[section][key] = value;
			else
				updatedIni[section][key] = userIni.get(section).get(key);
		}
	}
	// Write an updated config file
	updatedFile.write(updatedIni);
	return updatedConfigName;
}