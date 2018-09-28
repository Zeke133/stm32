#ifndef _I_GRAPHIC_OUTPUT_H
#define _I_GRAPHIC_OUTPUT_H

#include <stdint.h>

/*
Interface for graphic output.
*/
class IGraphicOutput {

public:

    virtual void drawPixel(uint8_t x, uint8_t y, uint8_t pixelVal) = 0;
    virtual void fill(uint8_t bit) = 0;
    virtual void update(void) = 0;
    // virtual void updatePage(uint8_t pageNumber, uint8_t startColumn) = 0;

    virtual uint8_t getWidth(void) const = 0;
    virtual uint8_t getHeight(void) const = 0;

};

#endif
