#ifndef _USART_H
#define _USART_H

// implements
#include <ITextOutput.h>
// using
extern "C" {
#include <stm32f1xx_ll_usart.h>
}
#include <gpio.h>
#include <dma.hpp>

// IRQ handler, extern "C" function, because of c++ bad names
extern "C" {
    void USART1_IRQHandler(void);
    void USART2_IRQHandler(void);
    void USART3_IRQHandler(void);
}

/*
USART driver API.
Implements ITextOutput interface.
Possibility of using loop and DMA based output methods.
*/
class Usart : public ITextOutput {

/// Interrupt handlers
friend void USART1_IRQHandler(void);
friend void USART2_IRQHandler(void);
friend void USART3_IRQHandler(void);

public:

    Usart(int usartN,
          DMA& txDMA,
          uint32_t bauld = 115200,
          uint32_t dataBits = LL_USART_DATAWIDTH_8B,
          uint32_t stopBits = LL_USART_STOPBITS_1,
          uint32_t parity = LL_USART_PARITY_NONE);

    // delete copy constructor and assignment operator
    Usart(const Usart&) = delete;
    Usart& operator=(const Usart&) = delete;

    // --- SEND DATA
    void putsBufferized(const char * string, uint32_t len = 0);     // repeat call will wait first data transmition complete
    void putc(char byte);
    void puts(const char *str);

    // --- RECEIVE DATA
    uint32_t getCount(void);
    uint8_t * getData(void);

    void clear(void);

    // --- SETUP
    void setUart(uint32_t bauld, uint32_t dataBits, uint32_t stopBits, uint32_t parity);

private:

    // void setNvic(uint8_t irqChannel);

    USART_TypeDef * usart;
    DMA& txDMA;

    uint8_t outputBuffer[100];
    uint8_t inputBuffer[100];
    uint8_t inputBufferCnt = 0;

    // static pointers for interrupt handlers
    static volatile uint8_t * buf1Ptr;
    static volatile uint8_t * buf2Ptr;
    static volatile uint8_t * buf3Ptr;
    static volatile uint8_t * buf1CntPtr;
    static volatile uint8_t * buf2CntPtr;
    static volatile uint8_t * buf3CntPtr;

    // race condition protection on DMA transmition
    static volatile uint8_t port1DmaTransmitionInProgress;  // flags indicatong transmition process
    static volatile uint8_t port2DmaTransmitionInProgress;
    static volatile uint8_t port3DmaTransmitionInProgress;
    static void callbackUsart1OnDmaIrq(void);               // callbacks on DMA transmition complete interrupt
    static void callbackUsart2OnDmaIrq(void);
    static void callbackUsart3OnDmaIrq(void);
    volatile uint8_t * dmaTransferActiveFlagPtr;            // pointer to flag used with current instance

};

#endif
