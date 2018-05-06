
#include <convertation.h>
#include <crc.h>
#include <gtest/gtest.h>

TEST(ITOA, zero) {

	EXPECT_STREQ("0", itoa(0, 2));
	EXPECT_STREQ("0", itoa(0, 8));
	EXPECT_STREQ("0", itoa(0, 10));
	EXPECT_STREQ("0", itoa(0, 16));
}

TEST(ITOA, one) {

	EXPECT_STREQ("1", itoa(1, 16));
	EXPECT_STREQ("1", itoa(1, 10));
	EXPECT_STREQ("1", itoa(1, 8));
	EXPECT_STREQ("1", itoa(1, 2));
}

TEST(ITOA, maxRange) {

	EXPECT_STREQ("7777777777", itoa(0xffffffff, 8));		// out of range, so it's shrinked
	EXPECT_STREQ("4294967295", itoa(0xffffffff, 10));
	EXPECT_STREQ("FFFFFFFF", itoa(0xffffffff, 16));
}

TEST(ITOA, random) {

	EXPECT_STREQ("4821881", itoa(4821881, 10));
	EXPECT_STREQ("499379", itoa(4821881, 16));	
	EXPECT_STREQ("22311571", itoa(4821881, 8));
}

TEST(TempSensor_API, crcCalc) {

	// 2 random bytes
	EXPECT_EQ(0xBC, calcCRC_1wire(0, 0x02));
	EXPECT_EQ(0xAF, calcCRC_1wire(0xBC, 0x1C));

	// 1 sample array
	unsigned char input1[] = { 0xA2, 0x00, 0x00, 0x00, 0x01, 0xB8, 0x1C, 0x02 };
	unsigned char crc = 0;
	for (int i = 7; i > 0; i--) {

		crc = calcCRC_1wire(crc, input1[i]);
	}
	EXPECT_EQ(input1[0], crc);

	// 2 sample array
	unsigned char input2[] = { 0x12, 0xAA, 0x12, 0x5F, 0x14, 0xA2, 0x12, 0x12 };
	crc = 0;
	for (int i = 7; i >= 0; i--) {

		crc = calcCRC_1wire(crc, input2[i]);
	}
	EXPECT_EQ(0x6D, crc);
}

TEST(TempSensor_API, calcCRCTable) {

	// 2 random bytes
	EXPECT_EQ(0xBC, calcCRC_1wireTable(0, 0x02));
	EXPECT_EQ(0xAF, calcCRC_1wireTable(0xBC, 0x1C));

	// 1 sample array
	unsigned char input1[] = { 0xA2, 0x00, 0x00, 0x00, 0x01, 0xB8, 0x1C, 0x02 };
	unsigned char crc = 0;
	for (int i = 7; i > 0; i--) {

		crc = calcCRC_1wireTable(crc, input1[i]);
	}
	EXPECT_EQ(input1[0], crc);

	// 2 sample array
	unsigned char input2[] = { 0x12, 0xAA, 0x12, 0x5F, 0x14, 0xA2, 0x12, 0x12 };
	crc = 0;
	for (int i = 7; i >= 0; i--) {

		crc = calcCRC_1wireTable(crc, input2[i]);
	}
	EXPECT_EQ(0x6D, crc);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
