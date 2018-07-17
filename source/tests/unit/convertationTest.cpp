// tested source
#include <convertation.h>

// testing framework
#include <gtest/gtest.h>

// std
#include <vector>

namespace UnitTests {

    using std::vector;

    struct ItoaValues {
        int base;
        int val;
        const char * text;
    };

    class ItoaTester : public ::testing::TestWithParam<struct ItoaValues> {};

    TEST_P(ItoaTester, CheckConvertation) {

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

    INSTANTIATE_TEST_CASE_P(ItoaTest, ItoaTester,
                            ::testing::ValuesIn(itoaValues));

    // ---

    struct ItoaValuesWithWidth {
        int base;
        int val;
        int width;
        const char * text;
    };

    class ItoaWidthTester : public ::testing::TestWithParam<struct ItoaValuesWithWidth> {};

    TEST_P(ItoaWidthTester, CheckWidth) {

        auto val = GetParam();
        EXPECT_STREQ(val.text, itoa(val.val, val.base, val.width));
    }

    vector<struct ItoaValuesWithWidth> itoaValuesWithWidth {

        {10, (int)4294967295, 7, "4967295"},
        {10, 4821881, 9, "004821881"}
    };

    INSTANTIATE_TEST_CASE_P(ItoaTestOfWidthParam, ItoaWidthTester,
                            ::testing::ValuesIn(itoaValuesWithWidth));

    // ---

    struct AtoiValues {
        int result;
        const char * text;
        int base;
        int signs;
    };

    class AtoiTester : public ::testing::TestWithParam<struct AtoiValues> {};

    TEST_P(AtoiTester, CheckConvertation) {

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

    INSTANTIATE_TEST_CASE_P(AtoiTest, AtoiTester,
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

    struct DateTimeValues {
        DateTime dateTime;
        uint32_t unixFormat;
    };

    class DateTimeToUnixTester : public ::testing::TestWithParam<struct DateTimeValues> {};
    class UnixToDateTimeTester : public ::testing::TestWithParam<struct DateTimeValues> {};

    TEST_P(DateTimeToUnixTester, DateTimeToUnixFormat) {

        auto val = GetParam();
        EXPECT_EQ(val.unixFormat, dateTime2Unix(val.dateTime));
    }

    TEST_P(UnixToDateTimeTester, UnixToDateTimeFormat) {

        auto val = GetParam();

        DateTime dt;

        unix2DateTime(dt, val.unixFormat);

        EXPECT_EQ(dt.year, val.dateTime.year);
        EXPECT_EQ(dt.month, val.dateTime.month);
        EXPECT_EQ(dt.date, val.dateTime.date);
        EXPECT_EQ(dt.wday, val.dateTime.wday);
        EXPECT_EQ(dt.hours, val.dateTime.hours);
        EXPECT_EQ(dt.minutes, val.dateTime.minutes);
        EXPECT_EQ(dt.seconds, val.dateTime.seconds);
    }

    vector<struct DateTimeValues> dateTimeValues {

        {
            {
                .hours = 0,
                .minutes = 0,
                .seconds = 0,
                .date = 1,
                .wday = 3,
                .month = 1,
                .year = 1970
            },
            0
        },
        {
            {
                .hours = 9,
                .minutes = 25,
                .seconds = 16,
                .date = 8,
                .wday = 1,
                .month = 5,
                .year = 2018
            },
            1525771516
        },
        {
            {
                .hours = 9,
                .minutes = 12,
                .seconds = 16,
                .date = 8,
                .wday = 1,
                .month = 5,
                .year = 2018
            },
            1525770736	// GMT: Tuesday, May 8, 2018 9:12:16 AM
        }
    };

    INSTANTIATE_TEST_CASE_P(DateTimeConvertation, DateTimeToUnixTester,
                            ::testing::ValuesIn(dateTimeValues));

    INSTANTIATE_TEST_CASE_P(DateTimeConvertation, UnixToDateTimeTester,
                            ::testing::ValuesIn(dateTimeValues));    

} // namespace UnitTests

