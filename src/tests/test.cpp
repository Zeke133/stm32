
#include "../utils/inc/convertation.h"
#include <gtest/gtest.h>

TEST(ITOA, decimal)
{
	const char *str2 = static_cast<const char *>(itoa(12345678, 10));

	ASSERT_STREQ("12345678", str2);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
