/**
 * @file    dma.cpp
 * @author  Denis Homutovski
 * @version V0.9.1
 * @date    06-12-2018
 * @brief   DMA class source
 */
  
#include <dma.hpp>

/**
 * Callback functions pointers being executed on interrupts
 * for each DMA channel.
 */
DMA::CallbackFunc DMA::callbacks[7] = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

void DMA1_Channel1_IRQHandler(void) {

    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    if (DMA::callbacks[0] != nullptr) DMA::callbacks[0]();
}
void DMA1_Channel2_IRQHandler(void) {

    LL_DMA_ClearFlag_TC2(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    if (DMA::callbacks[1] != nullptr) DMA::callbacks[1]();
}
void DMA1_Channel3_IRQHandler(void) {

    LL_DMA_ClearFlag_TC3(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
    if (DMA::callbacks[2] != nullptr) DMA::callbacks[2]();
}
void DMA1_Channel4_IRQHandler(void) {

    LL_DMA_ClearFlag_TC4(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
    if (DMA::callbacks[3] != nullptr) DMA::callbacks[3]();
}
void DMA1_Channel5_IRQHandler(void) {

    LL_DMA_ClearFlag_TC5(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
    if (DMA::callbacks[4] != nullptr) DMA::callbacks[4]();
}
void DMA1_Channel6_IRQHandler(void) {

    LL_DMA_ClearFlag_TC6(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_6);
    if (DMA::callbacks[5] != nullptr) DMA::callbacks[5]();
}
void DMA1_Channel7_IRQHandler(void) {

    LL_DMA_ClearFlag_TC7(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_7);
    if (DMA::callbacks[6] != nullptr) DMA::callbacks[6]();
}

/**
 * Table of serviced devices descriptors.
 * Maps each 'enum DMA::ServicedDevice' instance
 * to 'ServicedDeviceConfig'.
 */
const DMA::ServicedDeviceConfig DMA::descriptions[(uint8_t)ServicedDevice::LAST_ELEMENT] {
    {1, 1, 0},                          /// ADC,
    {1, 2, 0},                          /// SPI1_RX,
    {1, 3, 1},                          /// SPI1_TX,
    {1, 4, 0},                          /// SPI2/I2S2_RX,
    {1, 5, 1},                          /// SPI2/I2S2_TX,
    {1, 2, 1},                          /// USART3_TX,
    {1, 3, 0},                          /// USART3_RX,
    {1, 4, 1},                          /// USART1_TX,
    {1, 5, 0},                          /// USART1_RX,
    {1, 6, 0},                          /// USART2_RX,
    {1, 7, 1},                          /// USART2_TX,
    {1, 4, 1},                          /// I2C2_TX,
    {1, 5, 0},                          /// I2C2_RX,
    {1, 6, 1},                          /// I2C1_TX,
    {1, 7, 0},                          /// I2C1_RX,
    {1, 2, 0},                          /// TIM1_CH1,
    {1, 4, 0},                          /// TIM1_CH4,
    {1, 4, 0},                          /// TIM1_TRIG,
    {1, 4, 0},                          /// TIM1_COM,
    {1, 5, 0},                          /// TIM1_UP,
    {1, 6, 0},                          /// TIM1_CH3,
    {1, 1, 0},                          /// TIM2_CH3,
    {1, 2, 0},                          /// TIM2_UP,
    {1, 5, 0},                          /// TIM2_CH1,
    {1, 7, 0},                          /// TIM2_CH2,
    {1, 7, 0},                          /// TIM2_CH4,
    {1, 2, 0},                          /// TIM3_CH3,
    {1, 3, 0},                          /// TIM3_CH4,
    {1, 3, 0},                          /// TIM3_UP,
    {1, 6, 0},                          /// TIM3_CH1,
    {1, 6, 0},                          /// TIM3_TRIG
    {1, 1, 0},                          /// TIM4_CH1,
    {1, 4, 0},                          /// TIM4_CH2,
    {1, 5, 0},                          /// TIM4_CH3,
    {1, 7, 0},                          /// TIM4_UP
                                        /// ADC3,
    // {2, 1, 0}                        /// SPI_I2S3_RX,
    // {2, 2, 1}                        /// SPI_I2S3_TX,
    // {2, 3, 0}                        /// UART4_RX,
    // {2, 5, 1}                        /// UART4_TX,
                                        /// SDIO
    // {2, 1, 0}                        /// TIM5_CH4,
    // {2, 1, 0}                        /// TIM5_TRIG,
    // {2, 2, 0}                        /// TIM5_CH3,
    // {2, 2, 0}                        /// TIM5_UP,
    // {2, 4, 0}                        /// TIM5_CH2,
    // {2, 5, 0}                        /// TIM5_CH1,
    // {2, 3, 0}                        /// TIM6_UP_DAC_Channel1,
    // {2, 4, 0}                        /// TIM7_UP_DAC_Channel2,
                                        /// TIM8_CH3,
                                        /// TIM8_UP,
                                        /// TIM8_CH4,
                                        /// TIM8_TRIG,
                                        /// TIM8_COM,
                                        /// TIM8_CH1,
                                        /// TIM8_CH2
};

/**
 * @brief  Driver constructor.
 * @param  device: Device which going to use this DMA driver instance.
 */
DMA::DMA(ServicedDevice device)
    : servicedDevice(device) {

    ServicedDeviceConfig description = descriptions[(uint8_t)(servicedDevice)];

    if (description.controllerNumber == 1) {

        dma = DMA1;
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
        switch (description.channelNumber)
        {
            case 1: dmaChannel = LL_DMA_CHANNEL_1;
                    dmaIrq = DMA1_Channel1_IRQn;
                break;
            case 2: dmaChannel = LL_DMA_CHANNEL_2;
                    dmaIrq = DMA1_Channel2_IRQn;
                break;
            case 3: dmaChannel = LL_DMA_CHANNEL_3;
                    dmaIrq = DMA1_Channel3_IRQn;
                break;
            case 4: dmaChannel = LL_DMA_CHANNEL_4;
                    dmaIrq = DMA1_Channel4_IRQn;
                break;
            case 5: dmaChannel = LL_DMA_CHANNEL_5;
                    dmaIrq = DMA1_Channel5_IRQn;
                break;
            case 6: dmaChannel = LL_DMA_CHANNEL_6;
                    dmaIrq = DMA1_Channel6_IRQn;
                break;
            case 7: dmaChannel = LL_DMA_CHANNEL_7;
                    dmaIrq = DMA1_Channel7_IRQn;
                break;
            default:
                break;
        }
    } else {
        
        // assert ??? NOT IMPLEMENTED
        // dma = DMA2;
        // LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
        // switch (dmaChannelNumber)
        // {
        //     case 1: dmaChannel = LL_DMA_CHANNEL_1;
        //             dmaIrq = DMA2_Channel1_IRQn;
        //         break;
        //     case 2: dmaChannel = LL_DMA_CHANNEL_2;
        //             dmaIrq = DMA2_Channel2_IRQn;
        //         break;
        //     case 3: dmaChannel = LL_DMA_CHANNEL_3;
        //             dmaIrq = DMA2_Channel3_IRQn;
        //         break;
        //     case 4: dmaChannel = LL_DMA_CHANNEL_4;
        //             dmaIrq = DMA2_Channel4_5_IRQn;
        //         break;
        //     case 5: dmaChannel = LL_DMA_CHANNEL_5;
        //             dmaIrq = DMA2_Channel4_5_IRQn;
        //         break;
        //     default:
        //         break;
        // }
    }
}

/**
 * @brief  Set callback function on transmition complete interrupt.
 * @param  func: Pointer to user callback function.
 * @retval None
 */
void DMA::setCallback(CallbackFunc func) {

    callbacks[descriptions[(uint8_t)(servicedDevice)].channelNumber - 1] = func;
}

/**
 * @brief  Turns ON interrupt on transmission complete.
 * @retval None
 */
void DMA::turnOnCallback(void) {

    // Enables DMA interrupt on transmition complete
    LL_DMA_EnableIT_TC(dma, dmaChannel);
    NVIC_EnableIRQ(dmaIrq);
}

/**
 * @brief  Run data transmission.
 * @param  perephPtr: Pointer to perepheral device data register.
 * @param  memoryPtr: Pointer to memory buffer.
 * @param  size: Size of data to be transmited.
 * @retval None
 */
void DMA::runDataTransfer(void * perephPtr, const uint8_t * memoryPtr, uint32_t size) const {
    
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    // if DMA transfers data to peripheral - "inputOutput" flag value is 1
    uint32_t direction = descriptions[(uint8_t)servicedDevice].inputOutput
                                            ?
                                            LL_DMA_DIRECTION_MEMORY_TO_PERIPH :
                                            LL_DMA_DIRECTION_PERIPH_TO_MEMORY;

    LL_DMA_InitTypeDef params;

    params.PeriphOrM2MSrcAddress = (uint32_t)perephPtr;
    params.MemoryOrM2MDstAddress = (uint32_t)memoryPtr;

    params.Direction = direction;

    params.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    params.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;

    params.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
    params.MemoryOrM2MDstDataSize = LL_DMA_PDATAALIGN_BYTE;

    params.Mode = LL_DMA_MODE_NORMAL;   // LL_DMA_MODE_CIRCULAR
    params.Priority = LL_DMA_PRIORITY_LOW;

    params.NbData = size;
    
    LL_DMA_Init(dma, dmaChannel, &params);
    LL_DMA_EnableChannel(dma, dmaChannel);
}
