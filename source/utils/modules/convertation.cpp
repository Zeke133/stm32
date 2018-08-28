#include <convertation.h>

const char * itoa(unsigned int val, int base, uint8_t width) {

    const char * symbols = "0123456789ABCDEF";
    static char buf[11];
    int i;

    for(i = 0; i < 10; i++) {
        buf[i] = '0';
    }

    buf[i--] = 0;

    while(1) {

        if(i < 0) return buf;

        buf[i] = symbols[val % base];
        val /= base;

        if(val == 0) return width ? &buf[10-width] : &buf[i];
        i--;
    }
    return "ERROR!";
}

int32_t atoi(const uint8_t * mas, uint8_t base, uint8_t signs) {

    int32_t res = 0;
    int8_t sign = 1;
    const uint8_t * symbols = (const uint8_t *)"0123456789ABCDEF";

    if (base > 16) return 0;

    uint32_t i = 0;

    if (mas[0] == '-') {
        sign = -1;
        i = 1;
    }

    for( ; i < signs; i++) {
        // check for end of string
        if (mas[i] == 0) break;
        // search of symbol
        uint8_t s = 0;
        for ( ; s < base; s++) {
            if (mas[i] == symbols[s]) break;
        }
        // no such symbol
        if (s == base) break;
        // add to number
        res *= base;
        res += s;
    }

    return res * sign;
}

void unix2DateTime(DateTime& dateTimeRef, uint32_t unixTime) {
    
    uint32_t time;
    uint32_t t1, a, b, c, d, e, m;

    // Calc of Julian date JD and Julian day number JDN
    uint32_t jd = ((unixTime + 43200) / (86400 >> 1)) + (2440587 << 1) + 1;
    uint32_t jdn = jd>>1;

    // Calc of hours/minutes/seconds
    time = unixTime;
    t1 = time / 60;
    dateTimeRef.seconds = time - t1 * 60;

    time = t1;
    t1 = time / 60;
    dateTimeRef.minutes = time - t1 * 60;

    time = t1;
    t1 = time / 24;
    dateTimeRef.hours = time - t1 * 24;

    // time = unixTime;
    // t1 = time % 86400;
    // dateTimeRef.hours = t1 / 3600;
    // dateTimeRef.minutes = t1 % 3600 / 60;
    // dateTimeRef.seconds = t1 % 3600 % 60;

    // Calc of week day
    dateTimeRef.wday = jdn % 7;

    // Calc of date
    a = jdn + 32044;
    b = (4 * a + 3) / 146097;
    c = a - (146097 * b) / 4;
    d = (4 * c + 3) / 1461;
    e = c - (1461 * d) / 4;
    m = (5 * e + 2) / 153;
    dateTimeRef.date = e - (153 * m + 2) / 5 + 1;
    dateTimeRef.month = m + 3 - 12 * (m / 10);
    dateTimeRef.year = 100 * b + d - 4800 + (m / 10);
}

uint32_t dateTime2Unix(const DateTime& dateTime) {

    uint8_t a;
    uint16_t y;
    uint8_t m;
    uint32_t JDN;

    a = (14 - dateTime.month) / 12;
    y = dateTime.year + 4800 - a;
    m = dateTime.month + (12 * a) - 3;

    JDN = dateTime.date;
    JDN += (153 * m + 2) / 5;
    JDN += 365 * y;
    JDN += y / 4;
    JDN += -y / 100;
    JDN += y / 400;
    JDN = JDN - 32045;
    JDN = JDN - JULIAN_DATE_BASE;
    JDN *= 86400;
    JDN += (dateTime.hours * 3600);
    JDN += (dateTime.minutes * 60);
    JDN += (dateTime.seconds);

    return JDN;
}

int32_t ds18b20Temp2decimal(uint16_t tempDs18b20) {

    int32_t result = 0;

    if (tempDs18b20 & 0xF800) {     // sign is "-"
        result = (~tempDs18b20 >> 4) & 0x7F;
        result *= 10000;
        result += 10000;
        if (tempDs18b20 & 0x08) result -= 5000;
        if (tempDs18b20 & 0x04) result -= 2500;
        if (tempDs18b20 & 0x02) result -= 1250;
        if (tempDs18b20 & 0x01) result -= 625;
        result *= -1;
    } else {                        // sign is "+"
        result = (tempDs18b20 >> 4) & 0x7F;
        result *= 10000;        
        if (tempDs18b20 & 0x08) result += 5000;
        if (tempDs18b20 & 0x04) result += 2500;
        if (tempDs18b20 & 0x02) result += 1250;
        if (tempDs18b20 & 0x01) result += 625;
    }
    return result;
}
