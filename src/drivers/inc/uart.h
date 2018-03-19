#ifndef UART_H
#define UART_H


#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>

#include <misc.h>


extern uint16_t sym;
#include <led.h>
extern LED * ledGlobalPtr;


void usart_init(void);
void USART1_IRQHandler(void);
void USARTSendByte(const char byte);
void USARTSend(const char *pucBuffer);

void clear_RXBuffer(void);


#endif
