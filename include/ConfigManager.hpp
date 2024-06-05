#pragma once

#include <set>
#include <string>

#include "mini/ini.h"

/**
 * @brief Class that manages the original and user motor config files directories
 */
class ConfigManager
{
  public:
	/**
	 * @brief Construct a new Config Manager object which computes the full path to user config.
	 * Scans and creates list of files in two default directiories - user directories and mdtool
	 * install direcotry.
	 *
	 * @param userPath Path to the user config passed as an argument to mdtool function
	 */
	ConfigManager(std::string userPath);
	~ConfigManager() = default;

	std::string getConfigPath();
	std::string getConfigName();
	/**
	 * @brief Performs the update on directories of compared files.
	 *
	 * @return Was the update successful
	 */
	bool performUpdate();
	/**
	 * @brief Get file names of files that are different.
	 *
	 * @return Set of file names
	 */
	std::set<std::string> getDifferentFilePaths();
	/**
	 * @brief Get file names of files that are identical
	 *
	 * @return Set of file names
	 */
	std::set<std::string> getIdenticalFilePaths();

	bool static compareFiles(std::string originalFilePath, std::string userFilePath);
	/**
	 * @brief Checks if file exists.
	 *
	 * @param filePath Path to the file.
	 * @return @return true on file present, false otherwise.
	 */
	bool static doesFileExists(std::string filePath);
	/**
	 * @brief Checks if filename is present in original config directory set.
	 *
	 * @param configName Filename
	 * @return true on present, false otherwise
	 */
	bool isConfigDefault(std::string configName);
	bool isConfigDefault();
	/**
	 * @brief Checks if filename is in set that contains files that differ from the original.
	 *
	 * @param configName Filename
	 * @return true if config was edited; false if config is default.
	 */
	bool isConifgDifferent(std::string configName);
	bool isConifgDifferent();
	/**
	 * @brief Copies a default config file from original mdtool config directory to the user's
	 * config directory.
	 *
	 * @param configName Filename.
	 */
	void copyDefaultConfig(std::string configName);
	/**
	 * @brief Compares the user's configuration file with the default configuration file to ensure
	 * that all necessary fields are present.
	 *
	 * @return true if the configuration file contains all necessary fields; false if the
	 * configuration file is missing required fields;
	 */
	bool isConfigValid();
	/**
	 * @brief Creates a copy of the user's configuration file and updates all missing fields with
	 * default values from the default configuration file. The resulting configuration file is saved
	 * as a new file.
	 *
	 * @return The path to the updated copy of the user's configuration file.
	 */
	std::string validateConfig();
	/**
	 * @brief Checks if a file has the proper .cfg extension and if it is smaller than 1MB.
	 *
	 * @return true if the configuration file is valid; false otherwise.
	 */
	bool isFileValid();

  private:
	std::set<std::string> differentFilePaths, identicalFilePaths, defaultFilenames;

	std::string		  userConfigName, userConfigPath;
	const std::string userConfigDir = std::string(getenv("HOME")) + "/.config/mdtool/mdtool_motors";
	const std::string originalConfigDir		= "/etc/mdtool/mdtool_motors";
	const std::string defaultConfigFileName = "default.cfg";

	void update();
	void computeFullPathAndName(std::string userPath);
};