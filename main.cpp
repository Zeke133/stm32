#include <stm32f10x.h>

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>

#include <misc.h>

#include <string.h>

#include "delay.h"
#include "LiquidCrystal_I2C.h"
#include "DS_1wire.h"

#define RX_BUF_SIZE 80
char RX_FLAG_END_LINE = 0;
int RXi;
char RXc;
char RX_BUF[RX_BUF_SIZE] = {'\0'};
char buffer[80] = {'\0'};

void clear_RXBuffer(void) {
    for (RXi=0; RXi<RX_BUF_SIZE; RXi++)
        RX_BUF[RXi] = '\0';
    RXi = 0;
}

uint8_t bell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
uint8_t note[8]  = {0x2,0x3,0x2,0xe,0x1e,0xc,0x0};
uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
uint8_t duck[8]  = {0x0,0xc,0x1d,0xf,0xf,0x6,0x0};
uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
uint8_t cross[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
uint8_t retarrow[8] = {0x1,0x1,0x5,0x9,0x1f,0x8,0x4};

uint8_t habr1[8] = {0x0,0x0,0x2,0x3,0xd,0x7,0x7,0xf};
uint8_t habr2[8] = {0x0,0x1,0x7,0x1b,0x1e,0xf,0x1f, 0x1f};
uint8_t habr3[8] = {0x10,0x9,0x17,0x1c,0xc,0x1f,0xd, 0x1d};
uint8_t habr4[8] = {0x0,0x10,0x0,0x0,0x10,0x0,0x14,0x18};

uint8_t habr5[8] = {0x15,0xd,0xb,0xd,0x5,0x3,0x5, 0x6};
uint8_t habr6[8] = {0x1f,0x1f,0x1f,0x1f,0x1f,0x7,0x19, 0xe};
uint8_t habr7[8] = {0x1d,0x1d,0x1d,0x1a,0x1b, 0xd,0x19, 0x8};
uint8_t habr8[8] = {0x18,0x10,0x2,0x1c,0x1f,0x0,0x0,0x0};

uint8_t habr9[8] =  {0x1, 0x1,0x3, 0x3,0x1,0x0,0x0, 0x0};
uint8_t habr10[8] = {0x18,0x8,0x18,0x4,0x8,0x8,0x10,0x0};
uint8_t habr11[8] = {0x18,0x10,0x10,0x0,0x0,0x0,0x0, 0x0};

void SetSysClockTo72(void)
{
    ErrorStatus HSEStartUpStatus;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
    /* Системный RESET RCC (делать не обязательно, но полезно на этапе отладки) */
    RCC_DeInit();

    /* Включаем HSE (внешний кварц) */
    RCC_HSEConfig( RCC_HSE_ON);

    /* Ждем пока HSE будет готов */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    /* Если с HSE все в порядке */
    if (HSEStartUpStatus == SUCCESS)
    {
    /* Следующие две команды касаются исключительно работы с FLASH.
    Если вы не собираетесь использовать в своей программе функций работы с Flash,
    FLASH_PrefetchBufferCmd( ) та FLASH_SetLatency( ) можно закомментировать */

        /* Включаем Prefetch Buffer */
        //FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);

        /* FLASH Latency.
    Рекомендовано устанавливать:
        FLASH_Latency_0 - 0 < SYSCLK<= 24 MHz
        FLASH_Latency_1 - 24 MHz < SYSCLK <= 48 MHz
        FLASH_Latency_2 - 48 MHz < SYSCLK <= 72 MHz */
        //FLASH_SetLatency( FLASH_Latency_2);

        /* HCLK = SYSCLK */ /* Смотри на схеме AHB Prescaler. Частота не делится (RCC_SYSCLK_Div1) */
        RCC_HCLKConfig( RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */ /* Смотри на схеме APB2 Prescaler. Частота не делится (RCC_HCLK_Div1)  */
        RCC_PCLK2Config( RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */ /* Смотри на схеме APB1 Prescaler. Частота делится на 2 (RCC_HCLK_Div2)
        потому что на выходе APB1 должно быть не более 36МГц (смотри схему) */
        RCC_PCLK1Config( RCC_HCLK_Div2);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        /* Указываем PLL от куда брать частоту (RCC_PLLSource_HSE_Div1) и на сколько ее умножать (RCC_PLLMul_9) */
        /* PLL может брать частоту с кварца как есть (RCC_PLLSource_HSE_Div1) или поделенную на 2 (RCC_PLLSource_HSE_Div2). Смотри схему */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

        /* Включаем PLL */
        RCC_PLLCmd( ENABLE);

        /* Ждем пока PLL будет готов */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* Переключаем системное тактирование на PLL */
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);

        /* Ждем пока переключиться */
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    { /* Проблемы с HSE. Тут можно написать свой код, если надо что-то делать когда микроконтроллер не смог перейти на работу с внешним кварцом */

        /* Пока тут заглушка - вечный цикл*/
        while (1)
        {
        }
    }
}
void usart_init(void)
{
    /* Enable USART1 and GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* NVIC Configuration */
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure the GPIOs */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure the USART1 */
    USART_InitTypeDef USART_InitStructure;

    /* USART1 configuration ------------------------------------------------------*/
    /* USART1 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle
        - USART LastBit: The clock pulse of the last data bit is not output to
            the SCLK pin
     */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);

    /* Enable the USART1 Receive interrupt: this interrupt is generated when the
        USART1 receive data register is not empty */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
void USART1_IRQHandler(void)
{
    if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET)
    {
            RXc = USART_ReceiveData(USART1);
            RX_BUF[RXi] = RXc;
            RXi++;

            if (RXc != 13) {
                if (RXi > RX_BUF_SIZE-1) {
                    clear_RXBuffer();
                }
            }
            else {
                RX_FLAG_END_LINE = 1;
            }

            //Echo
            USART_SendData(USART1, RXc);
    }
}
void USARTSendByte(const char byte)
{
    USART_SendData(USART1, byte);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {
    }
}

void USARTSend(const char *pucBuffer)
{
    while (*pucBuffer)
    {
        USART_SendData(USART1, *pucBuffer++);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        {
        }
    }
}

unsigned char * itoa_2(int val, int base)
{
	const char * symbols = "0123456789ABCDEF";
	static unsigned char buf[10];
	int i;

	for(i = 0; i >= 8; i++)
	{
		buf[i] = '0';
	}
	buf[9] = 0;

	for(i = 8; i >= 0; i--)
	{
		assert_param(base < strlen(symbols));
		buf[i] =  symbols[val % base];
		val /= base;
		if(!val) break;
	}
	return &buf[i];
}

unsigned char * itoa(int val)
{
	static unsigned char ret[4];

	ret[3] = 0;
	ret[2] = val%10 + 0x30;
	val /= 10;
	ret[1] = val%10 + 0x30;
	val /= 10;
	ret[0] = val%10 + 0x30;

	return &ret[0];
}

int main(void) {

	SetSysClockTo72();

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init( GPIOC , &GPIO_InitStructure);

    // Initialize USART
    usart_init();
    USARTSend(" Hello.\r\nUSART1 is ready.\r\n");

    //
    LCDI2C_init(0x27,16,2);
    // ------- Quick 3 blinks of backlight  -------------
    int i;
    for( i = 0; i< 3; i++)
    {
      LCDI2C_backlight();
      Delay(250);
      LCDI2C_noBacklight();
      Delay(250);
    }
    LCDI2C_backlight(); // finish with backlight on


    Delay(2000);
    LCDI2C_clear();

    LCDI2C_createChar(0, habr1);

    LCDI2C_clear();
    LCDI2C_write_String("Hello Habrahabr");
    LCDI2C_setCursor(12,0);
    LCDI2C_write(0);

    Delay(2000);
    LCDI2C_clear();

    USARTSend("\xd\xa Initialisation of 1-Wire...");
    DS_1Wire_controller ds_one_wire(GPIOA, GPIO_Pin_8);
    unsigned char rd[8] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
    if (ds_one_wire.GetCodes(0x33, rd)) {
    	USARTSend("\xd\xa Get succ");
    } else {
    	USARTSend("\xd\xa Get err");
    }

	USARTSend("\xd\xa read[8]:");
	char pr[2] = "0";
	for(int i=0; i<8; i++) {
		pr[0] = rd[i] + 0x30;
		USARTSend(pr);
	}

    while (1)
        {
            if (RX_FLAG_END_LINE == 1) {
                // Reset END_LINE Flag
                RX_FLAG_END_LINE = 0;

                USARTSend("\r\nI has received a line:\r\n");
                USARTSend(RX_BUF);
                USARTSend("\r\n");

                LCDI2C_clear();
                LCDI2C_write_String(RX_BUF);

                if (strncmp(strupr(RX_BUF), "ON\r", 3) == 0) {
                    USARTSend("\r\nTHIS IS A COMMAND \"ON\"!!!\r\n");
                    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
                }

                if (strncmp(strupr(RX_BUF), "OFF\r", 4) == 0) {
                    USARTSend("\r\nTHIS IS A COMMAND \"OFF\"!!!\r\n");
                    GPIO_SetBits(GPIOC, GPIO_Pin_13);
                }

                clear_RXBuffer();
            }
        }

    return 0;
}




