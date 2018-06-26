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
#include <analogSensors.h>

#include <convertation.h>
#include <strings.h>


void LedTestInterfaceToUsart(USART& usart, LED& led);
void LcdTestInterfaceToUsart(USART& usart, Lcd& lcd);
void RTCTestInterfaceToUsart(USART& usart, RealTimeClock& rtc);
void Ds18b20TestInterfaceToUsart(USART& usart, Ds18b20& sensor);

void adcInit(void) {

    //clock for ADC (max 14MHz --> 72/6=12MHz)
    RCC_ADCCLKConfig (RCC_PCLK2_Div6);
    // enable ADC system clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
 
    //ADC
    ADC_InitTypeDef ADC_InitStructure;
    // define ADC config
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // we work in continuous sampling mode
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_239Cycles5); // define regular conversion config
    ADC_Init (ADC1, &ADC_InitStructure);    //set config of ADC1
 
    // Enable Temperature sensor
    ADC_TempSensorVrefintCmd(ENABLE);
 
    // Enable ADC
    ADC_Cmd (ADC1, ENABLE); //enable ADC1
 
    //  ADC calibration (optional, but recommended at power on)
    ADC_ResetCalibration(ADC1); // Reset previous calibration
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1); // Start new calibration (ADC must be off at that time)
    while(ADC_GetCalibrationStatus(ADC1));
 
    // start conversion
    ADC_Cmd (ADC1,ENABLE);  //enable ADC1
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // start conversion (will be endless as we are in continuous mode)
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

    uint8_t rom[8] = {0x28, 0x82, 0x65, 0x5B, 0x05, 0x00, 0x00, 0x20};
    Ds18b20 tempSensor(wait, GPIOA, GPIO_Pin_8, rom);    // 1-Wire DS18B20 temperature sensor
    // OneWire oneWire(wait, GPIOA, GPIO_Pin_8);

    Storage flash;                  // Flash storage

    uint8_t channels[] = {16, 17};
    ADC adc1(   1, 
                RCC_PCLK2_Div6, 
                ADC::ResultStoreMode::Regular, 
                2, 
                channels);          // ADC setup

    InternalTemp tempInternal(adc1, 0);

    // usart test
    usart1 << "\nHello, I'm STM32F103.\r\nUSART1 is ready.";

    // lcd test
    lcd.puts("Hello =)");
    lcd.cursorGoTo(1, 0);
    lcd.puts("Temp:");    

    // RTC test
    DateTime dt;
    rtc.getTime(dt);
    usart1 << USART::OutSet::dec;
    usart1 << "\nDate: " << dt.date << "." << dt.month << "." << dt.year;
    usart1 << "\nTime: " << dt.hours << ":" << dt.minutes << "." << dt.seconds;

    // Ds18b20 test
    usart1 << "\nDs18b20 stateIs " << (tempSensor.isErrorState() ? "err" : "ok");
    
    // Flash test
    flash.data.var1 ++;
    flash.writeToFlash();
    flash.readToRam();
    usart1 << "\nFlash test: " << USART::OutSet::dec << flash.data.var1 << ", " << flash.data.var4;

    // ADC test
    usart1 << "\nADC test: ";
    usart1 << USART::OutSet::dec;
    usart1 << adc1.getValue(0) << ", ";    // temp
    usart1 << adc1.getValue(1);            // Vref

    // analog sensor
    uint32_t t = tempInternal.getTemperatureC();
    usart1 << "\nInternal sensor: " << t << " v25 = " << tempInternal.getV22();

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

    while (1) {

        // Delay metering check
        wait.us(1000000);
        led.invert();
        
        // Temp measuring to LCD
        int32_t temp = tempSensor.getTemperature();
        lcd.cursorGoTo(1, 5);
        usart1 << "\n";
        if (temp < 0) {
            lcd.putc('-');
            usart1 << "-";
        }
        const char * text1 = itoa(temp/10000, 10);
        lcd.puts(text1);
        usart1 << text1;
        lcd.putc('.');
        usart1 << '.';
        const char * text2 = itoa(temp%10000, 10, 4);
        lcd.puts(text2);
        usart1 << text2;

        usart1 << "\nADC: " << adc1.getValue(0) << ", " << adc1.getValue(1);

        // usart1.sendBlocking(" Rcv: ");
        // usart1.send((const char *)usart1.getData(), usart1.getCount());

        // LedTestInterfaceToUsart(usart1, led);
        // LcdTestInterfaceToUsart(usart1, lcd);
        // RTCTestInterfaceToUsart(usart1, rtc);
        // Ds18b20TestInterfaceToUsart(usart1, tempSensor);

        // usart1.clear();

    }

}




