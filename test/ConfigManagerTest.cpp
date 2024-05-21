#include "ConfigManager.hpp"

#include <gtest/gtest.h>
#include <fstream>

class ConfigManagerTest : public ::testing::Test
{
  protected:
	virtual void SetUp()
	{
		// Create two test files
		std::ofstream file1(tempfile1Path);
		file1 << "This is a test file";
		file1.close();
		std::ofstream file2(tempfile2Path);
		file2 << "This is a test file 2";
		file2.close();
		std::ofstream file3(tempfile3Path);
		file3 << "This is a test file 3";
		file3.close();

		system(("mkdir " + tempOriginalDirectory).c_str());
		system(("mkdir " + tempUserDirectory).c_str());

		// Copy the test file to the original directory
		system(("cp " + tempfile1Path + " " + tempOriginalDirectory).c_str());
		system(("cp " + tempfile2Path + " " + tempOriginalDirectory).c_str());

		// Copy the test file to the user directory
		system(("cp " + tempfile1Path + " " + tempUserDirectory).c_str());
		system(("cp " + tempfile2Path + " " + tempUserDirectory).c_str());
		system(("cp " + tempfile3Path + " " + tempUserDirectory).c_str());
	}

	virtual void TearDown()
	{
		// //Delete the test files
		remove(tempfile1Path.c_str());
		remove(tempfile2Path.c_str());
		remove(tempfile3Path.c_str());

		// //Delete the test directories
		system(("rm -rf " + tempOriginalDirectory).c_str());
		system(("rm -rf " + tempUserDirectory).c_str());
	}

	std::string tempOriginalDirectory = "./original/";
	std::string tempUserDirectory = "./user/";

	std::string tempfile1Path = "asepoawinoeaine.txt";
	std::string tempfile2Path = "aweradswaeawesdae.txt";
	std::string tempfile3Path = "awerasdawsdae.txt";
};

TEST_F(ConfigManagerTest, FileCompareTest)
{
	ASSERT_EQ(ConfigManager::compareFiles(tempfile1Path, tempfile1Path), true);
	ASSERT_EQ(ConfigManager::compareFiles(tempfile1Path, tempfile2Path), false);
}

TEST_F(ConfigManagerTest, ReadFileTest)
{
	ASSERT_EQ(ConfigManager::doesFileExists(tempfile2Path), true);
	ASSERT_EQ(ConfigManager::doesFileExists("./asfaewawedddddddea.cpp"), false);
}

TEST_F(ConfigManagerTest, CompareOnlyOriginalFilesInDirectory)
{
	ConfigManager configManager(tempOriginalDirectory, tempUserDirectory);
	ASSERT_EQ(configManager.getIdenticalFilePaths().size(), 2);
	ASSERT_EQ(configManager.getDifferentFilePaths().size(), 0);

	// change file to check if it is detected
	std::ofstream file1(tempUserDirectory + tempfile1Path);
	file1 << "This is again a test file but different";
	file1.close();

	configManager.performUpdate();
	ASSERT_EQ(configManager.getIdenticalFilePaths().size(), 1);
	ASSERT_EQ(configManager.getDifferentFilePaths().size(), 1);
	ASSERT_EQ(configManager.getDifferentFilePaths().find(tempfile1Path) !=
				  configManager.getDifferentFilePaths().end(),
			  true);

	// revert file to original
	file1.open(tempUserDirectory + tempfile1Path);
	file1 << "This is a test file";
	file1.close();
}

TEST_F(ConfigManagerTest, IsConfigValidAndDifferent)
{
	// change file
	std::ofstream file1(tempUserDirectory + tempfile1Path);
	file1 << "This is again a test file but different";
	file1.close();

	ConfigManager configManager(tempOriginalDirectory, tempUserDirectory);
	ASSERT_EQ(configManager.isConfigDefault(tempfile1Path), true);
	ASSERT_EQ(configManager.isConfigDefault("aweasdassddddddddddddddd"), false);
	ASSERT_EQ(configManager.isConifgDifferent(tempfile1Path), true);
	ASSERT_EQ(configManager.isConifgDifferent(tempfile2Path), false);
}