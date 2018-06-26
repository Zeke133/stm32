#ifndef _O_STREAM_H
#define _O_STREAM_H

#include <outputInterface.h>

#include <convertation.h>

class OStream {
    
public:

    OStream(OutputInterface& device);

    OStream& operator<<(char symbol);                   // for single character
    OStream& operator<<(const char * string);           // for C-string
    OStream& operator<<(enum OutSet manipulator);       // for output manipulators
    OStream& operator<<(unsigned char num);                   // for integers
    OStream& operator<<(short num);                     // for integers
    OStream& operator<<(int num);                       // for integers
    
    enum class OutSet : char {
        hex,
        dec
    };        

private:

    const OutputInterface& device;

    char outputNumbersBase = 10;

};


#endif