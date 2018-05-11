#include <i2c.h>

I2c::I2c(uint8_t portNumber, uint8_t slaveAddr) {

    GPIO_TypeDef* gpioPort;
    uint16_t gpioPins;

    slaveAddress = slaveAddr;

    if (portNumber == 1) {

        port = I2C1;
        gpioPort = GPIOB;
        gpioPins = GPIO_Pin_6 | GPIO_Pin_7;

        //Turn needed modules tacting
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    } else if (portNumber == 2) {

        // Need to configure!

        // port = I2C1;
        // gpioPort = GPIOB;
        // gpioPins = GPIO_Pin_6 | GPIO_Pin_7;

        // //Turn needed modules tacting
        // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
        // RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
        // RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    }

    //I2C configuration
    I2C_InitTypeDef i2c;
    i2c.I2C_ClockSpeed = 100000;
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1 = 0x15;    // Random adress
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(port, &i2c);

    //GPIO init for I2C
    GPIO_Init_My(gpioPort, gpioPins, GPIO_Mode_AF_OD, GPIO_Speed_50MHz);

    //Enables the specified I2C peripheral.
    I2C_Cmd(port, ENABLE);

}

void I2c::startTransmit(uint8_t transmissionDirection) const {

    //wait for free bus
    while (I2C_GetFlagStatus(port, I2C_FLAG_BUSY));
    //gen start
    I2C_GenerateSTART(port, ENABLE);
    //wait flag
    while (!I2C_CheckEvent(port, I2C_EVENT_MASTER_MODE_SELECT));
    //send slave adr, may need bit shift
    //http://microtechnics.ru/stm32-ispolzovanie-i2c/#comment-8109
    I2C_Send7bitAddress(port, slaveAddress << 1, transmissionDirection);
    //wait flag
    if (transmissionDirection == I2C_Direction_Transmitter) {

    	while (!I2C_CheckEvent(port, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    } else if (transmissionDirection == I2C_Direction_Receiver) {

	    while (!I2C_CheckEvent(port, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    }
}

void I2c::write(uint8_t data) const {
    
    startTransmit();
    
    I2C_SendData(port, data);
    while (!I2C_CheckEvent(port, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(port, ENABLE);
}

uint8_t I2c::read(void) const {

    startTransmit();

    while (!I2C_CheckEvent(port, I2C_EVENT_MASTER_BYTE_RECEIVED));
    I2C_GenerateSTOP(port, ENABLE);
    return I2C_ReceiveData(port);
}