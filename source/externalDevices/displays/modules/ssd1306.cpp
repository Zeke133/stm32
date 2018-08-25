#include <ssd1306.h>

Ssd1306::Ssd1306(I2c& i2c, IDelayer& delay, IPixelFont& defaultFont)
    :   i2c(i2c),
        wait(delay),
        fontDefault(defaultFont) {

    initialization();
}

/*
| b7 |  b6  | b5 | b4 | b3 | b2 | b1 | b0 |
| Co | D/C# | 0  | 0  | 0  | 0  | 0  | 0  |
If the Co bit is set as logic “0”, the transmission of the following information will contain data bytes only.
The D/C# bit determines the next data byte is acted as a command or a data.
If the D/C# bit is set to logic “0”, it defines the following data byte as a command.
If the D/C# bit is set to logic “1”, it defines the following data byte as a data which will be stored at the GDDRAM.
The GDDRAM column address pointer will be increased by one automatically after each data write.
*/

void Ssd1306::writeCommand(uint8_t byte) {

    i2c.startTransmit();
    // Control bit to 1 and D/C# to 0: means command will be transmited
    i2c.write(0x80);
    i2c.write(byte);
    i2c.stopTransmit();
}

void Ssd1306::writeData(const uint8_t * buffer, uint16_t size) {

    i2c.startTransmit();
    // Control bit to 0 and D/C# to 0: means command followed with data will be transmited
    i2c.write(0x40);
    for(uint16_t i = 0; i < size; i++) {

        i2c.write(buffer[i]);
    }
    i2c.stopTransmit();
}

// Fill the whole screen with the given bit, color depends of display settings
void Ssd1306::fill(uint8_t bit) {

    uint8_t filler = (bit) ? 0xFF : 0x00;

    for(int page = 0; page < pagesNum; page++) {

        for(int segment = 0; segment < width; segment++) {

            displayBuffer[page][segment] = filler;
        }
    }
}

// Flush page of display-buffer to display
void Ssd1306::updatePage(uint8_t pageNumber, uint8_t startColumn) {

    // 10.1.13 Set Page Start Address for Page Addressing Mode (B0h~B7h).
    // This command positions the page start address from 0 to 7 in GDDRAM under Page Addressing Mode.
    // Please refer to Table 9-1 and Section 10.1.3 for details.
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetPageStartAddr) | (pageNumber & 0x07));

    // 10.1.1 Set Lower Column Start Address for Page Addressing Mode (00h~0Fh).
    // This command specifies the lower nibble of the 8-bit column start address for
    // the display data RAM under Page Addressing Mode.
    // The column address will be incremented by each data access.
    // Please refer to Section Table 9-1 and Section 10.1.3 for details.
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetLowColStartAddr) | (startColumn & 0x0F));

    // 10.1.2 Set Higher Column Start Address for Page Addressing Mode (10h~1Fh).
    // This command specifies the higher nibble of the 8-bit column start address for
    // the display data RAM under Page Addressing Mode.
    // The column address will be incremented by each data access.
    // Please refer to Section Table 9-1 and Section 10.1.3 for details.
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetHighColStartAddr) | ((startColumn >> 4) & 0x0F));
    writeData(&displayBuffer[pageNumber][startColumn], width - startColumn);
}

// Write display-buffer to the display
void Ssd1306::update(void) {

    for (uint8_t page = 0; page < pagesNum; page++) {

        updatePage(page, 0);
    }
}

void Ssd1306::setOnOff(uint8_t value) {

    writeCommand(static_cast<uint8_t>(value ? DisplayCmd::SetDispOn : DisplayCmd::SetDispOff));
}

void Ssd1306::setContrast(uint8_t value) {

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetContrastCtrl));
    writeCommand(value);
}

void Ssd1306::setInversion(uint8_t value) {

    writeCommand(static_cast<uint8_t>(value ? DisplayCmd::SetInverseDisp : DisplayCmd::SetNormalDisp));
}

void Ssd1306::initialization(void) {

    // Wait for the screen to boot
    wait.ms(100);

    // Depends of lines number - 64 usually
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetMultiplexRatio));
    writeCommand(0x3F);

    // No offset
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetDispOffset));
    writeCommand(0x00);

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetDispStartLine) | 0);

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetSegmentRemapOn));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetCOMOutScanDirRemap));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetCOMPinsHWConf));
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetCOMSeq) | static_cast<uint8_t>(DisplayCmd::SetCOMNoRemap));

    // setInversion(0);
    // setContrast(0x3F);

    writeCommand(static_cast<uint8_t>(DisplayCmd::EntireDispResume));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetDispClckDiv));
    writeCommand(0x80);

    writeCommand(static_cast<uint8_t>(DisplayCmd::ChargePumpSet));
    writeCommand(static_cast<uint8_t>(DisplayCmd::EnableChargePump));

    // GDDRAM access mode
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetMemAdrMode));
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetMemAdrModePage));

    setOnOff(1);

    // Clear screen
    fill(0);

    // Flush buffer to screen
    update();
}

// Set one pixel in the display-buffer
// x        => horizontal
// y        => vertical
// pixelVal => Pixel color
void Ssd1306::drawPixel(uint8_t x, uint8_t y, uint8_t pixelVal)
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

void Ssd1306::scroll() {

    writeCommand(0x29);
    writeCommand(0x00);
    writeCommand(0x00);
    writeCommand(0x00);
    writeCommand(0x07);
    writeCommand(0x01);
    writeCommand(0x2F);
}

// Draw 1 char to the display-buffer
// ch       => char symbol to display
void Ssd1306::putc(char symbol) {

    IPixelFont& font = fontDefault;
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
            drawPixel(x, y, pixel);
        }
    }

}

// Write string to display-buffer
void Ssd1306::puts(const char * str) {

    // Write until null-byte
    while (*str) {

        putc(*str);
        str++;
    }
}

// Position the cursor
void Ssd1306::cursorGoTo(uint8_t x, uint8_t y) {

    cursorX = (x >= width) ? 0 : x;
    cursorY = (y >= height) ? 0 : y;
}


