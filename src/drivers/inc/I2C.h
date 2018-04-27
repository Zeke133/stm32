#ifndef I2C_H
#define I2C_H

#include <stm32f10x_i2c.h>


#include <gpio.h>


// some to interface

class I2c {

    friend int main(void);

public:    

    void write(uint8_t byte);
    uint8_t read(void);

private:

    I2c(uint8_t portNumber, uint8_t slaveAddress);

    inline void startTransmit(uint8_t transmissionDirection = I2C_Direction_Transmitter);
    
    I2C_TypeDef* port;        //Hardware port used for communication by this instanse
    uint8_t slaveAddress;

};

#endif
