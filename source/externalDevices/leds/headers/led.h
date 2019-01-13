#ifndef _LED_H
#define _LED_H

// using
#include <gpio.h>

/*
API for easy operations with signaling LED
*/
class LED {

public:

    LED(GPIO_TypeDef* port, uint32_t pin);

    // delete copy constructor and assignment operator
    LED(const LED&) = delete;
    LED& operator=(const LED&) = delete;

    void on();
    void off();
    void invert();

private:

    GPIO_TypeDef* port;
    uint32_t pin;
    uint32_t state;

};

#endif
