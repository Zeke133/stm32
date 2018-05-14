#ifndef _DELAY_H
#define _DELAY_H

#include <stm32f10x.h>

extern "C" {
    void SysTick_Handler();
}

/*
Delay implementation based on SysTick timer.
Supports delays in Miliseconds and Microseconds.
*/
class Delay {

    // Interrypt handler
    friend void SysTick_Handler();
    friend int main(void);

public:

    void ms(uint32_t ms) const;
    void us(uint32_t us) const;

    inline void startProfiling(void);
    inline uint32_t getExecutionTime(void) const;

    inline static uint32_t sysTicks2us(uint32_t ticks);

    // delete copy constructor and assignment operator
    Delay(const Delay&) = delete;
    Delay& operator=(const Delay&) = delete;

private:
    
    Delay();

    inline uint32_t getSysTickValue(void) const;

    static uint32_t sysTicksPerUs;
    static uint32_t msCount;            // For store tick counts in ms
    uint32_t profilingStart;
    
};

#endif


