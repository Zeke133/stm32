#ifndef _TEXT_RENDER_H
#define _TEXT_RENDER_H

// implements
#include <ITerminal.h>
// using
#include <IPixelFont.h>
#include <IGraphicOutput.h>

/*
API for rendering text on graphic displays
*/
class TextRender : public ITerminal {

public:

    TextRender(IGraphicOutput& display, IPixelFont& defaultFont);

    // delete copy constructor and assignment operator
    TextRender(const TextRender&) = delete;
    TextRender& operator=(const TextRender&) = delete;

    void puts(const char* str);
    void putc(char symb);

    uint8_t getWidth(void) const { return width; };
    uint8_t getHeight(void) const { return height; };

    void clearRow(uint8_t row);
    void setCursor(uint8_t row, uint8_t column);

    // FLUSH!!!

private:

    void draw8bitSymbol(uint8_t asciiCode, uint8_t invertedColor, uint8_t x, uint8_t y) const;
    void draw16bitSymbol(uint8_t asciiCode, uint8_t invertedColor, uint8_t x, uint8_t y) const;

    IGraphicOutput& display;
    IPixelFont& font;

    // Text terminal parameters
    uint8_t width;              // rows
    uint8_t height;             // lines
    uint8_t cursorRow = 0;
    uint8_t cursorColumn = 0;

    // Font parameters
    uint8_t fontWidth;
    uint8_t fontHeight;
    uint8_t fontInversion;

};

#endif
