#ifndef _O_STREAM_H
#define _O_STREAM_H

#include <usart.h>

#include <convertation.h>


// Good idea to make an output stream for usability
// but Vtable is not supported by embedded c++ compiller
// SO IT DOESN'T WORK!


class OStream {
    
public:

    enum class OutSet : char {
        hex,
        dec
    };   

    OStream(Usart& device);

    OStream& operator<<(enum OutSet manipulator);       // for output manipulators
    OStream& operator<<(char symbol);                   // for single character
    OStream& operator<<(const char * string);           // for C-string
    OStream& operator<<(uint8_t);
    OStream& operator<<(int16_t);
    OStream& operator<<(uint16_t);
    OStream& operator<<(int32_t num);
    OStream& operator<<(uint32_t num);

private:

    Usart& usart;

    char outputNumbersBase = 10;

};


#endif