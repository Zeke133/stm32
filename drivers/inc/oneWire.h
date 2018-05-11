#ifndef _ONE_WIRE_H
#define _ONE_WIRE_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#include <gpio.h>
#include <delay.h>


class OneWire {

public:

	OneWire(Delay& timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t InitTOs[3], uint8_t WriteTOs[3], uint8_t ReadTOs[3]);

						// 60, 10, 50
						// 3, 90, 5
						// 1, 7, 90

						// Timings from other libs
						// 48, 7, 41
						// 10, 55, x
						// 3, 10, 53

	uint8_t ReadROM(uint8_t * resp);
	uint8_t MatchROM(const uint8_t * romCode /*64 bit RomCode*/);
	uint8_t SkipROM(void);

	uint8_t Initialization(void);

	void WriteTimeslot(uint8_t bit);
	uint8_t ReadTimeslot(void);

	void WriteByte(uint8_t byte);
	uint8_t ReadByte(void);

protected:

	const Delay& wait;					// timer

private:

	GPIO_TypeDef* GPIOx;				// Port to run
	const uint16_t GPIO_Pin;			// Pin of port for communication

	const uint8_t InitTOs[3];			// Timeouts in initialization - us
	const uint8_t WriteTOs[3];			// Timeouts in write timeslot - us
	const uint8_t ReadTOs[3];			// Timeouts in read timeslot - us

};



#endif