#ifndef DELAY_H
#define DELAY_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

void Delay(uint32_t ms);
void DelayMC(uint32_t mc);

#endif
