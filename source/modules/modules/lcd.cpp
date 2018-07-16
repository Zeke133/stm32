#include <lcd.h>



Lcd::Lcd(I2c& interfaceHw, IDelayer& delay, uint8_t lines)
    :   i2c(interfaceHw),
        wait(delay) {

    backlightPin = 1;

    functionSetReg = 0;
    if (lines > 1) functionSetReg |= static_cast<uint8_t>(FunctionSetBits::DoubleLines);
    // DataLength 4 bit - FunctionSetBits::DataLenghtLong = 0
    // Font 1 - 5x10 dots, 0 - 5x8 dots - FunctionSetBits::BigFont = 0

	displayCtrlReg = 0;
    displayCtrlReg |= static_cast<uint8_t>(DisplayCtrlBits::DisplayOnBit);
    // CursorOnBit = off,
    // BlinkOnBit = off,

    entryModeReg = 0;
    entryModeReg |= static_cast<uint8_t>(EntryModeBits::IncrementDecrBit);    // increment adr
    // ShiftDisplayBit = off

    //------------------------------------------------------------------------------
	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	wait.ms(50);

	// Now we pull both RS and R/W low to begin commands
    i2c.startTransmit();
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

void Lcd::write4bits(uint8_t data) {

    if (backlightPin) {
        data |= static_cast<uint8_t>(CmdBusBits::BackLight);    // Baclight enable
    }

    i2c.startTransmit();

    // set port data
	i2c.write(data);

    // pulse EN signal
	i2c.write(data | static_cast<uint8_t>(CmdBusBits::En));     // En high
	wait.us(1);                     // enable pulse must be >450ns

	i2c.write(data & ~static_cast<uint8_t>(CmdBusBits::En));    // En low
	wait.us(50);                    // commands need >37us to settle

    i2c.stopTransmit();
}

void Lcd::writeData(uint8_t value, uint8_t mode) {

    write4bits((value & 0xf0) | mode);
	write4bits(((value << 4) & 0xf0) | mode);
}

inline void Lcd::sendInstruction(uint8_t value) {

	writeData(value, 0/*CmdBusBits::Rs*/);
}

inline void Lcd::sendData(uint8_t value) {

	writeData(value, static_cast<uint8_t>(CmdBusBits::Rs));
}

inline void Lcd::clear() {

	sendInstruction(static_cast<uint8_t>(InstructionsBits::ClearDisplay));
	wait.ms(2);
}

inline void Lcd::home() {

	sendInstruction(static_cast<uint8_t>(InstructionsBits::ReturnHome));
	wait.ms(2); // 1.52 ms
}

void Lcd::cursorGoTo(uint8_t row, uint8_t col) {

    sendInstruction(static_cast<uint8_t>(InstructionsBits::SetDDRAMADDR) | (col + (row ? 0x40 : 0x00)));
}

void Lcd::displaySet(uint8_t mode /*1 - on, 0 - off*/) {

    if (mode) {
        displayCtrlReg |= static_cast<uint8_t>(DisplayCtrlBits::DisplayOnBit);
    } else {
        displayCtrlReg &= ~static_cast<uint8_t>(DisplayCtrlBits::DisplayOnBit);
    }
    
    sendInstruction(static_cast<uint8_t>(InstructionsBits::DisplayControl) | displayCtrlReg);
}

void Lcd::cursorSet(uint8_t mode /*1 - on, 0 - off*/) {

    if (mode) {
        displayCtrlReg |= static_cast<uint8_t>(DisplayCtrlBits::CursorOnBit);
    } else {
        displayCtrlReg &= ~static_cast<uint8_t>(DisplayCtrlBits::CursorOnBit);
    }
    
    sendInstruction(static_cast<uint8_t>(InstructionsBits::DisplayControl) | displayCtrlReg);
}

void Lcd::cursorBlinkSet(uint8_t mode /*1 - on, 0 - off*/) {

    if (mode) {
        displayCtrlReg |= static_cast<uint8_t>(DisplayCtrlBits::BlinkOnBit);
    } else {
	    displayCtrlReg &= ~static_cast<uint8_t>(DisplayCtrlBits::BlinkOnBit);
    }

    sendInstruction(static_cast<uint8_t>(InstructionsBits::DisplayControl) | displayCtrlReg);
}

void Lcd::scroll(uint8_t screenShift /*0 cursor or 1 screen*/, uint8_t rightLeft /* R - 1 or L - 0*/) {

    uint8_t mode = 0;

    if (rightLeft) {
        mode |= static_cast<uint8_t>(CursorShiftBits::RightLeft);
    }

    if (screenShift) {
        mode |= static_cast<uint8_t>(CursorShiftBits::DisplayCursor);
    }

    sendInstruction(static_cast<uint8_t>(InstructionsBits::CursorShift) | mode);
}

void Lcd::textFlowSet(uint8_t modeLR /*Left - 0, Right - 1*/) {

    if (modeLR) {
        entryModeReg |= static_cast<uint8_t>(EntryModeBits::IncrementDecrBit);
    } else {
        entryModeReg &= ~static_cast<uint8_t>(EntryModeBits::IncrementDecrBit);
    }
	
    sendInstruction(static_cast<uint8_t>(InstructionsBits::EntryModeSet) | entryModeReg);
}

void Lcd::autoScrollSet(uint8_t mode /*1 - on, 0 - off*/) {

    if (mode) {
        entryModeReg |= static_cast<uint8_t>(EntryModeBits::ShiftDisplayBit);
    } else {
        entryModeReg &= ~static_cast<uint8_t>(EntryModeBits::ShiftDisplayBit);
    }

    sendInstruction(static_cast<uint8_t>(InstructionsBits::EntryModeSet) | entryModeReg);
}

// Allows us to fill the first 8 CGRAM locations with custom characters
void Lcd::loadCustomSymbol(uint8_t adr, uint8_t * matrix /*5x8*/) {
    
	adr &= 0x07; // we only have 8 locations 0-7
	sendInstruction(static_cast<uint8_t>(InstructionsBits::SetCGRAMADDR) | (adr << 3));   // why <<3?
	for (int i = 0; i < 8; i++) {
		sendData(matrix[i]);
	}
}

// Turn the (optional) backlight off/on
void Lcd::backlightSet(uint8_t mode /*1 - on, 0 - off*/) {

    backlightPin = mode ? 1 : 0;
	
    i2c.startTransmit();
	i2c.write(backlightPin ? static_cast<uint8_t>(CmdBusBits::BackLight) : 0);
    i2c.stopTransmit();
}

void Lcd::puts(const char* str) {
    while(*str) {
        sendData(*str++);
    }
}

void Lcd::putc(char symb) {
    sendData(symb);
}



