
#include <convertation.h>
#include <crc.h>
#include <gtest/gtest.h>

#include <vector>

namespace {

using std::vector;

struct ItoaValues {
	int base;
	int val;
	const char * text;
};

class ItoaTest : public ::testing::TestWithParam<struct ItoaValues> {};

TEST_P(ItoaTest, CheckConvertation) {

	auto val = GetParam();
	EXPECT_STREQ(val.text, itoa(val.val, val.base));
}

vector<struct ItoaValues> itoaValues {

	{2, 0, "0"},
	{2, 1, "1"},
	{2, 3, "11"},

	{8, 0, "0"},
	{8, 1, "1"},
	{8, (int)0xffffffff, "7777777777"},		// out of range, so it's shrinked
	{8, 4821881, "22311571"},

	{10, 0, "0"},
	{10, 1, "1"},
	{10, (int)4294967295, "4294967295"},
	{10, 4821881, "4821881"},

	{16, 0, "0"},
	{16, 1, "1"},
	{16, (int)0xffffffff, "FFFFFFFF"},
	{16, 4821881, "499379"}
};

INSTANTIATE_TEST_CASE_P(ItoaTestName, ItoaTest,
                        ::testing::ValuesIn(itoaValues));

// ---

struct ItoaValuesWithWidth {
	int base;
	int val;
	int width;
	const char * text;
};

class ItoaTestWidth : public ::testing::TestWithParam<struct ItoaValuesWithWidth> {};

TEST_P(ItoaTestWidth, CheckWidth) {

	auto val = GetParam();
	EXPECT_STREQ(val.text, itoa(val.val, val.base, val.width));
}

vector<struct ItoaValuesWithWidth> itoaValuesWithWidth {

	{10, (int)4294967295, 7, "4967295"},
	{10, 4821881, 9, "004821881"}
};

INSTANTIATE_TEST_CASE_P(ItoaTestOfWidthParam, ItoaTestWidth,
                        ::testing::ValuesIn(itoaValuesWithWidth));

// ---

struct AtoiValues {
	int result;
	const char * text;
	int base;
	int signs;
};

class AtoiTest : public ::testing::TestWithParam<struct AtoiValues> {};

TEST_P(AtoiTest, CheckConvertation) {

	auto val = GetParam();
	EXPECT_EQ(val.result, atoi((const uint8_t *)(val.text), val.base, val.signs));
}

vector<struct AtoiValues> atoiValues {

	{155, "155", 10, 10},
	{0xFF, "FF", 16, 10},
	{-62, "-62", 10, 10},
	{0, "0", 10, 10},
	{123, "123456", 10, 3},
	{0xA24B81, "A24B81", 16, 6}

};

INSTANTIATE_TEST_CASE_P(AtoiTestName, AtoiTest,
                        ::testing::ValuesIn(atoiValues));

// ---

struct TemperatureValues {
	int32_t decimalValue;
	uint16_t dallasValue;
};

class TemperatureConvertation : public ::testing::TestWithParam<struct TemperatureValues> {};

TEST_P(TemperatureConvertation, CheckDS18B20Format) {

	auto val = GetParam();
	EXPECT_EQ(val.decimalValue, ds18b20Temp2decimal(val.dallasValue));
}

vector<struct TemperatureValues> dallasValues {

	{1250000,	0x07D0},
	{850000,	0x0550},
	{250625,	0x0191},
	{101250,	0x00A2},
	{ 5000,		0x0008},
	{0,			0x0000},
	{- 5000,	0xFFF8},
	{-101250,	0xFF5E},
	{-250625,	0xFE6F},
	{-550000,	0xFC90}
};

INSTANTIATE_TEST_CASE_P(DallasTest, TemperatureConvertation,
                        ::testing::ValuesIn(dallasValues));

// ---
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

} // namespace UnitTest

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
