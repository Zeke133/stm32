#include <main.h>

int main(void) {

    SetSysClockTo72();
    LED led(GPIOC, GPIO_Pin_13);
    Delay delayer;

    DMA dmaUsart1Tx(DMA::Device::USART1_TX);
    Usart usartPC(1, dmaUsart1Tx, 115200);
    DMA dmaUsart3Tx(DMA::Device::USART3_TX);
    Usart usartRadio(3, dmaUsart3Tx, 300);      // 300 8n1
    OStream cout(usartPC);
    cout << "\nSTM32F103C8T6. USART1 is ready.\nFirmware: ";
    cout << __DATE__ << ' ' << __TIME__;

    RealTimeClock rtc(delayer);
    {
        DateTime dt;
        rtc.getTime(dt);
        cout << OStream::OutSet::dec;
        cout << "\nRTC: " << dt.date << "." << dt.month << "." << dt.year;
        cout << " " << dt.hours << ":" << dt.minutes << "." << dt.seconds;
    }

    Storage flash;
    flash.data.var1 ++;
    flash.writeToFlash();
    flash.readToRam();
    cout << "\nReload after ERASING: " << OStream::OutSet::dec << flash.data.var1;

    DMA dmaForI2c1(DMA::Device::I2C1_TX);
    I2c i2cPort(1, dmaForI2c1);
    // Hd44780 lcd(i2cPort, delayer, 1, 16);
    Ssd1306 oled(i2cPort, delayer);
    oled.fill(0);
    Font_7x10 fontS;
    TextRender textRender(oled, fontS);
    textRender.puts("Fw: ");
    textRender.puts(__DATE__);
    textRender.putc(' ');
    textRender.puts(__TIME__);
    oled.update();

    Tacho tacho;

    uint8_t rom[8] = {0x28, 0x82, 0x65, 0x5B, 0x05, 0x00, 0x00, 0x20};
    OneWire oneWire(delayer, GPIOA, GPIO_Pin_8);
    Ds18b20 tempSensor(oneWire, delayer, rom);
    cout << "\nDs18b20 stateIs " << (tempSensor.isErrorState() ? "err" : "ok");
    cout << "\nTemp: " << (tempSensor.getTemperature());

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

    uint8_t key = 0;

    while (1) {

        delayer.ms(100);

        {
            key = 0;
            uint8_t * data = usartRadio.getData();
            uint8_t size = usartRadio.getCount();
            for (uint8_t i = 0; i < size - 1; ++i) {
                if (data[i] == 0x29) {
                    if (data[i+1] == 0xA5 && data[i+2] == 0x06) key = 1;// TOP      29 A5 06
                    if (data[i+1] == 0x65 && data[i+2] == 0x06) key = 2;// LEFT     29 65 06
                    if (data[i+1] == 0x25 && data[i+2] == 0x03) key = 3;// RIGHT    29 25 03
                }
            }
            usartRadio.clear();
        }

        usartPC.putsBufferized("\nTextOutput using USART and DMA");
        usartPC.putsBufferized("\nIn case of problems previous line will be corrupted!");

        textRender.clearRow(2);
        textRender.setCursor(2,0);
        textRender.puts("Key:");
        if (key == 0) textRender.puts("---");
        if (key == 1) textRender.puts("OK");
        if (key == 2) textRender.puts("Left");
        if (key == 3) textRender.puts("Right");

        textRender.clearRow(3);
        textRender.setCursor(3,0);
        textRender.puts("Ticks:");
        textRender.puts(itoa(tacho.getTicksPerRotation(),10));

        textRender.clearRow(4);
        textRender.setCursor(4,0);
        textRender.puts("RPM:");
        textRender.puts(itoa(tacho.getRpm(),10));

        textRender.clearRow(5);
        textRender.setCursor(5,0);
        textRender.puts("Temp:");
        textRender.puts(itoa(tempSensor.getLastTemperature(),10));
        textRender.puts("C");

        oled.update();

        tempSensor.initTemperatureMeasurment();

    }

}




