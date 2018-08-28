#ifndef _TACHO_H
#define _TACHO_H

// using
extern "C" {
#include <stm32f10x_tim.h>
}
#include <gpio.h>

// IRQ handler, extern "C" function, because of c++ bad names
extern "C" {
    void TIM2_IRQHandler(void);
}

/*
ALPHA
API for tachometer on TIM2 external interupt
*/
class Tacho {

public:

    Tacho();

    // delete copy constructor and assignment operator
    Tacho(const Tacho&) = delete;
    Tacho& operator=(const Tacho&) = delete;

    uint32_t getRpm(void);
    uint32_t getSpeed(void);

    void setRotationLength(uint32_t len);

private:

    uint32_t rotateLength;  // in mm km10-6

};

#endif
