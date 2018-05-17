#ifndef _TACHO_H
#define _TACHO_H


#include <stm32f10x_tim.h>

#include <gpio.h>


// IRQ handler, extern "C" function, because of c++ bad names
extern "C" {
    void TIM2_IRQHandler(void);
}

class Tacho {

friend int main(void);
    
public:

    uint32_t getRpm(void);
    uint32_t getSpeed(void);

    void setRotationLength(uint32_t len);

    // delete copy constructor and assignment operator
    Tacho(const Tacho&) = delete;
    Tacho& operator=(const Tacho&) = delete;

private:

    uint32_t rotateLength;  // in mm km10-6

    Tacho();

};



#endif

