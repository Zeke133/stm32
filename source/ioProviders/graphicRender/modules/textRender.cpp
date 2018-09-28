#include <textRender.h>

TextRender::TextRender(IGraphicOutput& display, IPixelFont& defaultFont)
    :   display(display),
        font(defaultFont) {

    // initialisation
    fontInversion = 0;
    fontWidth = font.getWidth();
    fontHeight = font.getHeight();
    width = display.getWidth() / fontWidth;
    height = display.getHeight() / fontHeight;
}

void TextRender::draw8bitSymbol(uint8_t asciiCode, uint8_t invertedColor, uint8_t x, uint8_t y) const {

    // Printable symbols starts from 0x20 in ASCII table
    asciiCode = asciiCode < 20 ? 1 : asciiCode - 0x20;
    // If symbol is out if range - put '!' sign
    const uint8_t * symbolData = (uint8_t*)font.getData(asciiCode);

    for (uint8_t row = 0; row < fontHeight; row++, y++) {
        for (uint8_t column = 0, bitOffset = 7; column < fontWidth; column++, bitOffset--) {

            uint8_t pixel = (symbolData[row] >> bitOffset) & 0x01;
            display.drawPixel(x + column, y, pixel ^ invertedColor);
        }
    }
}

void TextRender::draw16bitSymbol(uint8_t asciiCode, uint8_t invertedColor, uint8_t x, uint8_t y) const {

    // Printable symbols starts from 0x20 in ASCII table
    asciiCode = asciiCode < 20 ? 1 : asciiCode - 0x20;
    // If symbol is out if range - put '!' sign
    const uint16_t * symbolData = (uint16_t*)font.getData(asciiCode);

    for (uint8_t row = 0; row < fontHeight; row++, y++) {
        for (uint8_t column = 0, bitOffset = 15; column < fontWidth; column++, bitOffset--) {

            uint8_t pixel = (symbolData[row] >> bitOffset) & 0x01;
            display.drawPixel(x + column, y, pixel ^ invertedColor);
        }
    }
}

void TextRender::putc(char symbol) {

    uint8_t x = cursorColumn * fontWidth;
    uint8_t y = cursorRow * fontHeight;

    if (fontWidth <= 8) {
        draw8bitSymbol(symbol, fontInversion, x, y);
    } else {
        draw16bitSymbol(symbol, fontInversion, x, y);
    }

    cursorColumn++;

    // move cursor if doesn't fit to borders
    if (cursorColumn >= width) {
        cursorColumn = 0;
        cursorRow ++;
    }
    if (cursorRow >= height) {
        cursorRow = 0;
        cursorColumn = 0;
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
void TextRender::setCursor(uint8_t row, uint8_t column) {

    cursorRow = (row < height) ? row : height - 1;
    cursorColumn = (column < width) ? column : width - 1;
}

// Clear row
void TextRender::clearRow(uint8_t row) {

    setCursor(row, 0);
    for(int symbol = 0; symbol < width; symbol++) {
        putc(' ');
    }
}
