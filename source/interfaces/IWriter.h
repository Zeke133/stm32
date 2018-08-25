#ifndef _I_OUTPUT_H
#define _I_OUTPUT_H

#include <stdint.h>

/*
Interface for text output.
*/
class IWriter {

public:

    virtual void putc(char byte) = 0;
    virtual void puts(const char *str) = 0;

};

#endif

