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

#include <convertation.h>


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

    RealTimeClock rtc(wait);        // RealTime Clock

    uint8_t channels[12] = {16, 17};
    ADC adc1(1, RCC_PCLK2_Div6, ADC::ResultStoreMode::Regular, 2, channels); // ADC setup

    // usart test
    usart1 << "Hello.\r\nUSART1 is ready.\r\n";

    // lcd test
    lcd.backlightSet(1);
    lcd.puts("Hello LCD");

    // 1-wire test
    uint8_t timeouts[3][3] = { {60, 10, 50}, {3, 90, 5}, {1, 7, 90} };
    OneWire one_wire(wait, GPIOA, GPIO_Pin_8, timeouts[0], timeouts[1], timeouts[2]);

    uint8_t buf[8];
    usart1 << "\r\none_wire send: 0x33, ";
    if (one_wire.ReadROM(buf)) {
        usart1 << "Succ";
    } else {
        usart1 << "Err";
    }
    usart1 << "\r\none_wire resp: ";
    for(uint32_t i = 0; i < 8; i++) {
        usart1 << itoa(buf[i],16);
        usart1 << ",";
    }

    // Search test
    OneWireSearch oneWireDevices(one_wire);

    usart1 << "\r\nSearch start: ";
    if (oneWireDevices.searchAllDevices()) {
        usart1 << "found ";
        usart1 << itoa(oneWireDevices.devicesConnectedCount, 10);
        for(uint32_t i = 0; i < oneWireDevices.devicesConnectedCount; i++) {
            usart1 << "\r\n";
            for(uint32_t j = 0; j < 8; j++) {
                usart1 << itoa(oneWireDevices.devicesConnected[i][j],16);
                usart1 << ",";
            }
        }
    }
    else {
        usart1 << "not found";
    }

    // ADC test
    usart1 << "\r\nADC test: ";
    usart1 << itoa(adc1.getValue(16), 16) << ", ";  // temp
    usart1 << itoa(adc1.getValue(17), 16);  // Vref

    while (1) {

        // Delay metering check
        wait.ms(1000);
        led.invert();

        // Take user timeouts for one-wire bus
        TakeTimeouts(usart1, wait, (uint8_t*)timeouts);

        // Run and print results
        OneWire OneWire(wait, GPIOA, GPIO_Pin_8, timeouts[0], timeouts[1], timeouts[2]);
        usart1 << "\r\none_wire send: 0x33, ";
        if (one_wire.ReadROM(buf)) {
            usart1 << "Succ";
        } else {
            usart1 << "Err";
        }
        usart1 << "\r\none_wire resp: ";
        for(uint32_t i = 0; i < 8; i++) {
            usart1 << itoa(buf[i],16);
            usart1 << ",";
        }
    }

}




