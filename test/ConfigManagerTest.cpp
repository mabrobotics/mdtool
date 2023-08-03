#include "ConfigManager.hpp"

#include <gtest/gtest.h>

class ConfiManagetTest : public ::testing::Test
{
    protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(ConfiManagetTest, FileCompareTest)
{
    ASSERT_EQ(ConfigManager::compareFiles("test/ConfigManagerTest.cpp", "test/ConfigManagerTest.cpp"), true);
    ASSERT_EQ(ConfigManager::compareFiles("test/ConfigManagerTest.cpp", "test/test.cpp"), false);
}