#ifndef _ONE_WIRE_H
#define _ONE_WIRE_H

// implements
#include <IOneWire.h>
// using
#include <IDelayer.h>

#include <gpio.h>
#include <delay.h>

#include <crc.h>

/*
Software driver for Dallas 1-Wire interface based on GPIO
Data line must be pulled to V+ with 4.7kOhm resistor according Dallas datasheet.
*/
class OneWire : public IOneWire {

public:

    OneWire(IDelayer &timer, GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);

    // delete copy constructor and assignment operator
    OneWire(const OneWire &) = delete;
    OneWire &operator=(const OneWire &) = delete;

    uint8_t ReadROM(uint8_t *resp);
    uint8_t MatchROM(const uint8_t *romCode /*64 bit RomCode*/);
    uint8_t SkipROM(void);

    uint8_t Initialization(void);

    void WriteTimeslot(uint8_t bit);
    uint8_t ReadTimeslot(void);

    void WriteByte(uint8_t byte);
    uint8_t ReadByte(void);

protected:

    IDelayer &delayer;             // timer

private:

    GPIO_TypeDef *GPIOx;        // Port to run
    const uint32_t GPIO_Pin;    // Pin of port for communication

    static constexpr uint8_t InitTOs[3] = {48, 7, 41}; // Timeouts in initialization - us
    static constexpr uint8_t WriteTOs[3] = {3, 90, 5}; // Timeouts in write timeslot - us
    static constexpr uint8_t ReadTOs[3] = {1, 7, 90};  // Timeouts in read timeslot - us

    // Timings from other libs
    // 48, 7, 41
    // 10, 55, x
    // 3, 10, 53
};

#endif
