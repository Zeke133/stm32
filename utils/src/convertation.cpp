#include <convertation.h>


const char * itoa(unsigned int val, int base) {

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

		if(val == 0) return &buf[i];
		i--;
	}
	return "ERROR!";
}

void unix2DateTime(DateTime& dateTimeRef, uint32_t unixTime) {
    
    // Make unit tests!777
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

    //
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
