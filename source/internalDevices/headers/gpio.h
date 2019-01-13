#ifndef _GPIO_H
#define _GPIO_H

extern "C" {
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
}

/*
API for general purpose IO port setup
*/
namespace GPIO {

void initPins(GPIO_TypeDef* port, uint32_t pins, uint32_t mode, uint32_t outType, uint32_t pull, uint32_t speed = LL_GPIO_SPEED_FREQ_HIGH);

}

#endif
