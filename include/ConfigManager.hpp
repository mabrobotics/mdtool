#pragma once

#include <set>
#include <string>

#include "mini/ini.h"

/// @brief Class that manages the original and user motor config files directories
class ConfigManager
{
  public:
	/// @brief Construct a new Config Manager object which right away scans and creates list of
	/// compared files in two directiories
	/// @param originalConfigDir Path to the original (containing defaults) config directory
	/// @param userConfigDir Path to the user config directory
	ConfigManager(std::string originalConfigDir, std::string userConfigDir);
	~ConfigManager() = default;

	std::string getConfigPath();
	std::string getConfigName();

	/// @brief Performs the update on directories of compared files
	/// @return Was the update successful
	bool performUpdate();
	/// @brief Get file names of files that are different
	/// @return Set of file names
	std::set<std::string> getDifferentFilePaths();
	/// @brief Get file names of files that are identical
	/// @return Set of file names
	std::set<std::string> getIdenticalFilePaths();

	bool static compareFiles(std::string originalFilePath, std::string userFilePath);

	/// @brief Check if file exists
	/// @param filePath - path to the file
	/// @return True on file present, false otherwise
	bool static doesFileExists(std::string filePath);
	/// @brief Check if filename is present in original config directory set
	/// @param configName - filename
	/// @return True on present, false otherwise
	bool isConfigDefault(std::string configName);
	bool isConfigDefault();
	/// @brief Check if filename is in set that contains files that differ from the original
	/// defaults
	/// @param configName - filename
	bool isConifgDifferent(std::string configName);
	bool isConifgDifferent();

	bool isConfigValid(std::string configName);

	std::string validateConfig(std::string configName);

  private:
	std::set<std::string> differentFilePaths, identicalFilePaths, defaultFilenames;

	std::string userConfigName, userConfigPath;
	std::string originalConfigDir, userConfigDir;
	// const std::string userConfigDir =
	// 	std::string(getenv("HOME")) + "/.config/mdtool/mdtool_motors";
	// const std::string originalConfigDir	= "/etc/mdtool/mdtool_motors";
	const std::string defaultConfigFileName = "default.ini";

	void update();
};