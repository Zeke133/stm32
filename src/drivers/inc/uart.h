#ifndef UART_H
#define UART_H


#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>

#include <misc.h>


#define RX_BUF_SIZE 80
extern char RX_FLAG_END_LINE;
extern char RX_BUF[RX_BUF_SIZE];

void usart_init(void);
void USART1_IRQHandler(void);
void USARTSendByte(const char byte);
void USARTSend(const char *pucBuffer);

void clear_RXBuffer(void);


#endif
