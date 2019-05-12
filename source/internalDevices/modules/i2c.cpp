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
    uint32_t gpioPins;

    if (portNumber == 1) {

        port = I2C1;
        gpioPort = GPIOB;
        gpioPins = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;

        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

        txDMA.setCallback(&callbackOnTransmissionCompletePort1);
        rxDMA.setCallback(&callbackOnTransmissionCompletePort1);
    }
    else /*if (portNumber == 2)*/ {

        port = I2C2;
        gpioPort = GPIOB;
        gpioPins = LL_GPIO_PIN_10 | LL_GPIO_PIN_11;

        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

        txDMA.setCallback(&callbackOnTransmissionCompletePort2);
        rxDMA.setCallback(&callbackOnTransmissionCompletePort2);
    }

    // Turn needed modules tacting
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);

    // I2C configuration
    setI2c(speedClk, ownAddress);

    // GPIO init for I2C
    GPIO::initPins(gpioPort, gpioPins, LL_GPIO_MODE_ALTERNATE, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_DOWN, LL_GPIO_SPEED_FREQ_HIGH);

    // Enables the specified I2C peripheral.
    LL_I2C_Enable(port);

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

    LL_I2C_InitTypeDef params {
        .PeripheralMode = LL_I2C_MODE_I2C,
        .ClockSpeed = speedClk,
        .DutyCycle = LL_I2C_DUTYCYCLE_2,
        .OwnAddress1 = ownAddress,
        .TypeAcknowledge = LL_I2C_ACK,
        .OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT
    };

    LL_I2C_Init(port, &params);
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
    
    LL_I2C_DisableDMAReq_TX(port);
    // LL_I2C_DisableDMAReq_RX(port); // - same as previous

    // EV8_2: Wait until BTF is set before programming the STOP
    while ( !LL_I2C_IsActiveFlag_BTF(port) );
    LL_I2C_GenerateStopCondition(port);

    // --- custom part BUGGY
    while (1) {
    
        // volatile uint32_t cr1 = port->CR1, cr2 = port->CR2;
        uint32_t SR1 = port->SR1;
        uint32_t SR2 = port->SR2;

        if ( (SR1 | SR2) == 0 ) // in case of DMA transmit it's usually 0 but not an I2C_EVENT_MASTER_BYTE_TRANSMITTED
            return;

        else if ( SR2 == (I2C_SR2_MSL | I2C_SR2_BUSY | I2C_SR2_TRA) &&  // I2C_EVENT_MASTER_BYTE_TRANSMITTED
                  SR1 == (I2C_SR1_TXE | I2C_SR1_BTF) ) {                // usually not triggered - check
            BitBanding::setBit((void*)&(GPIOC->BSRR), 13);
            return;
        }

        else if ( SR2 == (I2C_SR2_MSL | I2C_SR2_BUSY) &&    // 30044
                  SR1 == (I2C_SR1_BTF | I2C_SR1_RXNE) ) {
            // after receiving second last byte need to programm NACK and STOP
            LL_I2C_AcknowledgeNextData(port, LL_I2C_NACK);
            LL_I2C_GenerateStopCondition(port);
            LL_I2C_ReceiveData8(port);
        }

        else if ( SR2 == (I2C_SR2_MSL | I2C_SR2_BUSY) &&    // 30040
                  SR1 == I2C_SR1_RXNE )
            LL_I2C_ReceiveData8(port);

        else if ( SR2 == (I2C_SR2_MSL | I2C_SR2_BUSY) &&    // 30000
                  SR1 == 0 ) {
            LL_I2C_EnableReset(port);                       // SOFT RESET. i2c is stucked.
            return;
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

        if (!LL_I2C_IsActiveFlag_BUSY(port)) break;
        
    }
    if (attempts == 0) return Status::StartTrBusy;

    LL_I2C_GenerateStartCondition(port);

    // Wait flag SB=1.
    // Cleared by reading SR1 register followed by writing the DR register
    for (attempts = maxAttempts; attempts > 0; --attempts) {
        if ( port->SR1 == I2C_SR1_SB &&
             port->SR2 == (I2C_SR2_MSL | I2C_SR2_BUSY) ) // I2C_EVENT_MASTER_MODE_SELECT
            break;
    }
    if (attempts == 0) return Status::StartTrGenStart;

    // Send slave address. LSB of address byte is used for
    // transmission direction indication. So device address need
    // to be shifted 1 bit left previously.
    LL_I2C_TransmitData8(port, slaveAddress & 0xFE);

    // Waiting flag ADDR=1.
    // Cleared by reading SR1 resister followed by reading SR2 register.
    for (attempts = maxAttempts; attempts > 0; --attempts) {
        if ( port->SR1 == (I2C_SR1_TXE | I2C_SR1_ADDR) &&
             port->SR2 == (I2C_SR2_MSL | I2C_SR2_BUSY | I2C_SR2_TRA) ) // I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED
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
    LL_I2C_GenerateStartCondition(port);

    uint32_t attempts;

    // Wait flag SB=1.
    // Cleared by reading SR1 register followed by writing the DR register
    for (attempts = maxAttempts; attempts > 0; --attempts) {
        if ( port->SR1 == I2C_SR1_SB &&
             port->SR2 == (I2C_SR2_MSL | I2C_SR2_BUSY) ) // I2C_EVENT_MASTER_MODE_SELECT
            break;
    }
    if (attempts == 0) return Status::StartRvGenStart;

    // ACK bit controls the (N)ACK of the current byte received
    LL_I2C_DisableBitPOS(port);

    // Send slave address. LSB of address byte is used for
    // transmission direction indication. So device address need
    // to be shifted 1 bit left previously.
    LL_I2C_TransmitData8(port, slaveAddress | 1);

    if (onlyOneByte) LL_I2C_AcknowledgeNextData(port, LL_I2C_NACK);

    // Waiting flag ADDR=1.
    // Cleared by reading SR1 resister followed by reading SR2 register.
    for (attempts = maxAttempts; attempts > 0; --attempts) {
        if ( port->SR1 == I2C_SR1_ADDR &&
             port->SR2 == (I2C_SR2_MSL | I2C_SR2_BUSY) ) // I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED
            break;
    }
    if (attempts == 0) return Status::StartRvAddress;

    if (onlyOneByte) LL_I2C_GenerateStopCondition(port);

    return Status::SUCCESS;
}

/**
 * @brief  Stop transmission.
 * @retval enum class Status.
 */
inline I2c::Status I2c::stopTransmission(void) const {

    LL_I2C_GenerateStopCondition(port);

    uint32_t attempts;
    for (attempts = maxAttempts; attempts > 0; --attempts) {
        if ( port->SR1 == (I2C_SR1_TXE | I2C_SR1_BTF) &&
             port->SR2 == (I2C_SR2_MSL | I2C_SR2_BUSY | I2C_SR2_TRA) ) // I2C_EVENT_MASTER_BYTE_TRANSMITTED
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

    LL_I2C_TransmitData8(port, data);

    uint32_t attempts;
    for (attempts = maxAttempts; attempts > 0; --attempts) {

        // I2C_EVENT_MASTER_BYTE_TRANSMITTING
        /* TRA, BUSY, MSL, TXE flags
        EV8: TxE=1, shift register not empty, data register empty,
        cleared by writing DR register */
        
        cout << "\r\n SR1() " << itoa(port->SR1, 16, 4);    // 82 1000 0010
        cout << "\r\n SR2() " << itoa(port->SR2, 16, 4);    // 7  0000 0111

        // if ( port->SR1 == (I2C_SR1_TXE | I2C_SR1_BTF) &&
        //      port->SR2 == (I2C_SR2_MSL | I2C_SR2_BUSY | I2C_SR2_TRA) )
        // 0x00070084 I2C_EVENT_MASTER_BYTE_TRANSMITTED
        /* TRA, BUSY, MSL, TXE and BTF flags
        EV8_2: TxE=1, BTF = 1, Program Stop request.
        TxE and BTF are cleared by hardware by the Stop condition
        */
            break;
    }
    if (attempts == 0) return Status::SendByte;

    return Status::SUCCESS;

    // Note: Stop condition should be programmed during EV8_2 event, when either TxE or BTF is set.
}

/**
 * @brief  Read data byte from peripheral.
 * @retval Received data byte.
 */
uint8_t I2c::receiveByte(void) const {

    uint8_t receivedByte = 0x77;    // Value can be used like Magic Constant
                                    // indicating problems in transmission.

    uint32_t attempts;
    
    for (attempts = maxAttempts; attempts > 0; --attempts) {

        // Check for Receive Buffer Not Empty flag
        if ( LL_I2C_IsActiveFlag_RXNE(port) ) {

            receivedByte = LL_I2C_ReceiveData8(port);
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
         (status = stopTransmission()) != Status::SUCCESS ) {

        cout << "\r\n send() 1 byte ERROR = " << itoa((uint8_t)status);
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
    LL_I2C_EnableDMAReq_TX(port);

    
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
    // LL_I2C_EnableLastDMA(port);
    LL_I2C_EnableDMAReq_RX(port);

    cout << "\r\n receiveBufferized() END";
    return Status::SUCCESS;

    // On transmition complete DMA triggers callback
    // and executes Stop procedure.
}
