#include <usart.h>

volatile uint8_t * Usart::buf1Ptr = 0;
volatile uint8_t * Usart::buf2Ptr = 0;
volatile uint8_t * Usart::buf3Ptr = 0;
volatile uint8_t * Usart::buf1CntPtr = 0;
volatile uint8_t * Usart::buf2CntPtr = 0;
volatile uint8_t * Usart::buf3CntPtr = 0;
volatile uint8_t Usart::port1DmaTransmitionInProgress = 0;
volatile uint8_t Usart::port2DmaTransmitionInProgress = 0;
volatile uint8_t Usart::port3DmaTransmitionInProgress = 0;

// Interrypt handlers
void USART1_IRQHandler(void) {

    if (USART1->SR & USART_SR_RXNE) {
        if (*Usart::buf1CntPtr < 100) Usart::buf1Ptr[(*Usart::buf1CntPtr)++] = LL_USART_ReceiveData8(USART1);
        else LL_USART_ReceiveData8(USART1);
    }
}
void USART2_IRQHandler(void) {

    if (USART2->SR & USART_SR_RXNE) {
        if (*Usart::buf2CntPtr < 100) Usart::buf2Ptr[(*Usart::buf2CntPtr)++] = LL_USART_ReceiveData8(USART2);
        else LL_USART_ReceiveData8(USART2);
    }
}
void USART3_IRQHandler(void) {

    if (USART3->SR & USART_SR_RXNE) {
        if (*Usart::buf3CntPtr < 100) Usart::buf3Ptr[(*Usart::buf3CntPtr)++] = LL_USART_ReceiveData8(USART3);
        else LL_USART_ReceiveData8(USART3);
    }
}

// Class implementation
// default settings 8 N 1
Usart::Usart(int usartN, DMA& txDMA, uint32_t bauld, uint32_t dataBits, uint32_t stopBits, uint32_t parity)
    : txDMA(txDMA) {

    IRQn_Type irqChannel;
    GPIO_TypeDef * port;
    uint32_t txPin, rxPin;

    if (usartN == 1) {
        buf1Ptr = inputBuffer;
        buf1CntPtr = &inputBufferCnt;

        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

        irqChannel = USART1_IRQn;
        port = GPIOA;
        txPin = LL_GPIO_PIN_9;
        rxPin = LL_GPIO_PIN_10;
        usart = USART1;
        dmaTransferActiveFlagPtr = &port1DmaTransmitionInProgress;
        txDMA.setCallback(&callbackUsart1OnDmaIrq);
    } else
    if (usartN == 2) {
        buf2Ptr = inputBuffer;
        buf2CntPtr = &inputBufferCnt;

        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

        irqChannel = USART2_IRQn;
        port = GPIOA;
        txPin = LL_GPIO_PIN_2;
        rxPin = LL_GPIO_PIN_3;
        usart = USART2;
        dmaTransferActiveFlagPtr = &port2DmaTransmitionInProgress;
        txDMA.setCallback(&callbackUsart2OnDmaIrq);
    } else
    /*if (usartN == 3)*/ {
        buf3Ptr = inputBuffer;
        buf3CntPtr = &inputBufferCnt;

        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

        irqChannel = USART3_IRQn;
        port = GPIOB;
        txPin = LL_GPIO_PIN_10;
        rxPin = LL_GPIO_PIN_11;
        usart = USART3;
        dmaTransferActiveFlagPtr = &port3DmaTransmitionInProgress;
        txDMA.setCallback(&callbackUsart3OnDmaIrq);
    }

    // Configure the GPIOs
    // Configure Tx as alternate function push-pull
    GPIO::initPins(port, txPin, LL_GPIO_MODE_ALTERNATE, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_DOWN, LL_GPIO_SPEED_FREQ_HIGH);
    // Configure Rx as input floating
    GPIO::initPins(port, rxPin, LL_GPIO_MODE_FLOATING, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_DOWN, LL_GPIO_SPEED_FREQ_HIGH);

    // Configure UART
    setUart(bauld, dataBits, stopBits, parity);
    LL_USART_Enable(usart);

    // Configure DMA
    LL_USART_EnableDMAReq_TX(usart);
    txDMA.turnOnCallback();

    // Enable the usart Receive interrupt: on usart receive data register is not empty
    LL_USART_EnableIT_RXNE(usart);
    // Global usart irq enable
    // setNvic(irqChannel);
    NVIC_EnableIRQ(irqChannel);
}

void Usart::setUart(uint32_t bauld, uint32_t dataBits, uint32_t stopBits, uint32_t parity) {

    LL_USART_InitTypeDef params {

        .BaudRate = bauld,
        .DataWidth = dataBits,
        .StopBits = stopBits,
        .Parity = parity,
        .TransferDirection = LL_USART_DIRECTION_TX_RX,
        .HardwareFlowControl = LL_USART_HWCONTROL_NONE
        // OverSampling = LL_USART_OVERSAMPLING_16
    };
    LL_USART_Init(usart, &params);
}

// void Usart::setNvic(uint8_t irqChannel) {

//     // NVIC Configuration: Enable the USARTx Interrupt
//     NVIC_InitTypeDef NVIC_InitStructure;
//     NVIC_InitStructure.NVIC_IRQChannel = irqChannel;
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//     NVIC_Init(&NVIC_InitStructure);
// }

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

    LL_USART_TransmitData8(usart, byte);
    while(!LL_USART_IsActiveFlag_TC(usart)) ;
}

void Usart::puts(const char *str) {

    while (*str) {
        LL_USART_TransmitData8(usart, *str++);
        while(!LL_USART_IsActiveFlag_TC(usart)) ;
    }
}

void Usart::putsBufferized(const char * data, uint32_t len) {

    while (*dmaTransferActiveFlagPtr) {}
    *dmaTransferActiveFlagPtr = 1;

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

    // LL_USART_DMA_GetRegAddr(usart);
    txDMA.runDataTransfer((void*)&usart->DR, outputBuffer, len);
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
