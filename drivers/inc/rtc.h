#ifndef _RTC_H
#define _RTC_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_rtc.h>
#include <stm32f10x_pwr.h>

#include <delay.h>
#include <convertation.h>


class RealTimeClock {

    public:

        RealTimeClock(Delay& wait);

        void setTime(const struct DateTime& dateTime);
        void setTime(uint32_t unixTime);

        void getTime(struct DateTime& dateTime) const;
        uint32_t getUnixTime(void) const;
        
    private:

        inline uint8_t init(void);

};

#endif


