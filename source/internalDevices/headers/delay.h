#ifndef _DELAY_H
#define _DELAY_H

extern "C" {
#include <stm32f10x.h>
}

#include <IDelayer.h>
#include <IProfiler.h>

extern "C" {
    void SysTick_Handler();
}

/*
Delay implementation based on SysTick timer and DWT.
Supports delays in Miliseconds and Microseconds.
*/

class Delay : public IProfiler, IDelayer {

    // Interrypt handler
    friend void SysTick_Handler();
    friend int main(void);

public:

    void ms(uint32_t ms);                       // acurancy of measurment is 1 ms. because of period of timer in 1 ms.
    void us(uint32_t us);                       // error may acure because of overflow every 59.65232354166667 sec.

    virtual void startProfiling(void);          // start counting of cpu ticks
    virtual uint32_t getExecutionTicks(void);   // get amount of CPU ticks from measuring start

    static uint32_t sysTicks2us(uint32_t ticks);

    // delete copy constructor and assignment operator
    Delay(const Delay&) = delete;
    Delay& operator=(const Delay&) = delete;

private:
    
    Delay();

    uint32_t getDWTTicksValue(void) const;

    static uint32_t sysTicksPerUs;
    static uint32_t msCount;                // For store tick counts in ms
    uint32_t profilingStart;                // Start value of DWT
    
};

#endif


