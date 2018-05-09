#include <usart.h>


// refactor to DMA using in interruption handlers


USART* usart1;


// Interrypt handler
void USART1_IRQHandler(void)
{
    if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET) {

        usart1->push(USART_ReceiveData(USART1));
    }
}

// Class implementation
USART::USART(int usartN, uint32_t bauld, uint16_t dataBits, uint16_t stopBits, uint16_t parity) {

    uint8_t irqChannel;
    GPIO_TypeDef * port;
    uint16_t txPin, rxPin;

    if (usartN == 1) {

        /* Enable USART1 and GPIOA clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

        irqChannel = USART1_IRQn;
        port = GPIOA;
        txPin = GPIO_Pin_9;
        rxPin = GPIO_Pin_10;
        usart = USART1;

    }   // add configuration for other USARTS later
    else return;
    
    /* NVIC Configuration */
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = irqChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure the GPIOs */
    /* Configure Tx as alternate function push-pull */
    GPIO_Init_My(port, txPin, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    /* Configure Rx as input floating */
    GPIO_Init_My(port, rxPin, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);

    set(bauld, dataBits, stopBits, parity);

    usart1 = this;
}

void USART::set(uint32_t bauld, uint16_t dataBits, uint16_t stopBits, uint16_t parity) {

    /* Configure the usart */
    USART_InitTypeDef USART_InitStructure;

    /* USART configured as follow:
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle
        - USART LastBit: The clock pulse of the last data bit is not output to
            the SCLK pin
     */
    USART_InitStructure.USART_BaudRate = bauld;
    USART_InitStructure.USART_WordLength = dataBits;
    USART_InitStructure.USART_StopBits = stopBits;
    USART_InitStructure.USART_Parity = parity;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(usart, &USART_InitStructure);

    /* Enable usart */
    USART_Cmd(usart, ENABLE);

    /* Enable the usart Receive interrupt: this interrupt is generated when the
        usart receive data register is not empty */
    USART_ITConfig(usart, USART_IT_RXNE, ENABLE);
}

void USART::send(char byte) {

    USART_SendData(usart, byte);
    while(USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET) {}
}

void USART::send(const char *str) {

    while (*str) {
        USART_SendData(usart, *str++);
        while(USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET) {}
    }
}

void USART::push(uint8_t byte) {

    if (bufferCnt < sizeof buffer) {

        buffer[bufferCnt++] = byte;
    }
}

void USART::clear() {

    bufferCnt = 0;
}

uint32_t USART::getCount() {

    return bufferCnt;
}

uint8_t* USART::getData() {

    return buffer;
}



