#include <adc.h>

ADC::ADC( uint32_t adcN,
          uint32_t clkDiv,                    // example: RCC_PCLK2_Div6
          enum ResultStoreMode chType,
          uint32_t chCnt,
          const uint8_t * chNumbers) {

    if (adcN == 1) adcUsed = ADC1;
    else adcUsed = ADC2;

    clockDivider = clkDiv;
    channelsType = chType;
    useDMA = 0;
    if (channelsType == ResultStoreMode::Injected) {
        channelsCnt = chCnt > 4 ? 4 : chCnt;            // have only 4 injected channels
    }
    else if (channelsType == ResultStoreMode::Regular) {
        if (chCnt > 1) {
            if (adcUsed == ADC1) {
                channelsCnt = chCnt > 12 ? 12 : chCnt;  // have only 12 regular channels
                useDMA = 1;                             // use DMA when more than 1 channel
            }
            else channelsCnt = 1;                       // ADC2 have no DMA so just 1 channel can be stored
        }
        else channelsCnt = chCnt;
    }
    
    for(uint32_t i = 0; i < channelsCnt; i++) {
        channelsNumbers[i] = chNumbers[i];
    }

    init();
}

void ADC::init() {

    // set clock for ADC. MAX can be 14Mhz!
    LL_RCC_SetADCClockSource(clockDivider);
    // enable ADC system clock
    LL_APB2_GRP1_EnableClock((adcUsed == ADC1) ? LL_APB2_GRP1_PERIPH_ADC1 : LL_APB2_GRP1_PERIPH_ADC2);

    // DMA configuration
    if (useDMA) {

        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

        LL_DMA_InitTypeDef params {
            .PeriphOrM2MSrcAddress = (uint32_t)&adcUsed->DR,
            .MemoryOrM2MDstAddress = (uint32_t)dataStorage,
            .Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY,
            .Mode = LL_DMA_MODE_CIRCULAR,
            .PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT,
            .MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT,
            .PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD,
            .MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD,
            .NbData = channelsCnt,
            .Priority = LL_DMA_PRIORITY_LOW
        };
        LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &params);
        
        LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    }

    // Config GPIO inputs of PortA & PortB
    {
        uint32_t pinsA = 0, pinsB = 0;

        for(uint32_t i = 0; i < channelsCnt; i++) {

            uint32_t channel = channelsNumbers[i];
            if (channel < 8) pinsA |= (1 << channel);
            else if (channel == 8) pinsB |= LL_GPIO_PIN_0;
            else if (channel == 9) pinsB |= LL_GPIO_PIN_1;
        }
        if (pinsA)
            GPIO::initPins(GPIOA, pinsA, LL_GPIO_MODE_ANALOG, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_DOWN, LL_GPIO_SPEED_FREQ_HIGH);
        if (pinsB)
            GPIO::initPins(GPIOB, pinsB, LL_GPIO_MODE_ANALOG, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_DOWN, LL_GPIO_SPEED_FREQ_HIGH);
    }

    // Common ADC config
    {
        LL_ADC_CommonInitTypeDef mode {
            .Multimode = LL_ADC_MULTI_INDEPENDENT
        };
        LL_ADC_CommonStructInit(&mode);

        LL_ADC_InitTypeDef params {
            .DataAlignment = LL_ADC_DATA_ALIGN_RIGHT,
            .SequencersScanMode = channelsCnt > 1 ? LL_ADC_SEQ_SCAN_ENABLE : LL_ADC_SEQ_SCAN_DISABLE
        };
        LL_ADC_Init(adcUsed, &params);
    }

    if (channelsType == ResultStoreMode::Injected) {
        
        LL_ADC_INJ_InitTypeDef params {
            .TriggerSource = LL_ADC_INJ_TRIG_SOFTWARE,
            .SequencerLength = channelsCnt,
            .SequencerDiscont = LL_ADC_INJ_SEQ_DISCONT_DISABLE,
            .TrigAuto = LL_ADC_INJ_TRIG_FROM_GRP_REGULAR
        };
        LL_ADC_INJ_Init(adcUsed, &params);

        for(uint32_t i = 0; i < channelsCnt; i++) {

            LL_ADC_INJ_SetSequencerRanks(adcUsed, i+1, channelsNumbers[i]);
            LL_ADC_SetChannelSamplingTime(adcUsed, channelsNumbers[i], LL_ADC_SAMPLINGTIME_7CYCLES_5);
        }
    } else {

        LL_ADC_REG_InitTypeDef params {
            .TriggerSource = LL_ADC_INJ_TRIG_SOFTWARE,
            .SequencerLength = channelsCnt,
            .SequencerDiscont = LL_ADC_INJ_SEQ_DISCONT_DISABLE,
            .ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS,
            .DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED
        };
        LL_ADC_REG_Init(adcUsed, &params);

        for(uint32_t i = 0; i < channelsCnt; i++) {

            LL_ADC_REG_SetSequencerRanks(adcUsed, i+1, channelsNumbers[i]);
            LL_ADC_SetChannelSamplingTime(adcUsed, channelsNumbers[i], LL_ADC_SAMPLINGTIME_7CYCLES_5);
        }
    }

    // Enable Temperature sensor if used
    for(uint32_t i = 0; i < channelsCnt; i++) {

        if (channelsNumbers[i] == LL_ADC_CHANNEL_TEMPSENSOR) {

            LL_ADC_SetCommonPathInternalCh(ADC12_COMMON, LL_ADC_PATH_INTERNAL_TEMPSENSOR);
        }
    }

    // Enable ADC
    LL_ADC_Enable(adcUsed);

    // Enable DMA
    if (useDMA) LL_ADC_REG_SetDMATransfer(adcUsed, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

    // ADC calibration (optional, but recommended at power on)
    // ADC_ResetCalibration(adcUsed);                      // Reset previous calibration
    // while(ADC_GetResetCalibrationStatus(adcUsed));

    LL_ADC_StartCalibration(adcUsed);
    while(LL_ADC_IsCalibrationOnGoing(adcUsed)) ;

    LL_ADC_REG_StartConversionSWStart(adcUsed);
    // LL_ADC_INJ_StartConversionSWStart(adcUsed);
}

uint16_t ADC::getValue(uint8_t channel) const {

    if (channelsType == ResultStoreMode::Regular) {

        if (useDMA) 
            return dataStorage[channel];
        else
            return LL_ADC_REG_ReadConversionData12(adcUsed);
    }
    else {
        
        // LL_ADC_INJ_ReadConversionData32
        return LL_ADC_INJ_ReadConversionData12(adcUsed, mapChannel2Injected[channel]);
    }
}
