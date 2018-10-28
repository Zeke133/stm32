#include <main.h>

void LedTestInterfaceToUsart(Usart& usart, LED& led);
void LcdTestInterfaceToUsart(Usart& usart, Hd44780& lcd);
void RTCTestInterfaceToUsart(Usart& usart, RealTimeClock& rtc);
void Ds18b20TestInterfaceToUsart(Usart& usart, Ds18b20& sensor);

int main(void) {

    SetSysClockTo72();

    DMA dmaForUsart(DMA::Device::USART1_TX);
    Usart usart1(1, dmaForUsart, 115200);
    OStream cout(usart1);

    cout << "\nSTM32F103C8T6. USART1 is ready.\nFirmware: ";
    cout << __DATE__ << ' ' << __TIME__;

    Delay delayer;
    LED led(GPIOC, GPIO_Pin_13);
    RealTimeClock rtc(delayer);

    DateTime dt;
    rtc.getTime(dt);
    cout << OStream::OutSet::dec;
    cout << "\nRTC: " << dt.date << "." << dt.month << "." << dt.year;
    cout << " " << dt.hours << ":" << dt.minutes << "." << dt.seconds;

    Storage flash;
    flash.data.var1 ++;
    flash.writeToFlash();
    flash.readToRam();
    cout << "\nReload after ERASING: " << OStream::OutSet::dec << flash.data.var1;

    DMA dmaForI2c1(DMA::Device::I2C1_TX);
    I2c i2cPort(1, dmaForI2c1);
    // Hd44780 lcd(i2cPort, delayer, 1, 16);
    // lcd.puts("Hello =)");
    // lcd.setCursorShow(1);
    // lcd.setCursorBlink(1);
    // lcd.setCursor(1, 0);
    // lcd.puts("Temp:");
    // lcd.setCursor(0, 3);

    Ssd1306 oled(i2cPort, delayer);
    oled.fill(0);
    Font_7x10 fontS;
    Font_11x18 fontM;
    TextRender textRender(oled, fontS);
    textRender.puts("Hello;)ABCDEFGH");
    oled.update();
    delayer.ms(1000);
    TextRender textRender2(oled, fontM);
    textRender2.setCursor(1,0);
    textRender2.puts("Hello;)AB");
    oled.update();

    // uint8_t rom[8] = {0x28, 0x82, 0x65, 0x5B, 0x05, 0x00, 0x00, 0x20};
    // OneWire oneWire(delayer, GPIOA, GPIO_Pin_8);
    // Ds18b20 tempSensor(oneWire, delayer, rom);   // 1-Wire DS18B20 temperature sensor
    // cout << "\nDs18b20 stateIs " << (tempSensor.isErrorState() ? "err" : "ok");

    uint8_t channels[] = {16, 17};
    ADC adc1(   1, 
                RCC_PCLK2_Div6,
                ADC::ResultStoreMode::Injected, 
                2, 
                channels);          // ADC setup

    cout << "\nADC test: ";
    cout << OStream::OutSet::dec;
    cout << adc1.getValue(0) << ", ";    // temp
    cout << adc1.getValue(1);            // Vref

    // Search test
    // OneWireSearch oneWireDevices(oneWire);

    // cout << "\r\nSearch start: ";
    // if (oneWireDevices.searchAllDevices()) {
    //     cout << "found ";
    //     cout << itoa(oneWireDevices.devicesConnectedCount, 10);
    //     for(uint32_t i = 0; i < oneWireDevices.devicesConnectedCount; i++) {
    //         cout << "\r\n";
    //         for(uint32_t j = 0; j < 8; j++) {
    //             cout << itoa(oneWireDevices.devicesConnected[i][j],16);
    //             cout << ",";
    //         }
    //     }
    // }
    // else {
    //     cout << "not found";
    // }

    while (1) {

        // Delay metering check
        delayer.us(1000000);
        led.invert();
        cout << "\nADC test: ";
        cout << OStream::OutSet::dec;
        cout << adc1.getValue(0) << ", ";    // temp
        cout << adc1.getValue(1);            // Vref

        // usart1.sendBlocking(" Rcv: ");
        // usart1.send((const char *)usart1.getData(), usart1.getCount());

        // LedTestInterfaceToUsart(usart1, led);
        // LcdTestInterfaceToUsart(usart1, lcd);
        // RTCTestInterfaceToUsart(usart1, rtc);
        // Ds18b20TestInterfaceToUsart(usart1, tempSensor);

        // usart1.clear();

    }

}




