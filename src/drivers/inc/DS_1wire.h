#ifndef DS_1WIRE_H
#define DS_1WIRE_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#include <gpio.h>
#include <delay.h>


class DS_1Wire_controller {

public:

	DS_1Wire_controller(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	DS_1Wire_controller(GPIO_TypeDef* GPIOx,
						uint16_t GPIO_Pin,
						uint8_t InitTOs[3],
						uint8_t WriteTOs[3],
						uint8_t ReadTOs[3]);

	uint8_t GetCodes(unsigned char req, unsigned char * resp);

private:

	uint8_t Initialization(void);
	void WriteTimeslot(uint8_t bit);
	uint8_t ReadTimeslot(void);

	void WriteByte(uint8_t byte);
	uint8_t ReadByte(void);

	GPIO_TypeDef* GPIOx;				// Port to run
	const uint16_t GPIO_Pin;			// Pin of port for communication

	const uint8_t InitTOs[3];			// Timeouts in initialization - us
	const uint8_t WriteTOs[3];			// Timeouts in write timeslot - us
	const uint8_t ReadTOs[3];			// Timeouts in read timeslot - us

};



#endif
