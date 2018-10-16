#ifndef _I2C_H
#define _I2C_H

// using
extern "C" {
#include <stm32f10x_i2c.h>
#include <stm32f10x_dma.h>
}
#include <gpio.h>

// IRQ handler, extern "C" function, because of c++ bad names
extern "C" {
    void DMA1_Channel6_IRQHandler(void);
}

/*
I2C driver.
Master mode only.
*/
class I2c {

public:

    I2c(uint8_t portNumber, uint32_t speedClk = 100000, uint8_t ownAddress = 0x15);

    // delete copy constructor and assignment operator
    I2c(const I2c&) = delete;
    I2c& operator=(const I2c&) = delete;

    void startTransmit(uint8_t slaveAddress, uint8_t transmissionDirection = I2C_Direction_Transmitter) const;
    void stopTransmit(void) const;

    void writeFast(const uint8_t * data, uint32_t size) const;
    void writeDMA(uint8_t slaveAddress, const uint8_t * data, uint32_t size) const;
    void write(uint8_t byte) const;
    uint8_t read(void) const;

private:

    void setI2c(uint32_t speedClk, uint8_t ownAddress);
    void setDMA(const uint8_t * data, uint32_t size) const;

    I2C_TypeDef* port;      // Hardware port used for communication by this instanse
    uint8_t ownAddress;
    DMA_Channel_TypeDef * dmaChannel;

};

#endif
