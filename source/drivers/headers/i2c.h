#ifndef _I2C_H
#define _I2C_H

extern "C" {
#include <stm32f10x_i2c.h>
}

#include <gpio.h>

/*
I2C driver
Master mode only can connect to 1 slave device
*/
class I2c {

    friend int main(void);

public:    

    void startTransmit(uint8_t transmissionDirection = I2C_Direction_Transmitter) const;

    void write(uint8_t byte) const;
    uint8_t read(void) const;
    
    void stopTransmit(void) const;

    // delete copy constructor and assignment operator
    I2c(const I2c&) = delete;
    I2c& operator=(const I2c&) = delete;

private:

    I2c(uint8_t portNumber, uint8_t slaveAddress);

    I2C_TypeDef* port;        // Hardware port used for communication by this instanse
    uint8_t slaveAddress;

};

#endif
