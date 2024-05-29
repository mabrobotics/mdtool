#include "ConfigManager.hpp"

#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "dirent.h"

ConfigManager::ConfigManager(std::string originalConfigDir, std::string userConfigDir)
	: originalConfigDir(originalConfigDir), userConfigDir(userConfigDir)
{
	update();

	if (isConfigDefault(defaultConfigFileName) && isConifgDifferent(defaultConfigFileName))
	{
		system(("cp " + originalConfigDir + "/" + defaultConfigFileName + " " + userConfigDir +
				"/" + defaultConfigFileName)
				   .c_str());
	}
}

std::string ConfigManager::getConfigPath() { return userConfigPath; }
std::string ConfigManager::getConfigName() { return userConfigName; }

void ConfigManager::update()
{
	// Clear the sets
	differentFilePaths.clear();
	identicalFilePaths.clear();
	defaultFilenames.clear();

	// Get the filenames of the original config
	DIR*		   dir;
	struct dirent* ent;
	if ((dir = opendir(originalConfigDir.c_str())) != NULL)
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
		std::string originalFile = originalConfigDir + "/" + filename;
		std::string userFile	 = userConfigDir + "/" + filename;

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

bool ConfigManager::isConfigDefault()
{
	return defaultFilenames.find(userConfigName) != defaultFilenames.end();
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

bool ConfigManager::isConifgDifferent()
{
	if (!isConfigDefault(userConfigName))
	{
		return false;
	}
	else
	{
		return differentFilePaths.find(userConfigName) != differentFilePaths.end();
	}
}

bool ConfigManager::isConfigValid(std::string configName)
{
	// Read default config file.
	mINI::INIFile	   defaultFile(userConfigDir + "/" + defaultConfigFileName);
	mINI::INIStructure defaultIni;
	defaultFile.read(defaultIni);

	// Read user config file.
	mINI::INIFile	   userFile(userConfigDir + "/" + configName);
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
			auto const& key	  = it2.first;
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
	mINI::INIFile	   defaultFile(userConfigDir + "/" + defaultConfigFileName);
	mINI::INIStructure defaultIni;
	defaultFile.read(defaultIni);

	// Read user config file.
	mINI::INIFile	   userFile(userConfigDir + "/" + configName);
	mINI::INIStructure userIni;
	userFile.read(userIni);

	std::string updatedConfigName =
		configName.substr(0, configName.find_last_of(".")) + "_updated.cfg";
	/* copy motors configs directory - not the best practice to use
	 * system() but std::filesystem is not available until C++17 */
	int result = 0;
	result	   = system(
		("cp " + userConfigDir + "/" + configName + " " + userConfigDir + "/" + updatedConfigName)
			.c_str());

	// Read updated config file.
	mINI::INIFile	   updatedFile(userConfigDir + "/" + updatedConfigName);
	mINI::INIStructure updatedIni;
	updatedFile.read(updatedIni);

	// Loop fills all lacking fields in the user's config file.
	for (auto const& it : defaultIni)
	{
		auto const& section	   = it.first;
		auto const& collection = it.second;
		for (auto const& it2 : collection)
		{
			auto const& key	  = it2.first;
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

std::string ConfigManager::getFullPath()
{
	if ((userConfigPath.rfind("./", 0) == 0) || (userConfigPath.rfind("/", 0) == 0))
	{
		if ((userConfigPath.rfind("./", 0) == 0))
		{
			char buffer[PATH_MAX];
#ifdef _WIN32
			if (GetCurrentDirectory(PATH_MAX, buffer))
			{
				return (buffer + userFilePath.substr(1));
			}
			else
			{
				throw std::runtime_error("GetCurrentDirectory() error: " +
										 std::to_string(GetLastError()));
			}
#else
			if (getcwd(buffer, sizeof(buffer)) != NULL)
			{
				return (buffer + userConfigPath.substr(1));
			}
			else
			{
				perror("getcwd() error");
			}
#endif
		}
	}
	else
	{
		return (userConfigPath + "/" + userConfigPath);
	}
}