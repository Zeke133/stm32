#ifndef UART_H
#define UART_H


#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>

#include <misc.h>
#include <gpio.h>


// IRQ handler, extern "C" function, because of c++ bad names
extern "C" {
    void USART1_IRQHandler(void);
}

// Class to work around USART
class USART {

    friend void USART1_IRQHandler(void);
    friend int main(void);

public:

    void send(char byte);
    void send(const char * string);

    uint32_t getCount(void);
    uint8_t* getData(void);

    void clear(void);

    void set(uint32_t bauld, uint16_t dataBits, uint16_t stopBits, uint16_t parity);

    USART& operator<<(char byte)
    {
        send(byte);
        return *this;
    }
    USART& operator<<(const char * string)
    {
        send(string);
        return *this;
    }

    // delete copy constructor and assignment operator
    USART(const USART&) = delete;
    USART& operator=(const USART&) = delete;

private:

    USART(  int usartN,
                uint32_t bauld = 115200,
                uint16_t dataBits = USART_WordLength_8b,
                uint16_t stopBits = USART_StopBits_1,
                uint16_t parity = USART_Parity_No);

    void push(uint8_t byte);

    USART_TypeDef* usart;

    uint8_t buffer[100];
    uint8_t bufferCnt = 0;

};

extern USART* usart1;

#endif
