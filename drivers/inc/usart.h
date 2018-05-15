#ifndef _UART_H
#define _UART_H


#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_dma.h>

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
class USART {

    friend void USART1_IRQHandler(void);
    friend void USART2_IRQHandler(void);
    friend int main(void);

public:

    void send(uint8_t byte);
    void send(const uint8_t * string);
    void send(const uint8_t * string, uint32_t len);
    void sendBlocking(char byte);
    void sendBlocking(const char *str);

    uint32_t getCount(void);
    uint8_t* getData(void);

    void clear(void);

    void setUart(uint32_t bauld, uint16_t dataBits, uint16_t stopBits, uint16_t parity);

    inline USART& operator<<(char byte) {
        send(static_cast<uint8_t>(byte));
        return *this;
    };
    inline USART& operator<<(const char * string) {
        send((uint8_t*)string);
        return *this;
    };

    // delete copy constructor and assignment operator
    USART(const USART&) = delete;
    USART& operator=(const USART&) = delete;

private:

    USART(  int usartN,
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
