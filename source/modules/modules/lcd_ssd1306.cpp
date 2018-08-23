#include <lcd_ssd1306.h>

Lcd_ssd1306::Lcd_ssd1306(I2c& i2c, IDelayer& delay, IPixelFont& defaultFont)
    :   i2c(i2c),
        wait(delay),
        fontDefault(defaultFont) {

    init();
}

void Lcd_ssd1306::writeCommand(uint8_t byte) {

    i2c.startTransmit();
    i2c.write(byte);
    i2c.stopTransmit();
    wait.us(2);
}

void Lcd_ssd1306::writeData(const uint8_t * buffer, uint16_t size) {

    i2c.startTransmit();

    for(uint16_t i = 0; i < size; i++) {

        i2c.write(buffer[i]);
    }

    i2c.stopTransmit();
    wait.us(2);
}

void Lcd_ssd1306::init(void) {

    // Wait for the screen to boot
    wait.ms(100);

    // Init LCD
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetDispOff));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetMemAdrMode));
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetMemAdrModePage));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetPageStartAddr) | 0);

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetCOMOutScanDirRemap));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetLowColStartAddr) | 0);
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetHighColStartAddr) | 0);

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetDispStartLine) | 0);

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetContrastCtrl));
    writeCommand(0xFF);

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetSegmentRemapOn));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetNormalDisp));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetMultiplexRatio));
    writeCommand(0x3F);

    writeCommand(static_cast<uint8_t>(DisplayCmd::EntireDispResume));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetDispOffset));
    writeCommand(0x00);

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetDispClckDiv));
    writeCommand(0xF0);

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetPrechargePer));
    writeCommand(0x22);

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetCOMPinsHWConf));
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetCOMAlt) | static_cast<uint8_t>(DisplayCmd::SetCOMNoRemap));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetVcomhDeselectLev));
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetVcomh077Vcc));

    writeCommand(static_cast<uint8_t>(DisplayCmd::ChargePumpSet));
    writeCommand(static_cast<uint8_t>(DisplayCmd::EnableChargePump));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetDispOn));

    // Clear screen
    fill(0);

    // Flush buffer to screen
    update();
}

// Fill the whole screen with the given bit, color depends of display settings
void Lcd_ssd1306::fill(uint8_t bit) {

    uint8_t filler = (bit) ? 0x00 : 0xFF;

    for(int page = 0; page < pagesNum; page++) {

        for(int segment = 0; segment < width; segment++) {

            displayBuffer[page][segment] = filler;
        }
    }
}

// Write display-buffer to the LCD
void Lcd_ssd1306::update(void) {

    for (uint8_t page = 0; page < pagesNum; page++) {

        // 10.1.13 Set Page Start Address for Page Addressing Mode (B0h~B7h).
        // This command positions the page start address from 0 to 7 in GDDRAM under Page Addressing Mode.
        // Please refer to Table 9-1 and Section 10.1.3 for details.
        writeCommand(0xB0 + page);

        // 10.1.1 Set Lower Column Start Address for Page Addressing Mode (00h~0Fh).
        // This command specifies the lower nibble of the 8-bit column start address for
        // the display data RAM under Page Addressing Mode.
        // The column address will be incremented by each data access.
        // Please refer to Section Table 9-1 and Section 10.1.3 for details.
        writeCommand(0x00);

        // 10.1.2 Set Higher Column Start Address for Page Addressing Mode (10h~1Fh).
        // This command specifies the higher nibble of the 8-bit column start address for
        // the display data RAM under Page Addressing Mode.
        // The column address will be incremented by each data access.
        // Please refer to Section Table 9-1 and Section 10.1.3 for details.
        writeCommand(0x10);

        writeData(displayBuffer[page], width);
    }
}

// Set one pixel in the display-buffer
// x        => horizontal
// y        => vertical
// pixelVal => Pixel color
void Lcd_ssd1306::drawPixel(uint8_t x, uint8_t y, uint8_t pixelVal)
{
    if (x >= width || y >= height) {
        // Don't write outside the buffer
        return;
    }

    // Draw in pixel
    if (pixelVal) {

        displayBuffer[y / 8][x] |= 1 << (y % 8);
    } 
    else {

        displayBuffer[y / 8][x] &= ~(1 << (y % 8));
    }
}

// Draw 1 char to the display-buffer
// ch       => char symbol to display
void Lcd_ssd1306::putc(char symbol) {

    IPixelFont& font = fontDefault;
    int columns = font.getWidth();
    int rows = font.getHeight();
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
    for (int row = 0; row < rows; row++) {
        
        for (int column = 0; column < columns; column++) {

            x = cursorX + column;
            y = cursorY + row;
            pixel = (bitmapPtr[row] >> column) & 0x01;
            pixel ^= inverted;
            drawPixel(x, y, pixel);
        }
    }

    // update cursor
    cursorX += columns;
    cursorY += rows;
}

// Write string to display-buffer
void Lcd_ssd1306::puts(const char * str) {

    // Write until null-byte
    while (*str) {

        putc(*str);
        str++;
    }
}

// Position the cursor
void Lcd_ssd1306::cursorGoTo(uint8_t x, uint8_t y) {

    cursorX = (x >= width) ? 0 : x;
    cursorY = (y >= height) ? 0 : y;
}


