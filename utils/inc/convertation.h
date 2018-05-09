#ifndef _CONVETRATION_H
#define _CONVETRATION_H

#include <stdint.h>


// Convertation of Integers to String representation
// val - input value
// base - 10 for decimal or 16 for HEX view
const char * itoa(unsigned int val, int base);

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
const uint32_t JULIAN_DATE_BASE = 2440588;

uint32_t dateTime2Unix(const DateTime& dateTime);
void unix2DateTime(DateTime& dateTimeRef, uint32_t unixTime);

#endif
