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
            .year = 2019
        };

        // Must have a delay after RTC initialization to set a time
        delayer.ms(500);
        setTime(time);
    }
}

uint8_t RealTimeClock::init(void) {

    // Enable tacting of PowerManagement and BackupManagement modules
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR | LL_APB1_GRP1_PERIPH_BKP);
    // Enable access to Backup data sector
    LL_PWR_EnableBkUpAccess();

    // Initialize RTC if it's OFF
    if (!LL_RCC_IsEnabledRTC()) {

        LL_RCC_ForceBackupDomainReset();
        LL_RCC_ReleaseBackupDomainReset();

        // Set clock source 32768 oscillator
        LL_RCC_LSE_Enable();
        
        while (!LL_RCC_LSE_IsReady()) ;
        LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);

        // Set divider to count seconds with RTC
        LL_RTC_SetAsynchPrescaler(RTC, 0x7FFF);

        // Turn ON RTC
        LL_RCC_EnableRTC();

        LL_RTC_WaitForSynchro(RTC);

        return 1;
    }
    return 0;
}

void RealTimeClock::setTime(const struct DateTime& dateTime) {

    LL_RTC_TIME_Set(RTC, dateTime2Unix(dateTime));
}

void RealTimeClock::setTime(uint32_t unixTime) {

    LL_RTC_TIME_Set(RTC, unixTime);
}

void RealTimeClock::getTime(struct DateTime& dateTime) const {

    unix2DateTime(dateTime, LL_RTC_TIME_Get(RTC));
}

uint32_t RealTimeClock::getUnixTime(void) const {

    return LL_RTC_TIME_Get(RTC);
}
