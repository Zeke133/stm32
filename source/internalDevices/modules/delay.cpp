#include <delay.h>

// SysTick_Handler function will be called every 1 ms
void SysTick_Handler() {

    if (Delay::msCount) Delay::msCount--;
}

// Class implementation
uint32_t Delay::msCount;
uint32_t Delay::sysTicksPerUs;

Delay::Delay() {

    /* Configuring SysTick timer for implementation of 1ms delays by timer interrupt */

    // Update SystemCoreClock value
    SystemCoreClockUpdate();

    // Configure the SysTick timer to overflow every 1 ms
    SysTick_Config(SystemCoreClock / 1000);
    msCount = 0;

    /* Configuring DWT timer for implementation of 1us delays by cycle */
    sysTicksPerUs = SystemCoreClock / 1000000;

    // DWT_Init
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {

        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }

    // For measuring of execution time
    profilingStart = 0;
}

void Delay::ms(uint32_t ms) {

    // Reload ms value
    msCount = ms;
    // wait
    while (msCount) ;
}

void Delay::us(uint32_t us) {

    uint32_t ticksBegin = getDWTTicksValue();
    uint32_t ticksEnd = ticksBegin + us * sysTicksPerUs;
    // wait
    if (ticksEnd < ticksBegin)
        while (getDWTTicksValue() > ticksBegin || getDWTTicksValue() < ticksEnd) ;
    else
        while (getDWTTicksValue() < ticksEnd) ;
}

inline uint32_t Delay::getDWTTicksValue(void) const {

    return DWT->CYCCNT;
}

inline void Delay::startProfiling(void) {

    profilingStart = getDWTTicksValue();
}

inline uint32_t Delay::getExecutionTicks(void) {

    uint32_t ticksNow = getDWTTicksValue();

    // overflow happened
    // no overflow or execution time is more than 59 seconds what is unlikely

    return (profilingStart > ticksNow) ? (0xffffffff - profilingStart + ticksNow) : (ticksNow - profilingStart);
}

uint32_t Delay::sysTicks2us(uint32_t ticks) {

    return ticks / sysTicksPerUs;
}
