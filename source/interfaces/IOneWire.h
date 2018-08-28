#ifndef _I_ONE_WIRE_H
#define _I_ONE_WIRE_H

#include <stdint.h>

/*
Interface for providing 1-Wire protocol functionality.
*/
class IOneWire {

public:

    virtual uint8_t ReadROM(uint8_t * resp) = 0;
    virtual uint8_t MatchROM(const uint8_t * romCode /*64 bit RomCode*/) = 0;
    virtual uint8_t SkipROM(void) = 0;

    virtual uint8_t Initialization(void) = 0;

    virtual void WriteTimeslot(uint8_t bit) = 0;
    virtual uint8_t ReadTimeslot(void) = 0;

    virtual void WriteByte(uint8_t byte) = 0;
    virtual uint8_t ReadByte(void) = 0;

};

#endif
