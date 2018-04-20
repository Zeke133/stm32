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
						unsigned int InitTOs[3],
						unsigned int WriteTOs[3],
						unsigned int ReadTOs[3]);

	bool GetCodes(unsigned char req, unsigned char * resp);

private:

	bool Initialization(void);
	void WriteTimeslot(unsigned char bit);
	unsigned char ReadTimeslot(void);

	void WriteByte(unsigned char byte);
	unsigned char ReadByte(void);

	GPIO_TypeDef* GPIOx;				// Port to run
	const uint16_t GPIO_Pin;			// Pin of port for communication

	const unsigned int InitTOs[3];		// Timeouts in initialization - us
	const unsigned int WriteTOs[3];		// Timeouts in write timeslot - us
	const unsigned int ReadTOs[3];		// Timeouts in read timeslot - us

};



#endif
