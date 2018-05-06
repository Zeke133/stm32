#ifndef Dallas_1wire_H
#define Dallas_1wire_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#include <gpio.h>
#include <delay.h>


class Dallas_1wire_controller {

public:

	Dallas_1wire_controller(Delay& timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	Dallas_1wire_controller(Delay& timer, GPIO_TypeDef* GPIOx,
						uint16_t GPIO_Pin,
						uint8_t InitTOs[3],
						uint8_t WriteTOs[3],
						uint8_t ReadTOs[3]);

	uint8_t ReadROM(uint8_t * resp);
	uint8_t MatchROM(const uint8_t * romCode /*64 bit RomCode*/);
	uint8_t SkipROM(void);

protected:

	void WriteTimeslot(uint8_t bit);
	uint8_t ReadTimeslot(void);

	void WriteByte(uint8_t byte);
	uint8_t ReadByte(void);

	const Delay& wait;					// timer

private:

	uint8_t Initialization(void);

	GPIO_TypeDef* GPIOx;				// Port to run
	const uint16_t GPIO_Pin;			// Pin of port for communication

	const uint8_t InitTOs[3];			// Timeouts in initialization - us
	const uint8_t WriteTOs[3];			// Timeouts in write timeslot - us
	const uint8_t ReadTOs[3];			// Timeouts in read timeslot - us

};



#endif
