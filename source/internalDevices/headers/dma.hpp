/**
 * @file    dma.hpp
 * @author  Denis Homutovski
 * @version V0.9.1
 * @date    06-12-2018
 * @brief   DMA class header.
 * @details   Direct Memory Access controller driver for STM32F10x.
 * @pre       -
 * @bug       -
 * @warning   DMA2 - is not implemented, please add if needed.
 * @copyright GNU Public License.
 */

#ifndef _DMA_H
#define _DMA_H

// using
extern "C" {
#include <stm32f10x_rcc.h>
#include <stm32f10x_dma.h>
}

// IRQ handlers. Extern "C" macro is needed for correct link procedure.
extern "C" {
    void DMA1_Channel1_IRQHandler(void);
    void DMA1_Channel2_IRQHandler(void);
    void DMA1_Channel3_IRQHandler(void);
    void DMA1_Channel4_IRQHandler(void);
    void DMA1_Channel5_IRQHandler(void);
    void DMA1_Channel6_IRQHandler(void);
    void DMA1_Channel7_IRQHandler(void);
}

/**
 * Direct Memory Access controller driver class.
 * Instance represents DMA channel.
 * Attention! DMA2 - is not implemented, please add if needed.
 * Serviced device which will use DMA controller instance chosen in constructor.
 * Here is no any hardware resources sharing protection,
 * so you must not create more than one instance for each physical DMA channel.
 * This implementation uses interrupts and able to execute user-defined
 * callback functions with specified actions.
 */
class DMA {

// Interrupt handlers need access to 'callbacks'.
friend void DMA1_Channel1_IRQHandler(void);
friend void DMA1_Channel2_IRQHandler(void);
friend void DMA1_Channel3_IRQHandler(void);
friend void DMA1_Channel4_IRQHandler(void);
friend void DMA1_Channel5_IRQHandler(void);
friend void DMA1_Channel6_IRQHandler(void);
friend void DMA1_Channel7_IRQHandler(void);

public:

    /**
     * Serviced devices available for work with DMA controller.
     * In another words - device which use exact DMA controller channel.
     */
    enum class ServicedDevice : uint8_t {
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

    DMA(ServicedDevice device);
    DMA(const DMA&) = delete;             /**< Delete copy constructor. */
    DMA& operator=(const DMA&) = delete;  /**< Delete assignment operator. */

    typedef void (*CallbackFunc)();       /**< Callback function type. */
    
    void turnOnCallback(void);
    void setCallback(CallbackFunc func);
    void runDataTransfer(void * perephPtr, const uint8_t * memoryPtr, uint32_t size) const;

private:

    /**
     * Configuration of devices serviced with DMA.
     * Needed to configure driver instance accordingly.
     */
    struct ServicedDeviceConfig {

        uint32_t controllerNumber : 2;
        /**< DMA controller: 1 or 2 */

        uint32_t channelNumber : 3;
        /**< Controller channel number: 1 ... 7 */

        uint32_t inputOutput : 1;
        /**< Data transmission direction:
         * - 0 - channel used for data input Rx
         * - 1 - channel used for data output Tx
         */
    };
    
    static const ServicedDeviceConfig descriptions[(uint8_t)ServicedDevice::LAST_ELEMENT];
    static CallbackFunc callbacks[7];
    
    ServicedDevice servicedDevice;      /**< Device index in 'ServicedDevice' enum. */
    DMA_Channel_TypeDef * dmaChannel;   /**< Configured DMA channel. */
    IRQn_Type dmaIrq;                   /**< Configured interrupt. */

};

#endif
