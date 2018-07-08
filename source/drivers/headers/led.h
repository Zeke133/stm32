#ifndef _LED_H
#define _LED_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#include <gpio.h>

/*
Driver for operations with signaling LEDs
*/
class LED {

friend int main(void);

public:

    void on();
    void off();
    void invert();

    // delete copy constructor and assignment operator
    LED(const LED&) = delete;
    LED& operator=(const LED&) = delete;

private:

    LED(GPIO_TypeDef* port, uint16_t pin);

	GPIO_TypeDef* port;
	uint16_t pin;
    uint32_t state;

};

#endif

