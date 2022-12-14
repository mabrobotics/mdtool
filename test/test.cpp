#include <gtest/gtest.h>

#include "ui.hpp"

TEST(mdtoolTest, Test_Dummy)
{
	ui::printHelp();
	ASSERT_EQ(1, true);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}