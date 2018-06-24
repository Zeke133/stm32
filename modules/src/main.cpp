#include <SysClock.h>
#include <led.h>
#include <usart.h>
#include <oneWire.h>
#include <oneWireSearch.h>
#include <ds18b20.h>
#include <i2c.h>
#include <lcd.h>
#include <rtc.h>
#include <adc.h>
#include <delay.h>
#include <storage.h>

#include <convertation.h>
#include <strings.h>


void LedTestInterfaceToUsart(USART& usart, LED& led);
void LcdTestInterfaceToUsart(USART& usart, Lcd& lcd);
void RTCTestInterfaceToUsart(USART& usart, RealTimeClock& rtc);
void Ds18b20TestInterfaceToUsart(USART& usart, Ds18b20& sensor);


int main(void) {

	SetSysClockTo72();

    // Init resourses
    Delay wait;                     // Delay based on SysTickTimer
    USART usart1(1);                // USART1
    LED led(GPIOC, GPIO_Pin_13);    // LED on gpio

    I2c i2cPort(1, 0x27);           // I2C parallel converter
    Lcd lcd(i2cPort, wait, 2);      // LCD on I2C adaptor

    RealTimeClock rtc(wait);        // RealTime Clock

    // 1-Wire DS18B20 temperature sensor
    uint8_t rom[8] = {0x28, 0x82, 0x65, 0x5B, 0x05, 0x00, 0x00, 0x20};
    Ds18b20 tempSensor(wait, GPIOA, GPIO_Pin_8, rom);
    // OneWire oneWire(wait, GPIOA, GPIO_Pin_8);

    uint8_t channels[12] = {16, 17};
    ADC adc1(1, 
                RCC_PCLK2_Div6, 
                ADC::ResultStoreMode::Regular, 
                2, 
                channels);          // ADC setup

    // Storage flash;

    // usart test
    usart1 << "\r\nHello, I'm STM32F103.\r\nUSART1 is ready.";

    // lcd test
    lcd.puts("Hello =)");
    lcd.cursorGoTo(1, 0);
    lcd.puts("Temp:");    

    // Ds18b20 test
    usart1 << "\r\nDs18b20 stateIs " << (tempSensor.isErrorState() ? "err" : "ok");
    usart1 << "\r\ntemp = " << itoa(tempSensor.getTemperature(), 10) << "\r\n";

    // Search test
    // OneWireSearch oneWireDevices(oneWire);

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
    usart1 << "\r\nADC test: ";
    usart1 << itoa(adc1.getValue(16), 16) << ", ";  // temp
    usart1 << itoa(adc1.getValue(17), 16);  // Vref

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
        wait.us(1000000);
        led.invert();
        
        // Temp measuring to LCD
        int32_t temp = tempSensor.getTemperature();
        lcd.cursorGoTo(1, 5);
        usart1 << "\r\n";
        if (temp < 0) {
            lcd.putc('-');
            usart1 << "-";
        }
        const char * text1 = itoa(temp/10000, 10, 4);
        lcd.puts(text1);
        usart1 << text1;
        lcd.putc('.');
        usart1 << '.';
        const char * text2 = itoa(temp%10000, 10, 4);
        lcd.puts(text2);
        usart1 << text2;

        //---change all uint8_t to char ?

        // usart1.sendBlocking(" Rcv: ");
        // usart1.send((const char *)usart1.getData(), usart1.getCount());

        // LedTestInterfaceToUsart(usart1, led);
        // LcdTestInterfaceToUsart(usart1, lcd);
        // RTCTestInterfaceToUsart(usart1, rtc);
        // Ds18b20TestInterfaceToUsart(usart1, tempSensor);

        // usart1.clear();

    }

}




