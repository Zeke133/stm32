#ifndef _TEXT_RENDER_H
#define _TEXT_RENDER_H

// implements
#include <ITextOutput.h>
// using
#include <IPixelFont.h>
#include <ssd1306.h>

/*
API for rendering text on graphic displays
*/
class TextRender : public ITextOutput {

public:

    void puts(const char* str);
    void putc(char symb);

    void setCursor(uint8_t x, uint8_t y);

    // delete copy constructor and assignment operator
    TextRender(const TextRender&) = delete;
    TextRender& operator=(const TextRender&) = delete;

private:

    TextRender(Ssd1306& display, IPixelFont& defaultFont);

    Ssd1306& display;
    IPixelFont& defaultFont;

    static const uint8_t width = 128;
    static const uint8_t height = 64;
    static const uint8_t pagesNum = height / 8;

    uint8_t cursorX = 0;
    uint8_t cursorY = 0;

};

#endif
