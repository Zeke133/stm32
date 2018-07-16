#ifndef _I_OUTPUT_H
#define _I_OUTPUT_H

#include <stdint.h>

class IWriter {

public:

    virtual void putc(char byte) = 0;
    virtual void puts(const char *str) = 0;

};

#endif
