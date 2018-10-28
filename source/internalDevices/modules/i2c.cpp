#include <i2c.h>

uint8_t I2c::dmaTransmitionInProgress;

I2c::I2c(uint8_t portNumber, DMA& dma, uint32_t speedClk, uint8_t ownAddress)
    : ownAddress(ownAddress),
      dmaController(dma) {

    GPIO_TypeDef* gpioPort;
    uint16_t gpioPins;

    if (portNumber == 1) {

        port = I2C1;
        gpioPort = GPIOB;
        gpioPins = GPIO_Pin_6 | GPIO_Pin_7;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

        dmaController.setCallbackOnIrq(&callbackI2C1OnDmaIrq);
    }
    else /*if (portNumber == 2)*/ {

        port = I2C2;
        gpioPort = GPIOB;
        gpioPins = GPIO_Pin_10 | GPIO_Pin_11;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

        dmaController.setCallbackOnIrq(&callbackI2C2OnDmaIrq);
    }

    // Turn needed modules tacting
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // I2C configuration
    setI2c(speedClk, ownAddress);

    // GPIO init for I2C
    GPIO_Init_My(gpioPort, gpioPins, GPIO_Mode_AF_OD, GPIO_Speed_50MHz);

    // Enables the specified I2C peripheral.
    I2C_Cmd(port, ENABLE);
    I2C_DMACmd(port, ENABLE);

    dmaController.turnOnCallback();
}

void I2c::setI2c(uint32_t speedClk, uint8_t ownAddress) {

    I2C_InitTypeDef i2c;

    i2c.I2C_ClockSpeed = speedClk;
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1 = ownAddress;
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    I2C_Init(port, &i2c);
}

void I2c::startTransmit(uint8_t slaveAddress, uint8_t transmissionDirection) const {

    // wait for free bus
    while (I2C_GetFlagStatus(port, I2C_FLAG_BUSY));
    // gen start
    I2C_GenerateSTART(port, ENABLE);
    // wait flag
    while (!I2C_CheckEvent(port, I2C_EVENT_MASTER_MODE_SELECT));
    // send slave adr, LSB used for read/write indication so need shift
    I2C_Send7bitAddress(port, slaveAddress, transmissionDirection);
    // wait flag
    if (transmissionDirection == I2C_Direction_Transmitter) {
        while (!I2C_CheckEvent(port, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    }
    else if (transmissionDirection == I2C_Direction_Receiver) {
        while (!I2C_CheckEvent(port, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    }
}

void I2c::write(uint8_t data) const {

    I2C_SendData(port, data);
    while (!I2C_CheckEvent(port, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

void I2c::callbackI2C1OnDmaIrq(void) {

    // EV8_2: Woyt until BTF is set before programming the STOP
    while ((I2C1->SR1 & 0x00004) != 0x000004);
    I2C_GenerateSTOP(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2c::dmaTransmitionInProgress = 0;
}

void I2c::callbackI2C2OnDmaIrq(void) {

    // EV8_2: Woyt until BTF is set before programming the STOP
    while ((I2C2->SR1 & 0x00004) != 0x000004);
    I2C_GenerateSTOP(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2c::dmaTransmitionInProgress = 0;
}

void I2c::writeDMA(uint8_t slaveAddress, const uint8_t * data, uint32_t size) const {

    while (dmaTransmitionInProgress) {}

    startTransmit(slaveAddress, I2C_Direction_Transmitter);

    dmaTransmitionInProgress = 1;

    dmaController.runDMA((void*)&port->DR, data, size);

    // on transmition complete DMA triggers callback
    // and executes i2c stop
}

uint8_t I2c::read(void) const {

    while (!I2C_CheckEvent(port, I2C_EVENT_MASTER_BYTE_RECEIVED));
    return I2C_ReceiveData(port);
}

void I2c::stopTransmit(void) const {

    I2C_GenerateSTOP(port, ENABLE);
    while (!I2C_CheckEvent(port, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}
