#ifndef _GPIO_H
#define _GPIO_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

void GPIO_Init_My(GPIO_TypeDef* port, uint16_t pins, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);

#endif