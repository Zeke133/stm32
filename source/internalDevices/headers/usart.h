#ifndef _USART_H
#define _USART_H

// implements
#include <ITextOutput.h>
// using
extern "C" {
#include <stm32f10x_usart.h>
}
#include <misc.h>
#include <gpio.h>
#include <dma.h>

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

public:

    Usart(int usartN,
          DMA& dma,
          uint32_t bauld = 115200,
          uint16_t dataBits = USART_WordLength_8b,
          uint16_t stopBits = USART_StopBits_1,
          uint16_t parity = USART_Parity_No);

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
    void setUart(uint32_t bauld, uint16_t dataBits, uint16_t stopBits, uint16_t parity);

private:

    void setNvic(uint8_t irqChannel);

    USART_TypeDef * usart;
    DMA& dmaController;

    uint8_t outputBuffer[100];
    uint8_t inputBuffer[100];
    uint8_t inputBufferCnt = 0;

    // race condition protection on DMA transmition
    static volatile uint8_t port1DmaTransmitionInProgress;  // flags indicatong transmition process
    static volatile uint8_t port2DmaTransmitionInProgress;
    static volatile uint8_t port3DmaTransmitionInProgress;
    static void callbackUsart1OnDmaIrq(void);               // callbacks on DMA transmition complete interrupt
    static void callbackUsart2OnDmaIrq(void);
    static void callbackUsart3OnDmaIrq(void);
    volatile uint8_t * dmaTransmitionInProgressFlagPtr;     // pointer to flag used with current instance

};

#endif
