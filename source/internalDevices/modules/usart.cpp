#include <usart.h>

static uint8_t * uart1bufPtr;
static uint8_t * uart2bufPtr;
static uint8_t * uart3bufPtr;
static uint8_t * uart1bufCntPtr;
static uint8_t * uart2bufCntPtr;
static uint8_t * uart3bufCntPtr;

// Interrypt handlers
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
void USART3_IRQHandler(void) {

    if ((USART3->SR & USART_FLAG_RXNE) != (u16)RESET) {
        if (*uart3bufCntPtr < 100) uart3bufPtr[(*uart3bufCntPtr)++] = USART_ReceiveData(USART3);
        else USART_ReceiveData(USART3);
    }
}

uint8_t Usart::port1DmaTransmitionInProgress = 0;
uint8_t Usart::port2DmaTransmitionInProgress = 0;
uint8_t Usart::port3DmaTransmitionInProgress = 0;

// Class implementation
// default settings 8 N 1
Usart::Usart(int usartN, DMA& dma, uint32_t bauld, uint16_t dataBits, uint16_t stopBits, uint16_t parity)
    : dmaController(dma) {

    uint8_t irqChannel;
    GPIO_TypeDef * port;
    uint16_t txPin, rxPin;

    if (usartN == 1) {
        uart1bufPtr = inputBuffer;
        uart1bufCntPtr = &inputBufferCnt;

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

        irqChannel = USART1_IRQn;
        port = GPIOA;
        txPin = GPIO_Pin_9;
        rxPin = GPIO_Pin_10;
        usart = USART1;
        dmaTransmitionInProgressFlagPtr = &port1DmaTransmitionInProgress;
        dmaController.setCallbackOnIrq(&callbackUsart1OnDmaIrq);
    } else
    if (usartN == 2) {
        uart2bufPtr = inputBuffer;
        uart2bufCntPtr = &inputBufferCnt;

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

        irqChannel = USART2_IRQn;
        port = GPIOA;
        txPin = GPIO_Pin_2;
        rxPin = GPIO_Pin_3;
        usart = USART2;
        dmaTransmitionInProgressFlagPtr = &port2DmaTransmitionInProgress;
        dmaController.setCallbackOnIrq(&callbackUsart2OnDmaIrq);
    } else
    if (usartN == 3) {
        uart3bufPtr = inputBuffer;
        uart3bufCntPtr = &inputBufferCnt;

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

        irqChannel = USART3_IRQn;
        port = GPIOB;
        txPin = GPIO_Pin_10;
        rxPin = GPIO_Pin_11;
        usart = USART3;
        dmaTransmitionInProgressFlagPtr = &port3DmaTransmitionInProgress;
        dmaController.setCallbackOnIrq(&callbackUsart3OnDmaIrq);
    }

    // Configure the GPIOs
    // Configure Tx as alternate function push-pull
    GPIO_Init_My(port, txPin, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    // Configure Rx as input floating
    GPIO_Init_My(port, rxPin, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);

    // Configure UART
    setUart(bauld, dataBits, stopBits, parity);
    USART_Cmd(usart, ENABLE);

    // Configure DMA
    USART_DMACmd(usart, USART_DMAReq_Tx, ENABLE);
    dmaController.turnOnCallback();

    // Global usart irq enable
    setNvic(irqChannel);
    // Enable the usart Receive interrupt: on usart receive data register is not empty
    USART_ITConfig(usart, USART_IT_RXNE, ENABLE);
}

void Usart::setUart(uint32_t bauld, uint16_t dataBits, uint16_t stopBits, uint16_t parity) {

    USART_InitTypeDef USART_InitStructure;

    // USART configured as follow:
    // - Hardware flow control disabled (RTS and CTS signals)
    // - Receive and transmit enabled
    // - USART Clock disabled
    // - USART CPOL: Clock is active low
    // - USART CPHA: Data is captured on the middle
    // - USART LastBit: The clock pulse of the last data bit is not output to the SCLK pin
    USART_InitStructure.USART_BaudRate = bauld;
    USART_InitStructure.USART_WordLength = dataBits;
    USART_InitStructure.USART_StopBits = stopBits;
    USART_InitStructure.USART_Parity = parity;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(usart, &USART_InitStructure);
}

void Usart::setNvic(uint8_t irqChannel) {

    // NVIC Configuration: Enable the USARTx Interrupt
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = irqChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void Usart::callbackUsart1OnDmaIrq(void) {

    Usart::port1DmaTransmitionInProgress = 0;
}
void Usart::callbackUsart2OnDmaIrq(void) {

    Usart::port2DmaTransmitionInProgress = 0;
}
void Usart::callbackUsart3OnDmaIrq(void) {

    Usart::port3DmaTransmitionInProgress = 0;
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

void Usart::putsBufferized(const char * data, uint32_t len) {

    while (*dmaTransmitionInProgressFlagPtr) {}
    *dmaTransmitionInProgressFlagPtr = 1;

    // copy data to temporary buffer
    if (len == 0) {
        for( ; data[len] != 0; ++len) {
            outputBuffer[len] = data[len];
        }
    } else {
        for(uint32_t i = 0; i < len; ++i) {
            outputBuffer[i] = data[i];
        }
    }

    dmaController.runDMA((void*)&usart->DR, outputBuffer, len);
}

void Usart::clear() {

    inputBufferCnt = 0;
}

uint32_t Usart::getCount() {

    return inputBufferCnt;
}

uint8_t * Usart::getData() {

    return inputBuffer;
}
