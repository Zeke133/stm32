#ifndef _ADC_H
#define _ADC_H

// using
extern "C" {
#include <stm32f1xx_ll_adc.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_dma.h>
}
#include <gpio.h>

/*
ADC driver class
Regular channels use DMA controller
ADC is 12 bit so max val = 4095
Vref = 3.3V - ?
*/
class ADC {

public:

    enum class ResultStoreMode : uint8_t {
        Injected,
        Regular
    };

    ADC(uint32_t adcN, uint32_t clkDiv, enum ResultStoreMode chType, uint32_t chCnt, const uint8_t * chNumbers);

    // delete copy constructor and assignment operator
    ADC(const ADC&) = delete;
    ADC& operator=(const ADC&) = delete;

    // Get value of chanel, channel number is array index 0...n
    uint16_t getValue(uint8_t channel) const;

private:

    void init(void);

    ADC_TypeDef * adcUsed;              // address of used ADC
    enum ResultStoreMode channelsType;  // regular or injected channels used
    uint32_t clockDivider;              // clock of ADC

    uint32_t channelsCnt;               // number of channels used
    uint8_t channelsNumbers[12];        // 0 to 17. TempSens 16 Vref 17
    uint32_t useDMA;                    // flag of using DMA for results storing
    uint16_t dataStorage[12];           // conversion rusults stored by DMA

    const uint32_t mapChannel2Injected[4] = {    // constexpr?
        LL_ADC_INJ_RANK_1,
        LL_ADC_INJ_RANK_2,
        LL_ADC_INJ_RANK_3,
        LL_ADC_INJ_RANK_4
    };

};

#endif
