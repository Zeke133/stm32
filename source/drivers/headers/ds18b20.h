#ifndef _DS18B20_H
#define _DS18B20_H

#include <oneWire.h>

#include <crc.h>
#include <convertation.h>

/*
Dallas ds18b20 temperature sensor driver
*/
class Ds18b20 : private OneWire {

friend int main(void);

public:

    enum class Resolution : uint8_t {

        // masks with bits for Configuraion register
        _9bit = 0x00,       // 9-bit resolution  93.75 ms
        _10bit = 0x20,      // 10-bit resolution  187.5 ms
        _11bit = 0x40,      // 11-bit resolution  375 ms
        _12bit = 0x60       // 12-bit resolution  750 ms
    };
    enum class PowerMode : uint8_t {

        parasite = 0,
        external = 1
    };
    
    // configuration
    void setAlarmTemp(int8_t Th, int8_t Tl);
    uint16_t getAlarmTemp(void);
    void setResolution(enum Resolution res);
    enum Resolution getResolution(void);

    // control temperature measurments
    int32_t getTemperature(void);                   // Get current temperature in 1 step. Blocks for a duration of measurment up to 750ms!
    void initTemperatureMeasurment(void);           // Start measurment. Doesn't wait for a result. Non blocking.
    int32_t getLastTemperature(void);               // Get measurment result in format 850000 = 85.0 C

    // status
    enum PowerMode getPowerMode(void);
    uint8_t isErrorState(void);

    // config storage in EEPROM
    void saveSettings(void);
    void restoreSettings(void);

    // delete copy constructor and assignment operator
    Ds18b20(const Ds18b20&) = delete;
    Ds18b20& operator=(const Ds18b20&) = delete;

private:

    Ds18b20(Delay& timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, enum Resolution res = Resolution::_12bit);
    Ds18b20(Delay& timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t * ROM, enum Resolution res = Resolution::_12bit);

    // procedures
    void initialization(enum Resolution res);
    uint8_t selectDevice(void);
    
    // hardware commands
    uint8_t writeScratchpad(void);
    uint8_t readScratchpad(void);        
    uint8_t copyScratchpad(void);
    uint8_t recallEE(void);
    uint8_t readPowerSupply(void);
    uint8_t convertT(void);

    void waitConvertionEnd(void);

    // -
    uint8_t errorState;
    uint8_t useROM = 0;
    uint8_t ROM[8];
    enum PowerMode powerMode;

    // scratchpad data
    uint8_t tempTrigerHigh;
    uint8_t tempTrigerLow;
    uint8_t configRegister;     // 0 R1 R0 1  1 1 1 1
    uint16_t temperature;
    
};

#endif
