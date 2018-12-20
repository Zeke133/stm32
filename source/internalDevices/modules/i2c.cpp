/**
 * @file    i2c.cpp
 * @author  Denis Homutovski
 * @version V0.9.0
 * @date    07-12-2018
 * @brief   I2c class source
 */

#include <i2c.hpp>

/**
 * @brief  Driver constructor.
 * @param  portNumber: Number of I2C hardware peripheral.
 * @param  dma: Reference to DMA instance used by this driver.
 * @param  speedClk: I2C clock in Hz.
 * @param  ownAddress: Own I2C address of instance.
 */
I2c::I2c(uint8_t portNumber, DMA& txDMA, DMA& rxDMA, OStream& cout, uint32_t speedClk, uint8_t ownAddress)
    : ownAddress(ownAddress),
      txDMA(txDMA),
      rxDMA(rxDMA),
      cout(cout) {

    GPIO_TypeDef* gpioPort;
    uint16_t gpioPins;

    if (portNumber == 1) {

        port = I2C1;
        gpioPort = GPIOB;
        gpioPins = GPIO_Pin_6 | GPIO_Pin_7;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

        txDMA.setCallback(&callbackOnTransmissionCompletePort1);
        rxDMA.setCallback(&callbackOnTransmissionCompletePort1);
    }
    else /*if (portNumber == 2)*/ {

        port = I2C2;
        gpioPort = GPIOB;
        gpioPins = GPIO_Pin_10 | GPIO_Pin_11;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

        txDMA.setCallback(&callbackOnTransmissionCompletePort2);
        rxDMA.setCallback(&callbackOnTransmissionCompletePort2);
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

    txDMA.turnOnCallback();
    rxDMA.turnOnCallback();
}

/**
 * @brief  Set peripheral paramiters.
 * @param  speedClk: I2C clock in Hz.
 * @param  ownAddress: Own I2C address of instance.
 * @retval None
 */
inline void I2c::setI2c(uint32_t speedClk, uint8_t ownAddress) {

    I2C_InitTypeDef i2c;

    i2c.I2C_ClockSpeed = speedClk;
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1 = ownAddress;
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    I2C_Init(port, &i2c);
}

/**
 * @brief  Procedure to stop DMA data transission.
 *         Common code for callback functions.
 *         Master transmitter: In the interrupt routine after the EOT interrupt,
 *         disable DMA requests.
 *         Then wait for a BTF event before programming the Stop condition.
 * @param  port: I2C device to stop.
 * @retval None
 */
inline void I2c::stopDmaTransmission(I2C_TypeDef* port) {
    
    I2C_DMACmd(port, DISABLE);
    // EV8_2: Wait until BTF is set before programming the STOP
    while ( (port->SR1 & I2C_FLAG_BTF) == 0);
    I2C_GenerateSTOP(port, ENABLE);

    // --- custom part BUGGY
    uint32_t event;

    while (1) {
        event = I2C_GetLastEvent(port);

        if (event == I2C_EVENT_MASTER_BYTE_TRANSMITTED) {
            
            break;
        }
        else if (event & (uint32_t)(I2C_SR1_BTF | I2C_SR1_RXNE)) {

            I2C_ReceiveData(port);
            // 30044 first BTF disapear >> 30040
            // 30040 than RxNE disapear >> 30000
        }
        else if (event & (uint32_t)((I2C_SR2_MSL | I2C_SR2_BUSY)<<16)) {

            I2C_GenerateSTOP(port, ENABLE);
            // 30044 first BTF disapear >> 30040
            // 30040 than RxNE disapear >> 30000
        }
        else {

            // does not react on STOP. 30000 BUSY MSL are still set 4ever
            // if (event & (uint32_t)0x30000)
            BitBanding::setBit((void*)&(GPIOC->BSRR), 13);
            //*((uint32_t *)(0x40000000 + 0x10000 + 0x1000 + 4*4)) = (1<<12);

            // I2C_CR1_SWRST I2C_CR1
            break;
        }
    }
}
/**
 * I2C1 data transmission complete callback.
 * Used for finishing data transmission in DMA mode.
 */
void I2c::callbackOnTransmissionCompletePort1(void) {

    stopDmaTransmission(I2C1);
}
/**
 * I2C2 data transmission complete callback.
 * Used for finishing data transmission in DMA mode.
 */
void I2c::callbackOnTransmissionCompletePort2(void) {

    stopDmaTransmission(I2C2);
}

/**
 * @brief  Start in transmitter mode.
 * @param  slaveAddress: I2C address of slave device shifted 1 bit left.
 * @retval enum class Status.
 */
inline I2c::Status I2c::startTransmitter(uint8_t slaveAddress) const {

    uint32_t attempts;

    // Waiting for free bus
    for (attempts = maxAttempts; attempts > 0; --attempts) {

        if (!I2C_GetFlagStatus(port, I2C_FLAG_BUSY)) break;
        
    }
    if (attempts == 0) return Status::StartTrBusy;

    I2C_GenerateSTART(port, ENABLE);

    // Wait flag SB=1.
    // Cleared by reading SR1 register followed by writing the DR register
    for (attempts = maxAttempts; attempts > 0; --attempts) {
        if (I2C_CheckEvent(port, I2C_EVENT_MASTER_MODE_SELECT))
            break;
    }
    if (attempts == 0) return Status::StartTrGenStart;

    // Send slave address. LSB of address byte is used for
    // transmission direction indication. So device address need
    // to be shifted 1 bit left previously.
    I2C_Send7bitAddress(port, slaveAddress, I2C_Direction_Transmitter);

    // Waiting flag ADDR=1.
    // Cleared by reading SR1 resister followed by reading SR2 register.
    for (attempts = maxAttempts; attempts > 0; --attempts) {
        if (I2C_CheckEvent(port, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
            break;
    }
    if (attempts == 0) return Status::StartTrAddress;

    return Status::SUCCESS;
}

/**
 * @brief  Start in receiver mode.
 * @param  slaveAddress: I2C address of slave device shifted 1 bit left.
 * @param  onlyOneByte: Flag indicating that will receive only 1 byte.
 * @retval enum class Status.
 */
I2c::Status I2c::startReceiver(uint8_t slaveAddress, uint8_t onlyOneByte) const {

    // Don't wait for free bus
    I2C_GenerateSTART(port, ENABLE);

    uint32_t attempts;

    // Wait flag SB=1.
    // Cleared by reading SR1 register followed by writing the DR register
    for (attempts = maxAttempts; attempts > 0; --attempts) {
        if (I2C_CheckEvent(port, I2C_EVENT_MASTER_MODE_SELECT))
            break;
    }
    if (attempts == 0) return Status::StartRvGenStart;

    I2C_NACKPositionConfig(port, I2C_NACKPosition_Current); // check with I2C_NACKPosition_Next

    // Send slave address. LSB of address byte is used for
    // transmission direction indication. So device address need
    // to be shifted 1 bit left previously.
    I2C_Send7bitAddress(port, slaveAddress, I2C_Direction_Receiver);

    if (onlyOneByte) I2C_AcknowledgeConfig(port, DISABLE);

    // Waiting flag ADDR=1.
    // Cleared by reading SR1 resister followed by reading SR2 register.
    for (attempts = maxAttempts; attempts > 0; --attempts) {
        if (I2C_CheckEvent(port, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
            break;
    }
    if (attempts == 0) return Status::StartRvAddress;

    if (onlyOneByte) I2C_GenerateSTOP(port, ENABLE);

    return Status::SUCCESS;
}

/**
 * @brief  Stop transmission.
 * @retval enum class Status.
 */
inline I2c::Status I2c::stopTransmission(void) const {

    I2C_GenerateSTOP(port, ENABLE);

    uint32_t attempts;
    for (attempts = maxAttempts; attempts > 0; --attempts) {
        if (I2C_CheckEvent(port, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
            break;
    }
    if (attempts == 0) return Status::StopTransm;

    return Status::SUCCESS;
}

/**
 * @brief  Write data byte to peripheral.
 * @param  data: Data byte to sent.
 * @retval enum class Status.
 */
inline I2c::Status I2c::sendByte(uint8_t data) const {

    I2C_SendData(port, data);

    uint32_t attempts;
    for (attempts = maxAttempts; attempts > 0; --attempts) {
        if (I2C_CheckEvent(port, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
            break;
    }
    if (attempts == 0) return Status::SendByte;

    return Status::SUCCESS;
}

/**
 * @brief  Read data byte from peripheral.
 * @retval Received data byte.
 */
uint8_t I2c::receiveByte(void) const {

    uint8_t receivedByte = 0x77;    // Value can be used like Magic Constant
                                    // indicating problems in transmission.

    uint32_t attempts, flags;
    for (attempts = maxAttempts; attempts > 0; --attempts) {

        flags = I2C_GetLastEvent(port);

        // Check for Receive Buffer Not Empty flag
        if (flags & I2C_FLAG_RXNE) {

            receivedByte = I2C_ReceiveData(port);
            break;
        }
    }
    
    return receivedByte;
}

/**
 * @brief  Send data byte. Use CPU for transmission. Blocks thread.
 * @param  slaveAddress: I2C address of slave device shifted 1 bit left.
 * @param  data: Data byte.
 * @retval enum class Status.
 */
I2c::Status I2c::send(uint8_t slaveAddress, uint8_t data) const {

    Status status;
    
    if ( (status = startTransmitter(slaveAddress)) != Status::SUCCESS ||
         (status = sendByte(data)) != Status::SUCCESS ||
         (status = stopTransmission()) != Status::SUCCESS ) { // ??? stuck here for a long

        cout << "\r\n send() 1 byte ERROR = " << itoa((uint8_t)status);
        return status;
    }
    else {

        cout << "\r\n send() END";
        return Status::SUCCESS;
    }
}

/**
 * @brief  Send data byte. Use CPU for transmission. Blocks thread.
 * @param  slaveAddress: I2C address of slave device shifted 1 bit left.
 * @param  data: Data byte.
 * @retval enum class Status.
 */
I2c::Status I2c::sendWOStop(uint8_t slaveAddress, uint8_t data) const {

    Status status;
    
    if ( (status = startTransmitter(slaveAddress)) != Status::SUCCESS ||
         (status = sendByte(data)) != Status::SUCCESS ) {

        cout << "\r\n sendWOStop() 1 byte ERROR = " << itoa((uint8_t)status);
        return status;
    }
    else {

        cout << "\r\n send() END";
        return Status::SUCCESS;
    }
}

/**
 * @brief  Send data buffer. Use CPU for transmission. Blocks thread.
 * @param  slaveAddress: I2C address of slave device shifted 1 bit left.
 * @param  data: Pointer to data.
 * @param  size: Amount of bytes to send.
 * @retval enum class Status.
 */
I2c::Status I2c::send(uint8_t slaveAddress, const uint8_t * data, uint32_t size) const {

    Status status = startTransmitter(slaveAddress);

    cout << "\r\n send() startTransmitter = " << itoa((uint8_t)status);
    if (status != Status::SUCCESS) return status;

    for (uint32_t i = 0; i < size; ++i) {

        status = sendByte(data[i]);

        cout << "\r\n send() sendByte = " << itoa((uint8_t)status);
        if (status != Status::SUCCESS) return status;
    }

    status = stopTransmission();

    cout << "\r\n send() stopTransmission = " << itoa((uint8_t)status);
    if (status != Status::SUCCESS) return status;

    cout << "\r\n send() END";
    return Status::SUCCESS;
}

/**
 * @brief  Receive data byte. Use CPU for transmission. Blocks thread.
 * @param  slaveAddress: I2C address of slave device shifted 1 bit left.
 * @retval Received data byte.
 */
uint8_t I2c::receive(uint8_t slaveAddress) const {
    
    // 1 byte will be received - indicating it with flag = 1.
    startReceiver(slaveAddress, 1);

    return receiveByte();
}

/**
 * @brief  Send data buffer. Use DMA for transmission. Doesn't blocks thread.
 * @param  slaveAddress: I2C address of slave device shifted 1 bit left.
 * @param  data: Pointer to data.
 * @param  size: Amount of bytes to send.
 * @retval enum class Status.
 */
I2c::Status I2c::sendBufferized(uint8_t slaveAddress, const uint8_t * data, uint32_t size) const {
    
    Status status = startTransmitter(slaveAddress);

    cout << "\r\n sendBufferized() startTransmitter = " << itoa((uint8_t)status);
    if (status != Status::SUCCESS) return status;

    // Turn ON DMA requests
    // Should be turned off after transmission
    I2C_DMACmd(port, ENABLE);
    
    txDMA.runDataTransfer((void*)&port->DR, data, size);

    cout << "\r\n sendBufferized() END";
    return Status::SUCCESS;

    // On transmition complete DMA triggers callback
    // and executes Stop procedure.
}

/**
 * @brief  Receive data buffer. Use DMA for transmission.
 *         Master receiver: when the number of bytes to be received is equal to or
 *         greater than two, the DMA controller sends a hardware signal, EOT_1,
 *         corresponding to the last but one data byte (number_of_bytes – 1).
 *         If, in the I2C_CR2 register, the LAST bit is set, I2C automatically sends a NACK
 *         after the next byte following EOT_1. The user can generate a
 *         Stop condition in the DMA Transfer Complete interrupt routine if enabled.
 * @param  slaveAddress: I2C address of slave device shifted 1 bit left.
 * @param  data: Pointer to data buffer.
 * @param  size: Amount of bytes to receive.
 * @retval enum class Status.
 */
I2c::Status I2c::receiveBufferized(uint8_t slaveAddress, const uint8_t * data, uint32_t size) const {

    // Transmission of data less that 2 bytes not allowed
    // because of hardware restrictions.
    if (size < 2) {
        
        cout << "\r\n receiveBufferized() size < 2";
        return Status::RvBuffSize;
    }

    // More than 1 byte will be received - indicating it with flag = 0.
    Status status = startReceiver(slaveAddress);

    cout << "\r\n receiveBufferized() startReceiver = " << itoa((uint8_t)status);
    if (status != Status::SUCCESS) return status;

    rxDMA.runDataTransfer((void*)&port->DR, data, size);

    // Turn ON DMA requests
    // Should be turned off after transmission
    I2C_DMALastTransferCmd(port, ENABLE);
    I2C_DMACmd(port, ENABLE);

    cout << "\r\n receiveBufferized() END";
    return Status::SUCCESS;

    // On transmition complete DMA triggers callback
    // and executes Stop procedure.
}
