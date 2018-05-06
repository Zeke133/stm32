#ifndef _DS18B20_H
#define _DS18B20_H

#include <dallas_1wire.h>
#include <crc.h>


class DS18B20 : private Dallas_1wire_controller {

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

        DS18B20(Delay& timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, enum Resolution res = Resolution::_12bit);

        void convertT(void);

        // configuration
        void setAlarmTemp(int8_t Th, int8_t Tl);
        void setResolution(enum Resolution res);
        enum Resolution getResolution(void);

        void writeScratchpad(void);
        uint8_t readScratchpad(void);        
        void copyScratchpad(void);
        void recallEE(void);

        enum PowerMode readPowerSupply(void);

    private:

        enum PowerMode powerMode;

        uint8_t tempTrigerHigh;
        uint8_t tempTrigerLow;
        enum Resolution configRegister;     // 0 R1 R0 1  1 1 1 1
        uint16_t temperature;
        
};

#endif
