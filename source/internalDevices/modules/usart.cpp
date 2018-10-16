#include <usart.h>

static uint8_t * uart1bufPtr;
static uint8_t * uart2bufPtr;
static uint8_t * uart1bufCntPtr;
static uint8_t * uart2bufCntPtr;

// Interrypt handler
void USART1_IRQHandler(void) {

    if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET) {
        if (*uart1bufCntPtr < 100) uart1bufPtr[(*uart1bufCntPtr)++] = USART_ReceiveData(USART1);
        else USART_ReceiveData(USART1);
    }
}
void USART2_IRQHandler(void) {

    if ((USART2->SR & USART_FLAG_RXNE) != (u16)RESET) {
        if (*uart2bufCntPtr < 100) uart2bufPtr[(*uart2bufCntPtr)++] = USART_ReceiveData(USART2);
        else USART_ReceiveData(USART2);
    }
}
void DMA1_Channel4_IRQHandler(void) {

    DMA_ClearITPendingBit(DMA1_IT_TC4);
    DMA_Cmd(DMA1_Channel4, DISABLE);
}
void DMA1_Channel7_IRQHandler(void) {

    DMA_ClearITPendingBit(DMA1_IT_TC7);
    DMA_Cmd(DMA1_Channel7, DISABLE);
}

// Class implementation
Usart::Usart(int usartN, uint32_t bauld, uint16_t dataBits, uint16_t stopBits, uint16_t parity) {

    uint8_t irqChannel;
    GPIO_TypeDef * port;
    uint16_t txPin, rxPin;
    IRQn_Type dmaIrq;

    if (usartN == 1) {
        uart1bufPtr = inputBuffer;
        uart1bufCntPtr = &inputBufferCnt;

        /* Enable USART1 and GPIOA clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
        // DMA enable
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        irqChannel = USART1_IRQn;
        port = GPIOA;
        txPin = GPIO_Pin_9;
        rxPin = GPIO_Pin_10;
        usart = USART1;
        dmaChannel = DMA1_Channel4;
        dmaIrq = DMA1_Channel4_IRQn;
    }
    else /*if (usartN == 2)*/ {
        uart2bufPtr = inputBuffer;
        uart2bufCntPtr = &inputBufferCnt;

        /* Enable USART1 and GPIOA clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        // DMA enable
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        irqChannel = USART2_IRQn;
        port = GPIOA;
        txPin = GPIO_Pin_2;
        rxPin = GPIO_Pin_3;
        usart = USART2;
        dmaChannel = DMA1_Channel7;
        dmaIrq = DMA1_Channel7_IRQn;
    }

    /* DMA for Transmiting*/
    setDMA();
    
    // Set UART interupt
    setNvic(irqChannel);    // what for??? global usart irq enable

    /* Configure the GPIOs */
    /* Configure Tx as alternate function push-pull */
    GPIO_Init_My(port, txPin, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    /* Configure Rx as input floating */
    GPIO_Init_My(port, rxPin, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);

    // Configure UART
    setUart(bauld, dataBits, stopBits, parity);
    
    /* Enable usart */
    USART_Cmd(usart, ENABLE);
    USART_DMACmd(usart, USART_DMAReq_Tx, ENABLE);

    DMA_ITConfig(dmaChannel, DMA_IT_TC, ENABLE);
    NVIC_EnableIRQ(dmaIrq);

    /* Enable the usart Receive interrupt: this interrupt is generated when the
        usart receive data register is not empty */
    USART_ITConfig(usart, USART_IT_RXNE, ENABLE);
}

void Usart::setUart(uint32_t bauld, uint16_t dataBits, uint16_t stopBits, uint16_t parity) {

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

}

void Usart::setDMA(void) {

    DMA_InitTypeDef DMA_InitStruct;

    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(usart->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&outputBuffer[0];
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = sizeof(outputBuffer);
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(dmaChannel, &DMA_InitStruct);
}

void Usart::setNvic(uint8_t irqChannel) {

    /* NVIC Configuration: Enable the USARTx Interrupt */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = irqChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void Usart::putc(char byte) {

    USART_SendData(usart, byte);
    while(USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET) {}
}

void Usart::puts(const char *str) {

    while (*str) {
        USART_SendData(usart, *str++);
        while(USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET) {}
    }
}

void Usart::putcFast(char byte) {

    outputBuffer[0] = byte;
    /* Restart DMA Channel*/
    DMA_Cmd(dmaChannel, DISABLE);
    dmaChannel->CNDTR = 1;
    DMA_Cmd(dmaChannel, ENABLE);
}

void Usart::putsFast(const char * str) {

    uint32_t i = 0;
    for( ; str[i] != 0; i++) {
        outputBuffer[i] = str[i];
    }
    /* Restart DMA Channel*/
    DMA_Cmd(dmaChannel, DISABLE);
    dmaChannel->CNDTR = i;
    DMA_Cmd(dmaChannel, ENABLE);
}

void Usart::putsFast(const char * data, uint32_t len) {

    for(uint32_t i = 0; i < len; i++) {
        outputBuffer[i] = data[i];
    }
    /* Restart DMA Channel*/
    DMA_Cmd(dmaChannel, DISABLE);
    dmaChannel->CNDTR = len;
    DMA_Cmd(dmaChannel, ENABLE);
}

inline void Usart::clear() {

    inputBufferCnt = 0;
}

inline uint32_t Usart::getCount() {

    return inputBufferCnt;
}

inline uint8_t * Usart::getData() {

    return inputBuffer;
}
