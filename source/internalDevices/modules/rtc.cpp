#include <rtc.h>

RealTimeClock::RealTimeClock(IDelayer& delayer) {

    if (init()) {

        // On first RTC init: setting default dateTime: 01.01.2018 00:00:00
        struct DateTime time {

            .hours = 0,
            .minutes = 0,
            .seconds = 0,
            .date = 1,
            .wday = 0,
            .month = 1,
            .year = 2018
        };

        // Must have a delay after RTC initialization to set a time
        delayer.ms(500);
        setTime(time);
    }
}

uint8_t RealTimeClock::init(void) {

    // Enable tacting of PowerManagement and BackupManagement modules
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    // Enable access to Backup data sector
    PWR_BackupAccessCmd(ENABLE);

    // Initialize RTC if it's OFF
    if ((RCC->BDCR & RCC_BDCR_RTCEN) != RCC_BDCR_RTCEN) {

        RCC_BackupResetCmd(ENABLE);
        RCC_BackupResetCmd(DISABLE);

        // Set clock source 32768 oscillator
        RCC_LSEConfig(RCC_LSE_ON);
        while ((RCC->BDCR & RCC_BDCR_LSERDY) != RCC_BDCR_LSERDY) {}
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

        // Set divider to count seconds with RTC
        RTC_SetPrescaler(0x7FFF);

        // Turn ON RTC
        RCC_RTCCLKCmd(ENABLE);

        RTC_WaitForSynchro();

        return 1;
    }
    return 0;
}

void RealTimeClock::setTime(const struct DateTime& dateTime) {

    RTC_SetCounter(dateTime2Unix(dateTime));
}

void RealTimeClock::setTime(uint32_t unixTime) {

    RTC_SetCounter(unixTime);
}

void RealTimeClock::getTime(struct DateTime& dateTime) const {

    unix2DateTime(dateTime, RTC_GetCounter());
}

uint32_t RealTimeClock::getUnixTime(void) const {

    return RTC_GetCounter();
}
