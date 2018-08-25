#ifndef	_I_PIXEL_FONT_H
#define _I_PIXEL_FONT_H

#include <stdint.h>

/*
Interface for graphic fonts.
*/
class IPixelFont {

public:

    virtual uint8_t getWidth(void) const = 0;
    virtual uint8_t getHeight(void) const = 0;

    virtual const void * getData(uint8_t asciiCode) const = 0;

};

#endif
