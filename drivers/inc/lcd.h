#ifndef _LCD_H
#define _LCD_H

#include <stm32f10x.h>

#include <i2c.h>
#include <delay.h>

// Hitachi HD44780 API

// API for LCD character displays
class Lcd {

        friend int main(void);

    public:
        
        void puts(const char* str);
        void putc(char symb);

        void clear(void);
        void home(void);
        void cursorGoTo(uint8_t col, uint8_t row);

        void loadCustomSymbol(uint8_t adr, uint8_t * matrix /*5x8*/);

        void displaySet(uint8_t mode /*1 - on, 0 - off*/);
        void cursorSet(uint8_t mode /*1 - on, 0 - off*/);
        void cursorBlinkSet(uint8_t mode /*1 - on, 0 - off*/);
        void scroll(uint8_t screenShift /*0 cursor or 1 screen*/, uint8_t rightLeft /* R - 1 or L - 0*/);
        void textFlowSet(uint8_t modeLR /*Left - 0, Right - 1*/);
        void autoScrollSet(uint8_t mode /*1 - on, 0 - off*/);

        // hardware dependent feature!
        void backlightSet(uint8_t mode /*1 - on, 0 - off*/);

    private:

        Lcd(I2c& interface, Delay& delay, uint8_t lines);

        void write4bits(uint8_t data);
        void writeData(uint8_t value, uint8_t mode);
        void sendInstruction(uint8_t value);
        void sendData(uint8_t value);

        const I2c& interface;
        const Delay& wait;

        uint8_t backlightPin;

        uint8_t functionSetReg;
        uint8_t displayCtrlReg;
        uint8_t entryModeReg;

        // Instructions
        enum class InstructionsBits : uint8_t {

            ClearDisplay = 0x01,
            ReturnHome = 0x02,
            EntryModeSet = 0x04,
            DisplayControl = 0x08,
            CursorShift = 0x10,
            FunctionSet = 0x20,
            SetCGRAMADDR = 0x40,
            SetDDRAMADDR = 0x80
        };

        // Entry Mode Set
        enum class EntryModeBits : uint8_t {

            ShiftDisplayBit = 0x01,     // 1 - display shifts, 0 - off
            IncrementDecrBit = 0x02,    // Increments 1 or decrements 0
        };

        // Display On/Off Control
        enum class DisplayCtrlBits : uint8_t {

            DisplayOnBit = 0x04,
            CursorOnBit = 0x02,
            BlinkOnBit = 0x01,
        };

        // Cursor or Display Shift
        enum class CursorShiftBits : uint8_t {

            DisplayCursor = 0x08,       // 1 - display shift, 0 - cursor shift
            RightLeft = 0x04,           // 1 - right, 0 - left
        };

        // Function Set
        enum class FunctionSetBits : uint8_t {

            // Sets the interface data length. Data is sent or received in 8-bit lengths (DB7 to DB0) when DL is 1,
            // and  in  4-bit  lengths  (DB7  to  DB4)  when  DL  is  0.When  4-bit  length  is  selected,  data  must  be  sent  or
            // received twice.

            DataLenghtLong = 0x10,      // 1 - 8 bit, 0 - 4 bit
            DoubleLines = 0x08,         // 1 - 2 lines, 0 - 1 line
            BigFont = 0x04,             // 1 - 5x10 dots, 0 - 5x8 dots
        };

        enum class CmdBusBits : uint8_t {

            BackLight = 0x08,           // Enable backlight, if using I2C to ParallelAdaptor it's possible to control
            
            En = 0x04,                  // Enable bit
            Rw = 0x02,                  // Read/Write bit
            Rs = 0x01,                  // Register select bit
        };

};

// Symbols examples
// uint8_t bell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
// uint8_t note[8]  = {0x2,0x3,0x2,0xe,0x1e,0xc,0x0};
// uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
// uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
// uint8_t duck[8]  = {0x0,0xc,0x1d,0xf,0xf,0x6,0x0};
// uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
// uint8_t cross[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
// uint8_t retarrow[8] = {0x1,0x1,0x5,0x9,0x1f,0x8,0x4};


#endif
