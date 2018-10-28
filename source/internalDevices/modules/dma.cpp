#include <dma.h>

// atribute for C compatibility
// pointers to callback functions to be executed on interrupts, for each channel
typedef void (*CallbackFunc)();
CallbackFunc callbacksOnIRQ[7];

void DMA1_Channel4_IRQHandler(void) {

    DMA_ClearITPendingBit(DMA1_IT_TC4);
    DMA_Cmd(DMA1_Channel4, DISABLE);
    callbacksOnIRQ[4]();
}
void DMA1_Channel6_IRQHandler(void) {

    DMA_ClearITPendingBit(DMA1_IT_TC6);
    DMA_Cmd(DMA1_Channel6, DISABLE);
    callbacksOnIRQ[6]();
}
void DMA1_Channel7_IRQHandler(void) {

    DMA_ClearITPendingBit(DMA1_IT_TC7);
    DMA_Cmd(DMA1_Channel7, DISABLE);
    callbacksOnIRQ[7]();                    // maybe needs for nullptr check !!! ??? 777
}

const DMA::ChannelMap DMA::channelsMap[(uint8_t)Device::LAST_ELEMENT] {
    {1, 1, 0},                          // ADC,
    {1, 2, 0},                          // SPI1_RX,
    {1, 3, 1},                          // SPI1_TX,
    {1, 4, 0},                          // SPI2/I2S2_RX,
    {1, 5, 1},                          // SPI2/I2S2_TX,
    {1, 2, 1},                          // USART3_TX,
    {1, 3, 0},                          // USART3_RX,
    {1, 4, 1},                          // USART1_TX,
    {1, 5, 0},                          // USART1_RX,
    {1, 6, 0},                          // USART2_RX,
    {1, 7, 1},                          // USART2_TX,
    {1, 4, 1},                          // I2C2_TX,
    {1, 5, 0},                          // I2C2_RX,
    {1, 6, 1},                          // I2C1_TX,
    {1, 7, 0},                          // I2C1_RX,
    {1, 2, 0},                          // TIM1_CH1,
    {1, 4, 0},                          // TIM1_CH4,
    {1, 4, 0},                          // TIM1_TRIG,
    {1, 4, 0},                          // TIM1_COM,
    {1, 5, 0},                          // TIM1_UP,
    {1, 6, 0},                          // TIM1_CH3,
    {1, 1, 0},                          // TIM2_CH3,
    {1, 2, 0},                          // TIM2_UP,
    {1, 5, 0},                          // TIM2_CH1,
    {1, 7, 0},                          // TIM2_CH2,
    {1, 7, 0},                          // TIM2_CH4,
    {1, 2, 0},                          // TIM3_CH3,
    {1, 3, 0},                          // TIM3_CH4,
    {1, 3, 0},                          // TIM3_UP,
    {1, 6, 0},                          // TIM3_CH1,
    {1, 6, 0},                          // TIM3_TRIG
    {1, 1, 0},                          // TIM4_CH1,
    {1, 4, 0},                          // TIM4_CH2,
    {1, 5, 0},                          // TIM4_CH3,
    {1, 7, 0},                          // TIM4_UP
                                        // ADC3,
    // {2, 1, 0}                        // SPI_I2S3_RX,
    // {2, 2, 1}                        // SPI_I2S3_TX,
    // {2, 3, 0}                        // UART4_RX,
    // {2, 5, 1}                        // UART4_TX,
                                        // SDIO
    // {2, 1, 0}                        // TIM5_CH4,
    // {2, 1, 0}                        // TIM5_TRIG,
    // {2, 2, 0}                        // TIM5_CH3,
    // {2, 2, 0}                        // TIM5_UP,
    // {2, 4, 0}                        // TIM5_CH2,
    // {2, 5, 0}                        // TIM5_CH1,
    // {2, 3, 0}                        // TIM6_UP_DAC_Channel1,
    // {2, 4, 0}                        // TIM7_UP_DAC_Channel2,
                                        // TIM8_CH3,
                                        // TIM8_UP,
                                        // TIM8_CH4,
                                        // TIM8_TRIG,
                                        // TIM8_COM,
                                        // TIM8_CH1,
                                        // TIM8_CH2
};

DMA::DMA(Device device) {

    deviceConnected = device;

    uint8_t dmaChannelNumber = channelsMap[static_cast<uint32_t>(device)].channelNumber;

    if (channelsMap[static_cast<uint32_t>(device)].controllerNumber == 1) {

        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
        switch (dmaChannelNumber)
        {
            case 1: dmaChannel = DMA1_Channel1;
                    dmaIrq = DMA1_Channel1_IRQn;
                break;
            case 2: dmaChannel = DMA1_Channel2;
                    dmaIrq = DMA1_Channel2_IRQn;
                break;
            case 3: dmaChannel = DMA1_Channel3;
                    dmaIrq = DMA1_Channel3_IRQn;
                break;
            case 4: dmaChannel = DMA1_Channel4;
                    dmaIrq = DMA1_Channel4_IRQn;
                break;
            case 5: dmaChannel = DMA1_Channel5;
                    dmaIrq = DMA1_Channel5_IRQn;
                break;
            case 6: dmaChannel = DMA1_Channel6;
                    dmaIrq = DMA1_Channel6_IRQn;
                break;
            case 7: dmaChannel = DMA1_Channel7;
                    dmaIrq = DMA1_Channel7_IRQn;
                break;
            default:
                break;
        }
    } else {

        // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
        // switch (dmaChannelNumber)
        // {
        //     case 1: dmaChannel = DMA2_Channel1;
        //             dmaIrq = DMA2_Channel1_IRQn;
        //         break;
        //     case 2: dmaChannel = DMA2_Channel2;
        //             dmaIrq = DMA2_Channel2_IRQn;
        //         break;
        //     case 3: dmaChannel = DMA2_Channel3;
        //             dmaIrq = DMA2_Channel3_IRQn;
        //         break;
        //     case 4: dmaChannel = DMA2_Channel4;
        //             dmaIrq = DMA2_Channel4_5_IRQn;
        //         break;
        //     case 5: dmaChannel = DMA2_Channel5;
        //             dmaIrq = DMA2_Channel4_5_IRQn;
        //         break;
        //     default:
        //         break;
        // }
    }
}

void DMA::setCallbackOnIrq(void (*func)(void)) {

    callbacksOnIRQ[channelsMap[(uint8_t)deviceConnected].channelNumber - 1] = func;
}

void DMA::turnOnCallback(void) {

    // Enables DMA interrupt on transmition complete
    DMA_ITConfig(dmaChannel, DMA_IT_TC, ENABLE);
    NVIC_EnableIRQ(dmaIrq);
}

void DMA::runDMA(void * destPtr, const uint8_t * data, uint32_t size) const {

    DMA_InitTypeDef DMA_InitStruct;

    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)destPtr;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)data;

    // if DMA put data to peripheral "output" flag value is 1
    if (channelsMap[(uint8_t)deviceConnected].inputOutput) {

        DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    } else {

        DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    }

    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;

    DMA_InitStruct.DMA_BufferSize = size;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(dmaChannel, &DMA_InitStruct);

    dmaChannel->CNDTR = size;       // exact size to transfer
    DMA_Cmd(dmaChannel, ENABLE);
}
