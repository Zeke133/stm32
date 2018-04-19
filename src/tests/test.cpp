
#include "../utils/inc/convertation.h"
#include <gtest/gtest.h>

TEST(ITOA, zero)
{
	EXPECT_STREQ("0", itoa(0, 2));
	EXPECT_STREQ("0", itoa(0, 8));
	EXPECT_STREQ("0", itoa(0, 10));
	EXPECT_STREQ("0", itoa(0, 16));
}

TEST(ITOA, one)
{
	EXPECT_STREQ("1", itoa(1, 16));
	EXPECT_STREQ("1", itoa(1, 10));
	EXPECT_STREQ("1", itoa(1, 8));
	EXPECT_STREQ("1", itoa(1, 2));
}

TEST(ITOA, maxRange)
{
	EXPECT_STREQ("7777777777", itoa(0xffffffff, 8));		// out of range, so it's shrinked
	EXPECT_STREQ("4294967295", itoa(0xffffffff, 10));
	EXPECT_STREQ("FFFFFFFF", itoa(0xffffffff, 16));
}

TEST(ITOA, random)
{
	EXPECT_STREQ("4821881", itoa(4821881, 10));
	EXPECT_STREQ("499379", itoa(4821881, 16));	
	EXPECT_STREQ("22311571", itoa(4821881, 8));
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
