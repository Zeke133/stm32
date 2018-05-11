
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

TEST(DateTimeConvertation, unixStartToDate) {

	DateTime dt;

	unix2DateTime(dt, 0);

	EXPECT_EQ(1, dt.date);
	EXPECT_EQ(1, dt.month);
	EXPECT_EQ(1970, dt.year);
	
	EXPECT_EQ(3, dt.wday);

	EXPECT_EQ(0, dt.hours);
	EXPECT_EQ(0, dt.minutes);
	EXPECT_EQ(0, dt.seconds);
}

TEST(DateTimeConvertation, unixRandomToDate) {

	DateTime dt;

	unix2DateTime(dt, 1525770736);	// GMT: Tuesday, May 8, 2018 9:12:16 AM

	EXPECT_EQ(8, dt.date);
	EXPECT_EQ(5, dt.month);
	EXPECT_EQ(2018, dt.year);
	
	EXPECT_EQ(1, dt.wday);

	EXPECT_EQ(9, dt.hours);
	EXPECT_EQ(12, dt.minutes);
	EXPECT_EQ(16, dt.seconds);
}

TEST(DateTimeConvertation, dateStartToUnix) {

	struct DateTime dt {

		.hours = 0,
		.minutes = 0,
		.seconds = 0,
		.date = 1,
		.wday = 0,
		.month = 1,
		.year = 1970
	};

	EXPECT_EQ(0, dateTime2Unix(dt));
}

TEST(DateTimeConvertation, dateRandToUnix) {

	struct DateTime dt {	// unix 1525771516 is GMT: Tuesday, May 8, 2018 9:25:16 AM

		.hours = 9,
		.minutes = 25,
		.seconds = 16,
		.date = 8,
		.wday = 1,
		.month = 5,
		.year = 2018
	};

	EXPECT_EQ(1525771516, dateTime2Unix(dt));
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
	EXPECT_EQ(0xBC, calcCRC_1wireQuick(0, 0x02));
	EXPECT_EQ(0xAF, calcCRC_1wireQuick(0xBC, 0x1C));

	// 1 sample array
	unsigned char input1[] = { 0xA2, 0x00, 0x00, 0x00, 0x01, 0xB8, 0x1C, 0x02 };
	unsigned char crc = 0;
	for (int i = 7; i > 0; i--) {

		crc = calcCRC_1wireQuick(crc, input1[i]);
	}
	EXPECT_EQ(input1[0], crc);

	// 2 sample array
	unsigned char input2[] = { 0x12, 0xAA, 0x12, 0x5F, 0x14, 0xA2, 0x12, 0x12 };
	crc = 0;
	for (int i = 7; i >= 0; i--) {

		crc = calcCRC_1wireQuick(crc, input2[i]);
	}
	EXPECT_EQ(0x6D, crc);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}