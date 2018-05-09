#ifndef I2C_H
#define I2C_H

#include <stm32f10x_i2c.h>


#include <gpio.h>


class I2c {

    friend int main(void);

public:    

    void write(uint8_t byte) const;
    uint8_t read(void) const;

private:

    I2c(uint8_t portNumber, uint8_t slaveAddress);

    inline void startTransmit(uint8_t transmissionDirection = I2C_Direction_Transmitter) const;
    
    I2C_TypeDef* port;        //Hardware port used for communication by this instanse
    uint8_t slaveAddress;

};

#endif
