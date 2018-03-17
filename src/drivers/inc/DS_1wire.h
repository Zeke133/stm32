#ifndef DS_1WIRE_H
#define DS_1WIRE_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#include "delay.h"


class DS_1Wire_controller {

public:

	DS_1Wire_controller(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

	bool GetCodes(unsigned char req, unsigned char * resp);

private:

	bool Initialization(void);
	void WriteTimeslot(bool bit);
	bool ReadTimeslot(void);

	void WriteByte(unsigned char byte);
	unsigned char ReadByte(void);

	const GPIO_TypeDef* GPIOx;
	const uint16_t GPIO_Pin;

};



#endif
