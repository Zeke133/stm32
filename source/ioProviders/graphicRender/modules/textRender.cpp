#include <textRender.h>

TextRender::TextRender(Ssd1306& display, IPixelFont& defaultFont)
    :   display(display),
        defaultFont(defaultFont) {}

void TextRender::putc(char symbol) {

    IPixelFont& font = defaultFont;
    uint8_t columns = font.getWidth();
    uint8_t rows = font.getHeight();
    uint8_t inverted = 0;

    const uint8_t * bitmapPtr = (uint8_t*)font.getData(symbol - 0x20);

    // use current coordinates as top,left angle of object
    // move cursor if doesn't fit to borders
    if (cursorX + columns >= width) {
        cursorX = 0;
        cursorY += rows;
    }
    if (cursorY + rows >= height) {
        cursorY = 0;
        cursorX = 0;
    }

    // Use the font to write
    uint8_t x, y, pixel;
    // update cursor
    cursorX += columns;
    for (uint8_t row = 0; row < rows; row++) {
        for (uint8_t column = 0; column < columns; column++) {
            x = cursorX - column;
            y = cursorY + row;
            pixel = (bitmapPtr[row] >> column) & 0x01;
            pixel ^= inverted;
            display.drawPixel(x, y, pixel);
        }
    }

}

// Write string to display-buffer
void TextRender::puts(const char * str) {

    // Write until null-byte
    while (*str) {
        putc(*str);
        str++;
    }
}

// Position the cursor
void TextRender::setCursor(uint8_t x, uint8_t y) {

    cursorX = (x >= width) ? 0 : x;
    cursorY = (y >= height) ? 0 : y;
}
