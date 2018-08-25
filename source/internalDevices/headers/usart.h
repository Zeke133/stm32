#ifndef _UART_H
#define _UART_H

extern "C" {
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_dma.h>
}

#include <IWriter.h>

#include <misc.h>
#include <gpio.h>


// IRQ handler, extern "C" function, because of c++ bad names
extern "C" {
    void USART1_IRQHandler(void);
    void USART2_IRQHandler(void);

    void DMA1_Channel4_IRQHandler(void);
    void DMA1_Channel7_IRQHandler(void);
}

// Class to work around USART
class Usart : IWriter {

    friend void USART1_IRQHandler(void);
    friend void USART2_IRQHandler(void);
    friend int main(void);

public:

    // --- SEND DATA
    // Fast methods use DMA and are not blocking!
    // so if called without a delay for delivery data in output buffer will be overwriten
    void putcFast(char byte);
    void putsFast(const char * string);
    void putsFast(const char * string, uint32_t len);
    // Usual blocking methods - are slow, without DMA
    void putc(char byte);
    void puts(const char *str);

    // --- RECEIVE DATA
    uint32_t getCount(void);
    uint8_t * getData(void);

    void clear(void);

    // --- SETUP
    void setUart(uint32_t bauld, uint16_t dataBits, uint16_t stopBits, uint16_t parity);

    // delete copy constructor and assignment operator
    Usart(const Usart&) = delete;
    Usart& operator=(const Usart&) = delete;

private:

    Usart(  int usartN,
                uint32_t bauld = 115200,
                uint16_t dataBits = USART_WordLength_8b,
                uint16_t stopBits = USART_StopBits_1,
                uint16_t parity = USART_Parity_No);

    void setNvic(uint8_t irqChannel);
    void setDMA(void);

    USART_TypeDef * usart;

    uint8_t outputBuffer[100];
    DMA_Channel_TypeDef * dmaChannel;

    uint8_t inputBuffer[100];
    uint8_t inputBufferCnt = 0;
    
};

#endif
