#include <lcd_ssd1306.h>

Lcd_ssd1306::Lcd_ssd1306(I2c& i2c, IDelayer& delay, IPixelFont& defaultFont)
    :   i2c(i2c),
        wait(delay),
        fontDefault(defaultFont) {

    initialization();
}

void Lcd_ssd1306::writeCommand(uint8_t byte) {

    i2c.startTransmit();
    wait.us(2); //!!!
    i2c.write(0x80);    // Control bit to 1 and D/C# to 0 - means command will be transmited
    wait.us(2);
    i2c.write(byte);
    wait.us(2);
    i2c.stopTransmit();
    wait.us(2);
}

void Lcd_ssd1306::writeData(const uint8_t * buffer, uint16_t size) {

    i2c.startTransmit();
    wait.us(2);//!!!
    // If the Co bit is set as logic “0”, the transmission of the following information will contain data bytes only.
    // The D/C# bit determines the next data byte is acted as a command or a data.
    // If the D/C# bit is set to logic “0”, it defines the following data byte as a command.
    // If the D/C# bit is set to logic “1”, it defines the following data byte as a data which will be stored at the GDDRAM.
    // The GDDRAM column address pointer will be increased by one automatically after each data write.
    i2c.write(0x40);    // Control bit to 0 and D/C# to 0 - means command will be transmited
    wait.us(2);

    for(uint16_t i = 0; i < size; i++) {

        i2c.write(buffer[i]);
        wait.us(2);
    }

    i2c.stopTransmit();
    wait.us(2);
}

void Lcd_ssd1306::initialization(void) {

    // Wait for the screen to boot
    wait.ms(100);

    // Init LCD
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetMultiplexRatio));
    writeCommand(0x3F);

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetDispOffset));
    writeCommand(0x00);

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetDispStartLine) | 0);

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetSegmentRemapOn));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetCOMOutScanDirRemap));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetCOMPinsHWConf));
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetCOMSeq) | static_cast<uint8_t>(DisplayCmd::SetCOMNoRemap));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetContrastCtrl));
    writeCommand(0x7F);

    writeCommand(static_cast<uint8_t>(DisplayCmd::EntireDispResume));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetNormalDisp));

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetDispClckDiv));
    writeCommand(0x80);

    writeCommand(static_cast<uint8_t>(DisplayCmd::ChargePumpSet));
    writeCommand(static_cast<uint8_t>(DisplayCmd::EnableChargePump));

    //
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetMemAdrMode));
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetMemAdrModePage));

    //
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetDispOn));

    // Clear screen
    fill(1);

    // Flush buffer to screen
    update();
}

// Fill the whole screen with the given bit, color depends of display settings
void Lcd_ssd1306::fill(uint8_t bit) {

    uint8_t filler = (bit) ? 0xFF : 0x00;

    for(int page = 0; page < pagesNum; page++) {

        for(int segment = 0; segment < width; segment++) {

            displayBuffer[page][segment] = filler;
        }
    }
}

// Write display-buffer to the LCD
void Lcd_ssd1306::update(void) {

    //!!!! update page x width from - to

    for (uint8_t page = 0; page < pagesNum; page++) {

        // 10.1.13 Set Page Start Address for Page Addressing Mode (B0h~B7h).
        // This command positions the page start address from 0 to 7 in GDDRAM under Page Addressing Mode.
        // Please refer to Table 9-1 and Section 10.1.3 for details.
        writeCommand(static_cast<uint8_t>(DisplayCmd::SetPageStartAddr) | (page & 0x07));

        // 10.1.1 Set Lower Column Start Address for Page Addressing Mode (00h~0Fh).
        // This command specifies the lower nibble of the 8-bit column start address for
        // the display data RAM under Page Addressing Mode.
        // The column address will be incremented by each data access.
        // Please refer to Section Table 9-1 and Section 10.1.3 for details.
        writeCommand(static_cast<uint8_t>(DisplayCmd::SetLowColStartAddr) | 0);

        // 10.1.2 Set Higher Column Start Address for Page Addressing Mode (10h~1Fh).
        // This command specifies the higher nibble of the 8-bit column start address for
        // the display data RAM under Page Addressing Mode.
        // The column address will be incremented by each data access.
        // Please refer to Section Table 9-1 and Section 10.1.3 for details.
        writeCommand(static_cast<uint8_t>(DisplayCmd::SetHighColStartAddr) | 0);

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


