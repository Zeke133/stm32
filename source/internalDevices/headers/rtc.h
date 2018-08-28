#ifndef _RTC_H
#define _RTC_H

// using
extern "C" {
#include <stm32f10x_rcc.h>
#include <stm32f10x_rtc.h>
#include <stm32f10x_pwr.h>
}
#include <IDelayer.h>
#include <convertation.h>

/*
Internal real time clock driver API.
*/
class RealTimeClock {

public:

    RealTimeClock(IDelayer& wait);

    // delete copy constructor and assignment operator
    RealTimeClock(const RealTimeClock&) = delete;
    RealTimeClock& operator=(const RealTimeClock&) = delete;

    void setTime(const struct DateTime& dateTime);
    void setTime(uint32_t unixTime);

    void getTime(struct DateTime& dateTime) const;
    uint32_t getUnixTime(void) const;

private:

    inline uint8_t init(void);

};

#endif
