#include <ssd1306.h>

Ssd1306::Ssd1306(I2c& i2c, IDelayer& delayer, uint8_t address)
    :   i2c(i2c),
        delayer(delayer),
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

void Ssd1306::writeCommand(const uint8_t * data, uint16_t size) {

    uint8_t output[10];     // max size of command must fit
    uint8_t outCnt = 0, inCnt = 0;

    // Create package to send
    while (inCnt < size && outCnt < sizeof output) {

        // Control bit to 1 and D/C# to 0: means command will be transmited
        output[outCnt++] = 0x80;
        // User data bytes
        output[outCnt++] = data[inCnt++];
    }

    i2c.send(address, output, outCnt);
}

// Fill the whole screen with the given bit, color depends of display settings
void Ssd1306::fill(uint8_t bit) {

    uint8_t filler = (bit) ? 0xFF : 0x00;

    uint8_t * displayBufferEnd = (uint8_t *)displayBuffer + pagesNum * width;
    uint8_t * displayBufferPtr = (uint8_t *)displayBuffer;

    while (displayBufferPtr != displayBufferEnd) {
        *displayBufferPtr++ = filler;
    }
}

// Flush display-buffer to GDDRAM
void Ssd1306::update(void) {

    uint8_t cmdBuf[3];

    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::SetColAddr);
    cmdBuf[1] = 0;     // Column start address
    cmdBuf[2] = 127;   // Column end address
    writeCommand(cmdBuf, sizeof cmdBuf);

    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::SetPageAddr);
    cmdBuf[1] = 0;     // Page start address
    cmdBuf[2] = 7;     // Page end address
    writeCommand(cmdBuf, sizeof cmdBuf);

    // Control bit to 0 and D/C# to 0: means command followed with data will be transmited
    writeDataCmd = 0x40;
    // located in memory right before "displayBuffer" for optimisation
    i2c.sendBufferized(address, &writeDataCmd, width*pagesNum + 1);
}

void Ssd1306::setOnOff(uint8_t value) {

    uint8_t cmd = static_cast<uint8_t>(value ? DisplayCmd::SetDispOn : DisplayCmd::SetDispOff);

    writeCommand(&cmd);
}

void Ssd1306::setContrast(uint8_t value) {

    uint8_t cmdBuf[2];

    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::SetContrastCtrl);
    cmdBuf[1] = value;

    writeCommand(cmdBuf, sizeof cmdBuf);
}

void Ssd1306::setInversion(uint8_t value) {

    uint8_t cmd = static_cast<uint8_t>(value ? DisplayCmd::SetInverseDisp : DisplayCmd::SetNormalDisp);

    writeCommand(&cmd);
}

void Ssd1306::initialization(void) {

    // Wait for the screen to boot
    delayer.ms(10/*100*/);

    uint8_t cmdBuf[2];

    // Depends of lines number - 64 usually
    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::SetMultiplexRatio);
    cmdBuf[1] = 0x3F;
    writeCommand(cmdBuf, sizeof cmdBuf);

    // No offset
    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::SetDispOffset);
    cmdBuf[1] = 0x00;
    writeCommand(cmdBuf, sizeof cmdBuf);

    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::SetDispStartLine) | 0;
    writeCommand(cmdBuf);

    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::SetSegmentRemapOn);
    writeCommand(cmdBuf);

    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::SetCOMOutScanDirRemap);
    writeCommand(cmdBuf);

    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::SetCOMPinsHWConf);
    cmdBuf[1] = static_cast<uint8_t>(DisplayCmd::SetCOMAlt) | static_cast<uint8_t>(DisplayCmd::SetCOMNoRemap);
    writeCommand(cmdBuf, sizeof cmdBuf);

    // default values
    // setInversion(0);
    // setContrast(0x3F);

    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::EntireDispResume);
    writeCommand(cmdBuf);

    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::SetDispClckDiv);
    cmdBuf[1] = 0x80;
    writeCommand(cmdBuf, sizeof cmdBuf);

    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::ChargePumpSet);
    cmdBuf[1] = static_cast<uint8_t>(DisplayCmd::EnableChargePump);
    writeCommand(cmdBuf, sizeof cmdBuf);

    // GDDRAM access mode
    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::SetMemAdrMode);
    cmdBuf[1] = static_cast<uint8_t>(DisplayCmd::SetMemAdrModeHoriz);
    writeCommand(cmdBuf, sizeof cmdBuf);

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

    uint8_t cmdBuf[7];
    uint8_t i = 0;

    if (allowVertical) {
        cmdBuf[i] = static_cast<uint8_t>(leftRight ? DisplayCmd::SetScrollVerRightHoriz : DisplayCmd::SetScrollVerLeftHoriz);
    } else {
        cmdBuf[i] = static_cast<uint8_t>(leftRight ? DisplayCmd::SetScrollRightHoriz : DisplayCmd::SetScrollLeftHoriz);
    }

    cmdBuf[++i] = 0x00;
    cmdBuf[++i] = startPage & 0x07;
    cmdBuf[++i] = "\x07\x04\x05\x00\x06\x01\x02\x03"[speed & 0x07];   // constants mapping according to datasheet
    cmdBuf[++i] = endPage & 0x07;

    if (allowVertical) {
        cmdBuf[++i] = verticalOffset & 0x3F;
    } else {
        cmdBuf[++i] = 0x00;
        cmdBuf[++i] = 0xFF;
    }

    writeCommand(cmdBuf, i);
}

void Ssd1306::setScrollVerticalArea(uint8_t rowsFixedOnTopNumber, uint8_t rowsScrolledNumber) {

    uint8_t cmdBuf[3];

    cmdBuf[0] = static_cast<uint8_t>(DisplayCmd::SetScrollVertArea);
    cmdBuf[1] = rowsFixedOnTopNumber & 0x3F;    // [reset] = 0
    // here could be a lot of values validations according to reference
    cmdBuf[2] = rowsScrolledNumber & 0x7F;      // [reset] = 64

    writeCommand(cmdBuf, sizeof cmdBuf);
}

void Ssd1306::startScroll(void) {

    uint8_t cmdBuf = static_cast<uint8_t>(DisplayCmd::SetScrollActivate);

    writeCommand(&cmdBuf);
}

void Ssd1306::stopScroll(void) {

    uint8_t cmdBuf = static_cast<uint8_t>(DisplayCmd::SetScrollDeactivate);

    writeCommand(&cmdBuf);
}
