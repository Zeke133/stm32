#ifndef _O_STREAM_H
#define _O_STREAM_H

// using
#include <ITextOutput.h>
#include <convertation.h>

/*
Output stream simple implementation for better usability.
*/
class OStream {

public:

    OStream(ITextOutput& driver) : device(driver) {};

    enum class OutSet : char {
        hex,
        dec
    };

    OStream& operator<<(enum OutSet manipulator);       // for output manipulators
    OStream& operator<<(char symbol);                   // for single character
    OStream& operator<<(const char * string);           // for C-string
    OStream& operator<<(uint8_t);
    OStream& operator<<(int16_t);
    OStream& operator<<(uint16_t);
    OStream& operator<<(int32_t num);
    OStream& operator<<(uint32_t num);

private:

    ITextOutput& device;
    char outputNumbersBase = 10;

};

#endif
