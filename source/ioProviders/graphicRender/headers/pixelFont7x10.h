#ifndef _PIXEL_FONT_7X10_H
#define _PIXEL_FONT_7X10_H

// implements
#include <IPixelFont.h>

class Font_7x10 : public IPixelFont {

public:

    uint8_t getWidth(void) const { return width; };
    uint8_t getHeight(void) const { return height; };

    const void * getData(uint8_t asciiCode) const {
        if (asciiCode > numberOfSymbols) return data[0];
        return data[asciiCode];
    };

private:

    static const uint8_t width = 7;
    static const uint8_t height = 10;
    static const uint8_t numberOfSymbols = 95;

    static const uint8_t data[numberOfSymbols][height];

};

#endif
