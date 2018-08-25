#include <usart.h>
#include <led.h>
#include <hd44780.h>
#include <rtc.h>
#include <ds18b20.h>

#include <convertation.h>
#include <strings.h>


void LedTestInterfaceToUsart(Usart& usart, LED& led) {
        
    usart.puts("\r\n Check for LED: ");

    if (usart.getCount() > 0) {        
        
        if (compare("led.on", usart.getData())) led.on();
        else if (compare("led.off", usart.getData())) led.off();
        else if (compare("led.invert", usart.getData())) led.invert();
        else usart.puts(" unkn cmd!!!");
    }
    else {

        usart.puts(" no cmd");
    }
}

void LcdTestInterfaceToUsart(Usart& usart, Lcd& lcd) {
        
    usart.puts("\r\n Check for LCD: ");

    if (usart.getCount() > 0) {

        uint8_t bell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
                
        if (compare("lcd.clear", usart.getData())) lcd.clear();
        else if (compare("lcd.home", usart.getData())) lcd.home();
        else if (compare("lcd.backlightSet", usart.getData())) lcd.backlightSet(usart.getData()[16] - 0x30);
        else if (compare("lcd.cursorGoTo", usart.getData())) lcd.cursorGoTo(usart.getData()[14] - 0x30, usart.getData()[15] - 0x30);
        else if (compare("lcd.displaySet", usart.getData())) lcd.displaySet(usart.getData()[14] - 0x30);
        else if (compare("lcd.cursorSet", usart.getData())) lcd.cursorSet(usart.getData()[13] - 0x30);
        else if (compare("lcd.cursorBlinkSet", usart.getData())) lcd.cursorBlinkSet(usart.getData()[18] - 0x30);
        else if (compare("lcd.scroll", usart.getData())) lcd.scroll(usart.getData()[10] - 0x30, usart.getData()[11] - 0x30);
        else if (compare("lcd.textFlowSet", usart.getData())) lcd.textFlowSet(usart.getData()[15] - 0x30);
        else if (compare("lcd.autoScrollSet", usart.getData())) lcd.autoScrollSet(usart.getData()[17] - 0x30);
        else if (compare("lcd.putc", usart.getData())) lcd.putc(usart.getData()[8]);
        else if (compare("lcd.puts", usart.getData())) lcd.puts((const char*)&(usart.getData()[8]));
        else if (compare("lcd.bellLoad", usart.getData())) lcd.loadCustomSymbol(1, bell);
        else if (compare("lcd.bellPrint", usart.getData())) lcd.putc(1);
        else usart.puts(" unkn cmd!!!");
    }
    else {

        usart.puts(" no cmd");
    }
}

void RTCTestInterfaceToUsart(Usart& usart, RealTimeClock& rtc) {
        
    usart.puts("\r\n Check for RTC: ");

    if (usart.getCount() > 0) {
                
        if (compare("rtc.getUnixTime", usart.getData())) {
            usart.puts(" time = ");
            usart.puts(itoa(rtc.getUnixTime(), 10));
        }
        else usart.puts(" unkn cmd!!!");
    }
    else {

        usart.puts(" no cmd");
    }
}

void Ds18b20TestInterfaceToUsart(Usart& usart, Ds18b20& sensor) {
        
    usart.puts("\r\n Check for DS18b20: ");

    if (usart.getCount() > 0) {

        if (compare("ds18b20.saveSettings", usart.getData())) sensor.saveSettings();
        else if (compare("ds18b20.restoreSettings", usart.getData())) sensor.restoreSettings();
        else if (compare("ds18b20.isErrorState", usart.getData())) usart.puts((sensor.isErrorState() ? "err" : "ok"));
        else if (compare("ds18b20.getTemperature", usart.getData())) usart.puts(itoa(sensor.getTemperature(), 10));
        else if (compare("ds18b20.getPowerMode", usart.getData())) {
            usart.puts((sensor.getPowerMode() == Ds18b20::PowerMode::external ? "external" : "parasite"));
        }
        else if (compare("ds18b20.getResolution", usart.getData())) usart.puts(itoa((uint8_t)(sensor.getResolution()), 16));
        else if (compare("ds18b20.getAlarmTemp", usart.getData())) usart.puts(itoa(sensor.getAlarmTemp(), 16));
        else if (compare("ds18b20.setResolution", usart.getData())) {

            sensor.setResolution((Ds18b20::Resolution)atoi((uint8_t *)&(usart.getData()[21]), 16, 2));
        }
        else if (compare("ds18b20.setAlarmTemp", usart.getData())) {
            
            sensor.setAlarmTemp(atoi((uint8_t *)&(usart.getData()[20]), 16, 2), atoi((uint8_t *)&(usart.getData()[22]), 16, 2));
        }
        
        else usart.puts(" unkn cmd!!!");
    }
    else {

        usart.puts(" no cmd");
    }
}
