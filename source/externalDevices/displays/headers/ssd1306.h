#ifndef _LCD_SSD1306_H
#define _LCD_SSD1306_H

#include <IWriter.h>
#include <IDelayer.h>
#include <IPixelFont.h>

#include <i2c.h>

/*
API for LCD character displays
Supports connection trought I2C.

SSD1306 is a single-chip CMOS OLED/PLED driver with controller for
organic / polymer light emitting diode dot-matrix graphic display system.
It consists of 128 segments and 64 commons.
This IC is designed for Common Cathode type OLED panel.

I2C parameters:
    “SA0” bit provides an extension bit for the slave address.
    Either “0111100” or “0111101”, can be selected as the slave address of SSD1306.
    D/C# pin acts as SA0 for slave address selection.
    tcycle Clock Cycle Time 2.5 us which equal to 400kHz
    tIDLE Idle Time before a new transmission can start 1.3 us
*/
class Ssd1306 : public IWriter {

    friend int main(void);

public:
    
    void puts(const char* str);
    void putc(char symb);

    void drawPixel(uint8_t x, uint8_t y, uint8_t pixelVal);
    void fill(uint8_t bit);
    void update(void);
    void updatePage(uint8_t pageNumber, uint8_t startColumn);
    void cursorGoTo(uint8_t x, uint8_t y);

    void scroll(void);

    void setOnOff(uint8_t value);
    void setContrast(uint8_t value);
    void setInversion(uint8_t value);

    // delete copy constructor and assignment operator
    Ssd1306(const Ssd1306&) = delete;
    Ssd1306& operator=(const Ssd1306&) = delete;

private:

    Ssd1306(I2c& i2c, IDelayer& delay, IPixelFont& defaultFont);

    I2c& i2c;
    IDelayer& wait;
    IPixelFont& fontDefault;

    static const uint8_t width = 128;
    static const uint8_t height = 64;
    static const uint8_t pagesNum = height / 8;

    uint8_t displayBuffer[pagesNum][width];
    uint8_t cursorX = 0;
    uint8_t cursorY = 0;

    void initialization(void);
    void writeCommand(uint8_t byte);
    void writeData(const uint8_t * buffer, uint16_t size);

    enum class DisplayCmd : unsigned char {

        // 1. Fundamental Command Table
        SetContrastCtrl = 0x81,                 // Set Contrast Control. Double byte command to select 1 out of 256 contrast steps.
        // next follows byte of value 0-FF      // Contrast increases as the value increases. (RESET = 7Fh)

        EntireDispResume = 0xA4,                // Entire Display ON. A4h, Resume to RAM content display (RESET). Output follows RAM content
        EntireDispOn = 0xA5,                    // Entire Display ON. A5h, Entire display ON. Output ignores RAM content
        
        SetNormalDisp = 0xA6,                   // Set Normal/Inverse Display. A6h Normal display (RESET). 0 in RAM: OFF in display panel. 1 in RAM: ON in display panel
        SetInverseDisp = 0xA7,                  // Set Normal/Inverse Display. A7h Inverse display. 0 in RAM: ON in display panel. 1 in RAM: OFF in display panel
        
        SetDispOff = 0xAE,                      // Set Display ON/OFF. AEh Display OFF (sleep mode) (RESET)
        SetDispOn = 0xAF,                       // Set Display ON/OFF. AFh Display ON in normal mode 

        // 2. Scrolling Command Table

        // 3. Addressing Setting Command Table
        SetLowColStartAddr = 0x00,              // Set Lower Column Start Address for Page Addressing Mode
        // use with '|x', where 'x' - value to set
                                                // Set the lower nibble of the column start address register for Page Addressing Mode using X[3:0] as data bits.
                                                // The initial display line register is reset to 0000b after RESET.
                                                // Note (1) This command is only for page addressing mode

        SetHighColStartAddr = 0x10,             // Set Higher Column Start Address for Page Addressing Mode
        // use with '|x', where 'x' - value to set
                                                // Set the higher nibble of the column start address registerfor Page Addressing Mode using X[3:0] as data bits.
                                                // The initial display line register is reset to 0000b after RESET.
                                                // Note (1) This command is only for page addressing mode

        SetMemAdrMode = 0x20,                   // Set Memory Addressing Mode.
        // next follows one of
        SetMemAdrModeHoriz = 0x00,              // Set Memory Addressing Mode. 00b, Horizontal Addressing Mode
        SetMemAdrModeVert = 0x01,               // Set Memory Addressing Mode. 01b, Vertical Addressing Mode
        SetMemAdrModePage = 0x02,               // Set Memory Addressing Mode. 10b, Page Addressing Mode (RESET)
        SetMemAdrModeInvalid = 0x03,            // Set Memory Addressing Mode. 11b, Invalid

        SetColAddr = 0x21,                      // Set Column Address Setup column start and end address.
        // start byte                           // Column start address, range : 0-127d, (RESET=0d) 
        // end byte                             // Column end address, range : 0-127d,   (RESET =127d)
                                                // Note (1) This command is only for horizontal or vertical addressing mode.

        SetPageAddr = 0x22,                     // Set Page Address Setup page start and end address 
        // start byte                           // Page start Address, range : 0-7d, (RESET = 0d)
        // end byte                             // Page end Address, range : 0-7d, (RESET = 7d)
                                                // Note (1) This command is only for horizontal or vertical addressing mode.

        SetPageStartAddr = 0xB0,                // Set Page Start Address for Page Addressing Mode
        // use with '|x', where 'x' - value to set
                                                // Set GDDRAM Page Start Address (PAGE0~PAGE7) for Page Addressing Mode using X[2:0].
                                                // Note (1) This command is only for page addressing mode

        // 4. Hardware Configuration (Panel resolution & layout related) Command Table
        SetDispStartLine = 0x40,                // Set Display Start Line.
        // use with '|x', where 'x' - value to set
                                                // Set display RAM display start line register from 0-63 using X5X3X2X1X0.
                                                // Display start line register is reset to 000000b during RESET.

        SetSegmentRemapOff = 0xA0,              // Set Segment Re-map. A0h, column address 0 is mapped to SEG0 (RESET)
        SetSegmentRemapOn = 0xA1,               // Set Segment Re-map. A1h, column address 127 is mapped to SEG0

        SetMultiplexRatio = 0xA8,               // Set Multiplex Ratio. Set MUX ratio to N+1 MUX N=A[5:0]
        // next follows byte of value 0F-3F     // from 16MUX to 64MUX, RESET= 111111b (i.e. 63d, 64MUX) A[5:0] from 0 to 14 are invalid entry.

        SetCOMOutScanDirNorm = 0xC0,            // Set COM Output Scan Direction. C0h, normal mode (RESET) Scan from COM0 to COM[N –1]
        SetCOMOutScanDirRemap = 0xC8,           // Set COM Output Scan Direction. C8h, remapped mode. Scan from COM[N-1] to COM0 Where N is the Multiplex ratio.

        SetDispOffset = 0xD3,                   // Set Display Offset. Set vertical shift by COM from 0d~63d
        // next follows byte of value 0-3F      // The value is reset to 00h after RESET.

        SetCOMPinsHWConf = 0xDA,                // Set COM Pins Hardware Configuration
        // next follows 'x|y', where 'x', 'y' - values to set
        SetCOMNoRemap = 0x02,                   // A[5]=0b(RESET), Disable COM Left/Right remap
        SetCOMLRRemap = 0x22,                   // A[5]=1b, Enable COM Left/Right remap
        SetCOMSeq = 0x02,                       // A[4]=0b, Sequential COM pin configuration
        SetCOMAlt = 0x12,                       // A[4]=1b(RESET), Alternative COM pin configuration

        // 5. Timing & Driving Scheme Setting Command Table
        SetDispClckDiv = 0xD5,                  // Set  Display Clock Divide Ratio/Oscillator Frequency
        // next follows next follows 'x|y', where 'x', 'y' - values to set
        // x = [0x00 - 0x0F]                    // A[3:0] : Define the divide ratio (D) of the display clocks (DCLK):
                                                // Divide ratio= A[3:0] + 1, RESET is 0000b (divide ratio = 1)
        // y = [0x00 - 0x0F] << 4               // A[7:4] : Set the Oscillator Frequency, FOSC.
                                                // Oscillator Frequency increases with the value of A[7:4] and vice versa.
                                                // RESET is 1000b Range:0000b~1111b. Frequency increases as setting value increases.

        SetPrechargePer = 0xD9,                 // Set Pre-charge Period.
        // next follows 'x|y', where 'x', 'y' - values to set
        // x = [0x00 - 0x0F]                    // A[3:0] : Phase 1 period of up to 15 DCLK clocks 0 is invalid entry (RESET=2h)
        // y = [0x00 - 0x0F] << 4               // A[7:4] : Phase 2 period of up to 15 DCLK clocks 0 is invalid entry (RESET=2h )

        SetVcomhDeselectLev = 0xDB,             // Set VCOMH Deselect Level
        // next follows one of                  // A[6:4] Hex code V COMH deselect level
        SetVcomh065Vcc = 0x00,                  // 000b 00h ~ 0.65 x VCC
        SetVcomh077Vcc = 0x20,                  // 010b 20h ~ 0.77 x VCC (RESET)
        SetVcomh083Vcc = 0x30,                  // 011b 30h ~ 0.83 x VCC

        NOP = 0xE3,                             // NOP: Command for no operation


        // Charge Pump Command Table
        ChargePumpSet = 0x8D,                   // Charge Pump Setting
        DisableChargePump = 0x10,               // A[2] = 0b, Disable charge pump(RESET)
        EnableChargePump = 0x14,                // A[2] = 1b, Enable charge pump during display on
                                                // Note (1) The Charge Pump must be enabled by the following command:
                                                // 8Dh - Charge Pump Setting, 14h - Enable Charge Pump, AFh - Display ON
    };
};

#endif