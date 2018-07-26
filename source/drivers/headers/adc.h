#ifndef _ADC_H
#define _ADC_H

extern "C" {
#include <stm32f10x_adc.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_dma.h>
}

#include <gpio.h>


// 
/*
ADC driver class
ADC is 12 bit so max val = 4095
Vref = 3.3V - ?
*/
class ADC {

friend int main(void);

public:

    enum class ResultStoreMode {
        Injected,
        Regular
    };

    // Get value of chanel, channel number is array index 0...n
    uint16_t getValue(uint8_t channel) const;

    // delete copy constructor and assignment operator
    ADC(const ADC&) = delete;
    ADC& operator=(const ADC&) = delete;

private:

    ADC(uint32_t adcN, uint32_t clkDiv, enum ResultStoreMode chType, uint32_t chCnt, const uint8_t * chNumbers);

    void init(void);

    ADC_TypeDef * adcUsed;              // address of used ADC
    enum ResultStoreMode channelsType;  // regular or injected channels used
    uint32_t clockDivider;              // clock of ADC
    
    uint32_t channelsCnt;               // number of channels used
    uint8_t channelsNumbers[12];        // 0 to 17. TempSens 16 Vref 17
    uint32_t useDMA;                    // flag of using DMA for results storing
    uint16_t dataStorage[12];           // conversion rusults stored by DMA

    const uint8_t mapChannel2Injected[4] = {
        ADC_InjectedChannel_1,
        ADC_InjectedChannel_2,
        ADC_InjectedChannel_3,
        ADC_InjectedChannel_4
    };

};

#endif
