#include "ConfigManager.hpp"

#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "dirent.h"

ConfigManager::ConfigManager(std::string userPath)
{
	update();
	if (isConfigDefault(defaultConfigFileName) && isConifgDifferent(defaultConfigFileName))
		copyDefaultConfig(defaultConfigFileName);
	computeFullPathAndName(userPath);
}

std::string ConfigManager::getConfigPath() { return userConfigPath; }
std::string ConfigManager::getConfigName() { return userConfigName; }

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

void ConfigManager::copyDefaultConfig(std::string configName)
{
	int result = system(
		("cp " + originalConfigDir + "/" + configName + " " + userConfigDir + "/" + configName)
			.c_str());
	if (result)
		std::cerr << "Error: Failed to copy the user's file: "
				  << (originalConfigDir + "/" + configName).c_str() << std::endl
				  << "Please check the file and try again." << std::endl;
}

bool ConfigManager::isConfigValid()
{
	// Read default config file.
	mINI::INIFile	   defaultFile(userConfigDir + "/" + defaultConfigFileName);
	mINI::INIStructure defaultIni;
	defaultFile.read(defaultIni);

	// Read user config file.
	mINI::INIFile	   userFile(userConfigPath);
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
			if (!userIni[section].has(key))
				return false;
		}
	}
	return true;
}

std::string ConfigManager::validateConfig()
{
	// Read default config file.
	mINI::INIFile	   defaultFile(userConfigDir + "/" + defaultConfigFileName);
	mINI::INIStructure defaultIni;
	defaultFile.read(defaultIni);

	// Read user config file.
	mINI::INIFile	   userFile(userConfigPath);
	mINI::INIStructure userIni;
	userFile.read(userIni);

	std::string updatedUserConfigPath =
		userConfigPath.substr(0, userConfigPath.find_last_of(".")) + "_updated.cfg";
	/* copy motors configs directory - not the best practice to use
	 * system() but std::filesystem is not available until C++17 */
	int result = system(("cp " + userConfigPath + " " + updatedUserConfigPath).c_str());
	if (result)
		std::cerr << "Error: Failed to copy the user's file: " << userConfigPath << std::endl
				  << "Please check the file and try again." << std::endl;

	// Read updated config file.
	mINI::INIFile	   updatedFile(updatedUserConfigPath);
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
	return updatedUserConfigPath;
}

bool ConfigManager::isFileValid()
{
	std::string fileExtension = userConfigPath.substr(userConfigPath.find_last_of("."));

	if (!(fileExtension == ".cfg"))
	{
		std::cerr
			<< "Error: Configuration file has invalid extension. Create .cfg file and try again."
			<< std::endl;
		return false;
	}

	std::ifstream file(userConfigPath, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		std::cerr << "Error: Unable to open file: " << userConfigPath << std::endl;
		return false;
	}

	std::streampos fileSize = file.tellg();	 // Get the file size
	file.close();							 // Close the file

	std::size_t size = static_cast<std::size_t>(fileSize);

	const std::size_t oneMB = 1048576;	// 1 MB in bytes

	if (size > oneMB)
	{
		std::cerr << "Error: File is larger than 1 MB, check the configuration file." << std::endl;
		return false;
	}

	return true;
}

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

void ConfigManager::computeFullPathAndName(std::string userPath)
{
	if ((userPath.rfind("./", 0) == 0) || (userPath.rfind("/", 0) == 0))
	{
		if ((userPath.rfind("./", 0) == 0))
		{
			char buffer[PATH_MAX];
#ifdef _WIN32
			if (GetCurrentDirectory(PATH_MAX, buffer))
				return (buffer + userFilePath.substr(1));
			else
				throw std::runtime_error("GetCurrentDirectory() error: " +
										 std::to_string(GetLastError()));
#else
			if (getcwd(buffer, sizeof(buffer)) != NULL)
				userConfigPath = (buffer + userPath.substr(1));
			else
				perror("getcwd() error");
#endif
		}
		else
			userConfigPath = userPath;
	}
	else
		userConfigPath = (userConfigDir + "/" + userPath);
	userConfigName = userConfigPath.substr(userConfigPath.find_last_of("/") + 1);
}
