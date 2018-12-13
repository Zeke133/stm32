#ifndef _PIXEL_FONT_16X26_H
#define _PIXEL_FONT_16X26_H

// implements
#include <IPixelFont.h>

/*
Data representation of pixel font.
Each symbol is array of lines.
MSB is right pixel.
*/
class Font_16x26 : public IPixelFont {

public:

    uint8_t getWidth(void) const { return width; };
    uint8_t getHeight(void) const { return height; };

    const void * getData(uint8_t asciiCode) const {
        // If symbol is out if range - put '!' sign
        return asciiCode < numberOfSymbols ? data[asciiCode] : data[1];
    };

private:

    static constexpr uint8_t width = 16;
    static constexpr uint8_t height = 26;
    static constexpr uint8_t numberOfSymbols = 95;

    static const uint16_t data[numberOfSymbols][height];

};

#endif
