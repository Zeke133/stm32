#ifndef _DMA_H
#define _DMA_H

// using
extern "C" {
#include <stm32f10x_rcc.h>
#include <stm32f10x_dma.h>
}

// IRQ handler, extern "C" function, because of c++ bad names
extern "C" {
    void DMA1_Channel1_IRQHandler(void);
    void DMA1_Channel2_IRQHandler(void);
    void DMA1_Channel3_IRQHandler(void);
    void DMA1_Channel4_IRQHandler(void);
    void DMA1_Channel5_IRQHandler(void);
    void DMA1_Channel6_IRQHandler(void);
    void DMA1_Channel7_IRQHandler(void);
}

/*
Direct Memory Access controller driver.
! DMA2 - is not implemented, please add if needed.
Device is chosed in constructor. No device usage protection,
so you can not use same device for different instanses.
Use interrupts and execute user-defined callback functions for specific actions.
*/
class DMA {

public:

    enum class Device : uint8_t {
        // DMA1 devices
        ADC_1,

        SPI1_RX,
        SPI1_TX,

        SPI2_I2S2_RX,
        SPI2_I2S2_TX,

        USART3_TX,
        USART3_RX,

        USART1_TX,
        USART1_RX,

        USART2_RX,
        USART2_TX,

        I2C2_TX,
        I2C2_RX,
        I2C1_TX,
        I2C1_RX,

        TIM1_CH1,
        TIM1_CH4,
        TIM1_TRIG,
        TIM1_COM,
        TIM1_UP,
        TIM1_CH3,

        TIM2_CH3,
        TIM2_UP,
        TIM2_CH1,
        TIM2_CH2,
        TIM2_CH4,

        TIM3_CH3,
        TIM3_CH4,
        TIM3_UP,
        TIM3_CH1,
        TIM3_TRIG,

        TIM4_CH1,
        TIM4_CH2,
        TIM4_CH3,
        TIM4_UP,

        // DMA2 devices
        // ADC3,

        SPI_I2S3_RX,
        SPI_I2S3_TX,

        UART4_RX,
        UART4_TX,

        // SDIO,

        TIM5_CH4,
        TIM5_TRIG,
        TIM5_CH3,
        TIM5_UP,
        TIM5_CH2,
        TIM5_CH1,

        TIM6_UP_DAC_Channel1,

        TIM7_UP_DAC_Channel2,

        // TIM8_CH3,
        // TIM8_UP,
        // TIM8_CH4,
        // TIM8_TRIG,
        // TIM8_COM,
        // TIM8_CH1,
        // TIM8_CH2

        // end
        LAST_ELEMENT
    };

    DMA(Device device);

    // delete copy constructor and assignment operator
    DMA(const DMA&) = delete;
    DMA& operator=(const DMA&) = delete;

    // interface
    void setCallbackOnIrq(void (*func)(void));  // set function for callback on interrupt
    void turnOnCallback(void);                  // turns ON interrupt on DMA transmition complete

    void runDMA(void * destPtr, const uint8_t * data, uint32_t size) const;

private:

    struct ChannelMap {
        uint32_t controllerNumber : 2;
        uint32_t channelNumber : 3;
        uint32_t inputOutput : 1;           // 0 - channel used for data input Rx; 1 - output Tx
    };
    static const ChannelMap channelsMap[(uint8_t)Device::LAST_ELEMENT];

    Device deviceConnected;
    DMA_Channel_TypeDef * dmaChannel;
    IRQn_Type dmaIrq;

};

#endif
