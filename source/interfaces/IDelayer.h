#ifndef _I_DELAYER_H
#define _I_DELAYER_H

#include <stdint.h>

/*
Interface to provide time delay functionality in easy way.
*/
class IDelayer {

public:

    virtual void ms(uint32_t ms) = 0;
    virtual void us(uint32_t us) = 0;

};

#endif
