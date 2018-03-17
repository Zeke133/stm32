#ifndef LED_H
#define LED_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#include "gpio.h"

class LED {

public:

    LED(GPIO_TypeDef* port, uint16_t pin);
    void on();
    void off();

private:

	GPIO_TypeDef* port;
	uint16_t pin;

};

#endif

