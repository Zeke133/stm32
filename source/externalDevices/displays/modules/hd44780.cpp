#include <hd44780.h>

Hd44780::Hd44780(I2c& i2c, IDelayer& delay, uint8_t lines, uint8_t columns, uint8_t address)
    :   i2c(i2c),
        wait(delay),
        address(address),
        width(columns) {

    functionSetReg = 0;
    // DataLength 4 bit - FunctionSetBits::DataLenghtLong = 0
    // Font 1 - 5x10 dots, 0 - 5x8 dots - FunctionSetBits::BigFont = 0
    if (lines <= 1) {
        functionSetReg |= static_cast<uint8_t>(FunctionSetBits::BigFont);
        height = 1;
    } else {
        functionSetReg |= static_cast<uint8_t>(FunctionSetBits::DoubleLines);
        height = 2;
    }

    displayCtrlReg = 0;
    displayCtrlReg |= static_cast<uint8_t>(DisplayCtrlBits::DisplayOnBit);
    // CursorOnBit = off,
    // BlinkOnBit = off,

    entryModeReg = 0;
    entryModeReg |= static_cast<uint8_t>(EntryModeBits::IncrementDecrBit);
    // address++
    // ShiftDisplayBit = off

    //------------------------------------------------------------------------------
    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
    wait.ms(50);

    // Now we pull both RS and R/W low to begin commands
    i2c.startTransmit(address);
    i2c.write(0);
    i2c.stopTransmit();
    // reset expander and turn backlight off (Bit 8 = 1)
    wait.ms(100);

    // put the LCD into 4 bit mode
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x30);
    wait.ms(5);         // wait min 4.1ms

    // write4bits(0x30);
    // wait.ms(5);         // wait min 4.1ms

    write4bits(0x30);
    wait.us(150);

    // finally, set to 4-bit interface
    write4bits(0x20);

    // set # lines, font size, etc.
    sendInstruction(static_cast<uint8_t>(InstructionsBits::FunctionSet) | functionSetReg);

    // turn the display on with no cursor or blinking default
    sendInstruction(static_cast<uint8_t>(InstructionsBits::DisplayControl) | displayCtrlReg);

    // clear display, set cursor position to zero
    clear();

    // set the entry mode
    sendInstruction(static_cast<uint8_t>(InstructionsBits::EntryModeSet) | entryModeReg);

    // set cursor position to zero
    home();
}

void Hd44780::write4bits(uint8_t data) {

    if (backlightPin) {
        data |= static_cast<uint8_t>(CmdBusBits::BackLight);    // Baclight enable
    }

    i2c.startTransmit(address);

    // set port data
    i2c.write(data);

    // pulse EN signal
    i2c.write(data | static_cast<uint8_t>(CmdBusBits::En));     // En high
    // enable pulse must be >450ns
    wait.us(1);

    i2c.write(data & ~static_cast<uint8_t>(CmdBusBits::En));    // En low
    // commands need >37us to settle
    wait.us(50);

    i2c.stopTransmit();
}

inline void Hd44780::writeData(uint8_t value, uint8_t mode) {

    write4bits((value & 0xf0) | mode);
    write4bits(((value << 4) & 0xf0) | mode);
}

inline void Hd44780::sendInstruction(uint8_t value) {

    writeData(value, 0/*CmdBusBits::Rs*/);
}

inline void Hd44780::sendData(uint8_t value) {

    writeData(value, static_cast<uint8_t>(CmdBusBits::Rs));
}

void Hd44780::clear() {

    sendInstruction(static_cast<uint8_t>(InstructionsBits::ClearDisplay));
    wait.ms(2);
}

void Hd44780::home() {

    sendInstruction(static_cast<uint8_t>(InstructionsBits::ReturnHome));
    // 1.52 ms
    wait.ms(2);
}

void Hd44780::setCursor(uint8_t row, uint8_t column) {

    row = (row < height) ? row : (height - 1);
    column = (column < width) ? column : (width - 1);
    sendInstruction(static_cast<uint8_t>(InstructionsBits::SetDDRAMADDR) | (column + (row ? 0x40 : 0x00)));
}

void Hd44780::setDisplayScreen(uint8_t mode /*1 - on, 0 - off*/) {

    if (mode) {
        displayCtrlReg |= static_cast<uint8_t>(DisplayCtrlBits::DisplayOnBit);
    } else {
        displayCtrlReg &= ~static_cast<uint8_t>(DisplayCtrlBits::DisplayOnBit);
    }
    
    sendInstruction(static_cast<uint8_t>(InstructionsBits::DisplayControl) | displayCtrlReg);
}

void Hd44780::setCursorShow(uint8_t mode /*1 - on, 0 - off*/) {

    if (mode) {
        displayCtrlReg |= static_cast<uint8_t>(DisplayCtrlBits::CursorOnBit);
    } else {
        displayCtrlReg &= ~static_cast<uint8_t>(DisplayCtrlBits::CursorOnBit);
    }
    
    sendInstruction(static_cast<uint8_t>(InstructionsBits::DisplayControl) | displayCtrlReg);
}

void Hd44780::setCursorBlink(uint8_t mode /*1 - on, 0 - off*/) {

    if (mode) {
        displayCtrlReg |= static_cast<uint8_t>(DisplayCtrlBits::BlinkOnBit);
    } else {
        displayCtrlReg &= ~static_cast<uint8_t>(DisplayCtrlBits::BlinkOnBit);
    }

    sendInstruction(static_cast<uint8_t>(InstructionsBits::DisplayControl) | displayCtrlReg);
}

void Hd44780::setScrollMode(uint8_t screenShift /*0 cursor or 1 screen*/, uint8_t rightLeft /* R - 1 or L - 0*/) {

    uint8_t mode = 0;

    if (rightLeft) {
        mode |= static_cast<uint8_t>(CursorShiftBits::RightLeft);
    }

    if (screenShift) {
        mode |= static_cast<uint8_t>(CursorShiftBits::DisplayCursor);
    }

    sendInstruction(static_cast<uint8_t>(InstructionsBits::CursorShift) | mode);
}

void Hd44780::setTextFlow(uint8_t modeLR /*Left - 0, Right - 1*/) {

    if (modeLR) {
        entryModeReg |= static_cast<uint8_t>(EntryModeBits::IncrementDecrBit);
    } else {
        entryModeReg &= ~static_cast<uint8_t>(EntryModeBits::IncrementDecrBit);
    }

    sendInstruction(static_cast<uint8_t>(InstructionsBits::EntryModeSet) | entryModeReg);
}

void Hd44780::setScrollAuto(uint8_t mode /*1 - on, 0 - off*/) {

    if (mode) {
        entryModeReg |= static_cast<uint8_t>(EntryModeBits::ShiftDisplayBit);
    } else {
        entryModeReg &= ~static_cast<uint8_t>(EntryModeBits::ShiftDisplayBit);
    }

    sendInstruction(static_cast<uint8_t>(InstructionsBits::EntryModeSet) | entryModeReg);
}

// Turn the (optional) backlight off/on
void Hd44780::setBacklight(uint8_t mode /*1 - on, 0 - off*/) {

    backlightPin = mode ? 1 : 0;

    i2c.startTransmit(address);
    i2c.write(backlightPin ? static_cast<uint8_t>(CmdBusBits::BackLight) : 0);
    i2c.stopTransmit();
}

// Allows us to fill the first 8 CGRAM locations with custom characters
void Hd44780::loadCustomSymbol(uint8_t adr, uint8_t * matrix /*5x8*/) {

    adr &= 0x07; // we only have 8 locations 0-7
    sendInstruction(static_cast<uint8_t>(InstructionsBits::SetCGRAMADDR) | (adr << 3));   // why <<3?
    for (int i = 0; i < 8; i++) {
        sendData(matrix[i]);
    }
}

void Hd44780::puts(const char* str) {

    while(*str) {
        sendData(*str++);
    }
}

void Hd44780::putc(char symb) {

    sendData(symb);
}

uint8_t Hd44780::getWidth(void) const {

    return width;
}

uint8_t Hd44780::getHeight(void) const {

    return height;
}

void Hd44780::clearRow(uint8_t row) {

    setCursor(row, 0);

    for(uint8_t symbol = 0; symbol < width; symbol++) {
        sendData(' ');
    }
}
