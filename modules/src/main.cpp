#include <SysClock.h>
#include <led.h>
#include <usart.h>
#include <oneWire.h>
#include <oneWireSearch.h>
#include <i2c.h>
#include <lcd.h>
#include <rtc.h>
#include <adc.h>
#include <delay.h>
#include <storage.h>

#include <convertation.h>

unsigned int compare(const char * str1, const uint8_t * str2) {

    while (*str1 && *str2) {
        if (*str1 != *str2) return 0;
        str1++;
        str2++;
    }
    return 1;
}

void TakeTimeouts(USART& usart, Delay& wait, uint8_t* timeoutsPtr) {
    
    usart << "\xd\xaWait for receive of timeouts...";

    if (usart.getCount() < 9) {
        wait.ms(100);
    }

    usart << "\xd\xaReceived:";
    for (uint32_t i = 0; i < 9; i++) {
        usart << "\xd\xa-";
        usart << itoa((usart.getData())[i], 10);

        timeoutsPtr[i] = (usart.getData())[i];
    }

}

int main(void) {

	SetSysClockTo72();

    // Init resourses
    Delay wait;                     // Delay based on SysTickTimer
    USART usart1(1);                // USART1
    LED led(GPIOC, GPIO_Pin_13);    // LED on gpio
    I2c i2cPort(1, 0x27);           // I2C parallel converter

    Lcd lcd(i2cPort, wait, 2);      // LCD on I2C adaptor

    // RealTimeClock rtc(wait);        // RealTime Clock

    // uint8_t channels[12] = {16, 17};
    // ADC adc1(1, 
    //             RCC_PCLK2_Div6, 
    //             ADC::ResultStoreMode::Regular, 
    //             2, 
    //             channels);          // ADC setup

    // Storage flash;

    // usart test
    usart1 << "Hello.\r\nUSART1 is ready.\r\n";

    // lcd test
    lcd.backlightSet(1);
    lcd.puts("Hello Katerina =)");
    lcd.cursorGoTo(1, 0);
    lcd.puts("Good night!");

    // 1-wire test
    // uint8_t timeouts[3][3] = { {60, 10, 50}, {3, 90, 5}, {1, 7, 90} };
    // OneWire one_wire(wait, GPIOA, GPIO_Pin_8, timeouts[0], timeouts[1], timeouts[2]);

    // uint8_t buf[8];
    // usart1 << "\r\none_wire send: 0x33, ";
    // if (one_wire.ReadROM(buf)) {
    //     usart1 << "Succ";
    // } else {
    //     usart1 << "Err";
    // }
    // usart1 << "\r\none_wire resp: ";
    // for(uint32_t i = 0; i < 8; i++) {
    //     usart1 << itoa(buf[i],16);
    //     usart1 << ",";
    // }

    // Search test
    // OneWireSearch oneWireDevices(one_wire);

    // usart1 << "\r\nSearch start: ";
    // if (oneWireDevices.searchAllDevices()) {
    //     usart1 << "found ";
    //     usart1 << itoa(oneWireDevices.devicesConnectedCount, 10);
    //     for(uint32_t i = 0; i < oneWireDevices.devicesConnectedCount; i++) {
    //         usart1 << "\r\n";
    //         for(uint32_t j = 0; j < 8; j++) {
    //             usart1 << itoa(oneWireDevices.devicesConnected[i][j],16);
    //             usart1 << ",";
    //         }
    //     }
    // }
    // else {
    //     usart1 << "not found";
    // }

    // ADC test
    // usart1 << "\r\nADC test: ";
    // usart1 << itoa(adc1.getValue(16), 16) << ", ";  // temp
    // usart1 << itoa(adc1.getValue(17), 16);  // Vref

    // Flash test
    // usart1 << "\r\nFlash test: ";
    // usart1 << itoa(flash.data.var1, 10) << ", ";
    // usart1 << itoa(flash.data.var4, 10) << ", ";
    // flash.data.var1 = 1001;
    // flash.data.var4 = 4004;
    // flash.writeToFlash();
    // flash.readToRam();
    // usart1 << "\r\nafter update: ";
    // usart1 << itoa(flash.data.var1, 10) << ", ";
    // usart1 << itoa(flash.data.var4, 10) << ", ";

    while (1) {

        // Delay metering check
        wait.ms(1000);

        //led.invert();

        take this TEST to a module
        change all uint8_t to char ?

        usart1.sendBlocking("\xd\xaWait for receive cmd...");
        
        if (usart1.getCount() > 0) {

            uint8_t bell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
            
            usart1.sendBlocking(" Received: ");
            usart1.send((const char *)usart1.getData(), usart1.getCount());
            
            if (compare("led.on", usart1.getData())) led.on();
            else if (compare("led.off", usart1.getData())) led.off();
            else if (compare("led.invert", usart1.getData())) led.invert();
            
            else if (compare("lcd.clear", usart1.getData())) lcd.clear();
            else if (compare("lcd.home", usart1.getData())) lcd.home();
            else if (compare("lcd.backlightSet", usart1.getData())) lcd.backlightSet(usart1.getData()[16] - 0x30);
            else if (compare("lcd.cursorGoTo", usart1.getData())) lcd.cursorGoTo(usart1.getData()[14] - 0x30, usart1.getData()[15] - 0x30);
            else if (compare("lcd.displaySet", usart1.getData())) lcd.displaySet(usart1.getData()[14] - 0x30);
            else if (compare("lcd.cursorSet", usart1.getData())) lcd.cursorSet(usart1.getData()[13] - 0x30);
            else if (compare("lcd.cursorBlinkSet", usart1.getData())) lcd.cursorBlinkSet(usart1.getData()[18] - 0x30);
            else if (compare("lcd.scroll", usart1.getData())) lcd.scroll(usart1.getData()[10] - 0x30, usart1.getData()[11] - 0x30);
            else if (compare("lcd.textFlowSet", usart1.getData())) lcd.textFlowSet(usart1.getData()[15] - 0x30);
            else if (compare("lcd.autoScrollSet", usart1.getData())) lcd.autoScrollSet(usart1.getData()[17] - 0x30);
            else if (compare("lcd.putc", usart1.getData())) lcd.putc(usart1.getData()[8]);
            else if (compare("lcd.puts", usart1.getData())) lcd.puts((const char*)&(usart1.getData()[8]));
            else if (compare("lcd.bellLoad", usart1.getData())) lcd.loadCustomSymbol(1, bell);
            else if (compare("lcd.bellPrint", usart1.getData())) lcd.putc(1);
            else usart1.sendBlocking(" unkn cmd!!!");    

            usart1.clear();
        }
        else {
            usart1.sendBlocking(" no cmd");
        }
        wait.ms(1000);

        // Take user timeouts for one-wire bus
        // TakeTimeouts(usart1, wait, (uint8_t*)timeouts);

        // Run and print results
        // OneWire OneWire(wait, GPIOA, GPIO_Pin_8, timeouts[0], timeouts[1], timeouts[2]);
        // usart1 << "\r\none_wire send: 0x33, ";
        // if (one_wire.ReadROM(buf)) {
        //     usart1 << "Succ";
        // } else {
        //     usart1 << "Err";
        // }
        // usart1 << "\r\none_wire resp: ";
        // for(uint32_t i = 0; i < 8; i++) {
        //     usart1 << itoa(buf[i],16);
        //     usart1 << ",";
        // }
    }

}




