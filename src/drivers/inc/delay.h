#ifndef DELAY_H
#define DELAY_H

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

    void ms(uint32_t ms);
    void us(uint32_t us);

    // delete copy constructor and assignment operator
    Delay(const Delay&) = delete;
    Delay& operator=(const Delay&) = delete;

private:
    
    Delay();

    uint32_t ClkCyclPerUs;
    static uint32_t msTicks;    // For store tick counts in ms
    
};

/*
Global pointer to class instanse. Because we can't make it singleton without including c++ libs.
*/
extern Delay* delay;

#endif


