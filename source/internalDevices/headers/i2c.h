#ifndef _I2C_H
#define _I2C_H

// using
extern "C" {
#include <stm32f10x_i2c.h>
}
#include <gpio.h>

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

    void write(uint8_t byte) const;
    uint8_t read(void) const;

private:

    I2C_TypeDef* port;      // Hardware port used for communication by this instanse
    uint8_t ownAddress;

};

#endif
