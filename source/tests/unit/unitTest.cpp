
#include <convertation.h>
#include <crc.h>


#include <gtest/gtest.h>

#include <vector>

namespace {

using std::vector;



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
