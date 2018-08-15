#ifndef _LCD_SSD1306_H
#define _LCD_SSD1306_H

#include <IWriter.h>
#include <IDelayer.h>

#include <i2c.h>

/*
API for LCD character displays
Supports connection trought I2C.

SSD1306 is a single-chip CMOS OLED/PLED driver with controller for
organic / polymer light emitting diode dot-matrix graphic display system.
It consists of 128 segments and 64 commons.
This IC is designed for Common Cathode type OLED panel.

I2C address default = 0x78
*/
class Lcd_ssd1306 : public IWriter {

        friend int main(void);

    public:
        
        void puts(const char* str);
        void putc(char symb);

        

        // delete copy constructor and assignment operator
        Lcd_ssd1306(const Lcd_ssd1306&) = delete;
        Lcd_ssd1306& operator=(const Lcd_ssd1306&) = delete;

    private:

        Lcd_ssd1306(I2c& i2c, IDelayer& delay);

        void writeCommand(char byte);

        void init();

        void fill();
        void update();

        void drawPixel();
        void cursorGoTo();

        I2c& i2c;
        IDelayer& wait;

};

#endif
