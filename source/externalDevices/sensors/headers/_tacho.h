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

72000000 / 720 = 100000 = 10uS for tick
Measure period 'T' with each revoluton of crankshaft in ticks. Tick can be converted to us or RPM.

Calculate by table an ingnition angle 'w'. p - pressure, t - temperature. f(rpm, t, p) = w.
Convert w to ticks. Table of 1 degree to ticks.

Calculate ignition coil charge time 'Ict'.

Calculate Timer value to interrupt - w + Ict.

Set timer for coil charge start and end.

Turn ON ignition coil charge on interrupt and turn OFF on next.

*/
class Tacho {

friend void TIM2_IRQHandler(void);

public:

    Tacho();

    // delete copy constructor and assignment operator
    Tacho(const Tacho&) = delete;
    Tacho& operator=(const Tacho&) = delete;

    uint32_t getRpm(void);
    uint32_t getTicksPerRotation(void);
    uint32_t getSpeed(void);

    void setRotationLength(uint32_t len);

private:

    const uint32_t timerClk = 100000;   // 10uS for tick
    const uint32_t timerPrescaler = SystemCoreClock / timerClk;

    static uint16_t currentTicks;
    static uint32_t ticks;

    uint32_t rotateLength;  // in mm km10-6

};

#endif
