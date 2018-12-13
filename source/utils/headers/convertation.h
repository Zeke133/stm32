#ifndef _CONVETRATION_H
#define _CONVETRATION_H

#include <stdint.h>

// Convertation of Integers to String representation
// val - input value
// base - 10 for decimal or 16 for HEX view
// signs - fixed length of number, 0 - auto
const char * itoa(unsigned int val, int base = 10, uint8_t width = 0);

// Convertation of Strings to Int
int32_t atoi(const uint8_t * mas, uint8_t base = 10, uint8_t signs = 10);

// Convertation of Date Time to UNIX format
struct DateTime {

    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t date;
    uint8_t wday;
    uint8_t month;
    uint16_t year;
};

// (UnixTime = 00:00:00 01.01.1970 = JD0 = 2440588)
constexpr uint32_t JULIAN_DATE_BASE = 2440588;      // appears like static / why not in class?

uint32_t dateTime2Unix(const DateTime& dateTime);
void unix2DateTime(DateTime& dateTimeRef, uint32_t unixTime);

// Dallas DS18B20 temperature sensor format to Decimal in form 125.0000 format
int32_t ds18b20Temp2decimal(uint16_t tempDs18b20);

#endif
