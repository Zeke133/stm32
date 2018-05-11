#include <adc.h>





ADC::ADC(
            uint32_t adcN,
            uint32_t clkDiv,                    // example: RCC_PCLK2_Div6
            enum ResultStoreMode chType,
            uint32_t chCnt,
            const uint8_t * chNumbers) {

    if (adcN == 1) {
        adcUsed = ADC1;
    }
    else {
        adcUsed = ADC2;
    }
    
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
                useDMA = chCnt > 1 ? 1 : 0;             // use DMA when more than 1 channel
            }
            else {
                channelsCnt = 1;                        // ADC2 have no DMA so just 1 channel can be stored
            }
        }
    }
    
    for(uint32_t i = 0; i < channelsCnt; i++) {
        channelsNumbers[i] = chNumbers[i];
    }

    init();
}

void ADC::init() {
    
    // set clock for ADC. MAX can be 14Mhz!
    RCC_ADCCLKConfig(clockDivider);
    // enable ADC system clock    
    RCC_APB2PeriphClockCmd((adcUsed == ADC1) ? RCC_APB2Periph_ADC1 : RCC_APB2Periph_ADC2, ENABLE);

    // DMA configuration
    if (useDMA) {

        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

        DMA_InitTypeDef DMA_InitStructure;
        DMA_InitStructure.DMA_BufferSize = channelsCnt;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)dataStorage;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&adcUsed->DR;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;
        DMA_Init(DMA1_Channel1, &DMA_InitStructure);
        DMA_Cmd(DMA1_Channel1 ,ENABLE) ;
    }

    // Config GPIO inputs of PortA & PortB
    {
        uint16_t pinsA = 0;
        uint16_t pinsB = 0;
        for(uint32_t i = 0; i < channelsCnt; i++) {

            uint32_t channel = channelsNumbers[i];

            if (channel < 8) {
                pinsA |= (1 << channel);
            }
            else if (channel == 8) {
                pinsB |= GPIO_Pin_0;
            }
            else if (channel == 9) {
                pinsB |= GPIO_Pin_1;
            }
        }
        // May be need another speed sestting - must be checked
        if (pinsA) GPIO_Init_My(GPIOA, pinsA, GPIO_Mode_AIN, GPIO_Speed_50MHz /*777*/);
        if (pinsB) GPIO_Init_My(GPIOB, pinsB, GPIO_Mode_AIN, GPIO_Speed_50MHz /*777*/);
    }

    // define ADC config
    {
        ADC_InitTypeDef ADC_InitStructure;
        ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                      // we work in continuous sampling mode
        ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                      // Configures the ADC to operate in independent or dual mode.
        ADC_InitStructure.ADC_ScanConvMode = channelsCnt > 1 ? ENABLE : DISABLE;// Scan (multichannels) or Single (one channel) mode.
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;     // Timer compare for example
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfChannel = channelsCnt;
        ADC_Init(adcUsed, &ADC_InitStructure);
    }

    if (channelsType == ResultStoreMode::Injected) {

        ADC_InjectedSequencerLengthConfig(adcUsed, channelsCnt);
        for(uint32_t i = 0; i < channelsCnt; i++) {
            
            ADC_InjectedChannelConfig(adcUsed, channelsNumbers[i], i+1, ADC_SampleTime_7Cycles5);
        }
        ADC_ExternalTrigInjectedConvConfig(adcUsed, ADC_ExternalTrigInjecConv_None);
    } else {

        for(uint32_t i = 0; i < channelsCnt; i++) {
            
            ADC_RegularChannelConfig(adcUsed, channelsNumbers[i], i+1, ADC_SampleTime_7Cycles5);
        }
    }
    
    // Enable Temperature sensor if used
    for(uint32_t i = 0; i < channelsCnt; i++)
    {
        if (channelsNumbers[i] == ADC_Channel_TempSensor) {
            ADC_TempSensorVrefintCmd(ENABLE);
        }
    }
        
    // Enable ADC
    ADC_Cmd(adcUsed, ENABLE);                           // enable ADC

    // Enable DMA
    if (useDMA) {
        ADC_DMACmd(adcUsed, ENABLE);
    }
 
    // ADC calibration (optional, but recommended at power on)
    ADC_ResetCalibration(adcUsed);                      // Reset previous calibration
    while(ADC_GetResetCalibrationStatus(adcUsed));
    ADC_StartCalibration(adcUsed);                      // Start new calibration (ADC must be off at that time)
    while(ADC_GetCalibrationStatus(adcUsed));

    // start conversion 777 need or not?
    ADC_Cmd(ADC1,ENABLE);                               // enable ADC
    
    if (channelsType == ResultStoreMode::Injected) {
        ADC_AutoInjectedConvCmd(adcUsed, ENABLE);
    }

    ADC_SoftwareStartConvCmd(adcUsed, ENABLE);          // start conversion (will be endless as we are in continuous mode)

}

uint32_t ADC::getValue(uint32_t channel) {

    if (channelsType == ResultStoreMode::Regular) {
        if (useDMA) {
            return dataStorage[channel];
        }
        else {
            return ADC_GetConversionValue(adcUsed);
        }
    }
    else {
        return ADC_GetInjectedConversionValue(adcUsed, mapChannel2Injected[channel]);
    }
    
    //temperature = (uint16_t)((V25-adc_value)/Avg_Slope+25);
}

