#include <ssd1306.h>

Ssd1306::Ssd1306(I2c& i2c, IDelayer& delay, uint8_t address)
    :   i2c(i2c),
        wait(delay),
        address(address) {

    initialization();
}

/*
Description of service byte format:
| b7 |  b6  | b5 | b4 | b3 | b2 | b1 | b0 |
| Co | D/C# | 0  | 0  | 0  | 0  | 0  | 0  |
If the Co bit is set as logic “0”, the transmission of the following information will contain data bytes only.
The D/C# bit determines the next data byte is acted as a command or a data.
If the D/C# bit is set to logic “0”, it defines the following data byte as a command.
If the D/C# bit is set to logic “1”, it defines the following data byte as a data which will be stored at the GDDRAM.
The GDDRAM column address pointer will be increased by one automatically after each data write.
*/

void Ssd1306::writeCommand(uint8_t byte) {

    i2c.startTransmit(address);
    // Control bit to 1 and D/C# to 0: means command will be transmited
    i2c.write(0x80);
    i2c.write(byte);
    i2c.stopTransmit();
}

void Ssd1306::writeData(const uint8_t * buffer, uint16_t size) {

    i2c.startTransmit(address);
    // Control bit to 0 and D/C# to 0: means command followed with data will be transmited
    i2c.write(0x40);
    uint8_t * end = (uint8_t*)buffer + size;
    while (buffer < end) {
        i2c.write(*buffer++);
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

// Flush page of display-buffer to GDDRAM
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

// Write display-buffer to the GDDRAM
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
    writeCommand(static_cast<uint8_t>(DisplayCmd::SetCOMAlt) | static_cast<uint8_t>(DisplayCmd::SetCOMNoRemap));

    // default values
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
void Ssd1306::drawPixel(uint8_t x, uint8_t y, uint8_t pixelVal) {
    
    if (x >= width || y >= height) {
        // Don't write outside the buffer
        return;
    }

    if (pixelVal) {
        displayBuffer[y / 8][x] |= 1 << (y % 8);
    } else {
        displayBuffer[y / 8][x] &= ~(1 << (y % 8));
    }
}

void Ssd1306::setScroll(uint8_t leftRight,
                        uint8_t allowVertical,
                        uint8_t speed,
                        uint8_t startPage,
                        uint8_t endPage,
                        uint8_t verticalOffset) {

    if (allowVertical) {
        writeCommand(static_cast<uint8_t>(leftRight ? DisplayCmd::SetScrollVerRightHoriz : DisplayCmd::SetScrollVerLeftHoriz));
    } else {
        writeCommand(static_cast<uint8_t>(leftRight ? DisplayCmd::SetScrollRightHoriz : DisplayCmd::SetScrollLeftHoriz));
    }

    writeCommand(0x00);
    writeCommand(startPage & 0x07);
    writeCommand("\x07\x04\x05\x00\x06\x01\x02\x03"[speed & 0x07]);
    writeCommand(endPage & 0x07);

    if (allowVertical) {
        writeCommand(verticalOffset & 0x3F);
    } else {
        writeCommand(0x00);
        writeCommand(0xFF);
    }
}

void Ssd1306::setScrollVerticalArea(uint8_t rowsFixedOnTopNumber, uint8_t rowsScrolledNumber) {

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetScrollVertArea));
    writeCommand(rowsFixedOnTopNumber & 0x3F);  // [reset] = 0
    // here could be a lot of values validations according to reference
    writeCommand(rowsScrolledNumber & 0x7F);    // [reset] = 64
}

void Ssd1306::startScroll(void) {

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetScrollActivate));
}

void Ssd1306::stopScroll(void) {

    writeCommand(static_cast<uint8_t>(DisplayCmd::SetScrollDeactivate));
}
