/**
 * @file    i2c.hpp
 * @author  Denis Homutovski
 * @version V0.9.0
 * @date    07-12-2018
 * @brief   I2C class header.
 * @details   I2C driver.
 * @pre       -
 * @bug       -
 * @warning   Master mode only.
 * @copyright GNU Public License.
 */

#ifndef _I2C_H
#define _I2C_H

// using
extern "C" {
#include <stm32f1xx_ll_i2c.h>
}
#include <gpio.h>
#include <dma.hpp>

#include <bitBanding.hpp>
#include <ostream.h>    // debug ???

/**
 * I2C driver. Use DMA for packet data transmission.
 * Master mode only implemented.
 */
class I2c {

public:

    I2c(uint8_t portNumber,
        DMA& txDMA,
        DMA& rxDMA,
        OStream& cout,
        uint32_t speedClk = 100000,
        uint8_t ownAddress = 0x15);
        
    I2c(const I2c&) = delete;               /**< Delete copy constructor. */
    I2c& operator=(const I2c&) = delete;    /**< Delete assignment operator. */

    /**
     * Return status of procedures
     */
    enum class Status : uint8_t {
        SUCCESS = 0,
        StartTrBusy = 10,
        StartTrGenStart,
        StartTrAddress,
        StartRvGenStart = 20,
        StartRvAddress,
        StopTransm = 30,
        SendByte = 40,
        RvBuffSize = 50
    };

    Status send(uint8_t slaveAddress, uint8_t data) const;
    Status send(uint8_t slaveAddress, const uint8_t * data, uint32_t size) const;
    uint8_t receive(uint8_t slaveAddress) const;

    Status sendBufferized(uint8_t slaveAddress, const uint8_t * data, uint32_t size) const;
    Status receiveBufferized(uint8_t slaveAddress, const uint8_t * data, uint32_t size) const;

private:

    void setI2c(uint32_t speedClk, uint8_t ownAddress);

    Status startTransmitter(uint8_t slaveAddress) const;
    Status startReceiver(uint8_t slaveAddress, uint8_t onlyOneByte = 0) const;
    Status stopTransmission(void) const;

    Status sendByte(uint8_t byte) const;
    uint8_t receiveByte(void) const;
    
    /** Maximum amount of attempts during work with peripheral. */ 
    static constexpr uint32_t maxAttempts = 7200000;

    I2C_TypeDef* port;      /**< Hardware port used by this instanse. */
    uint8_t ownAddress;     /**< Own I2C address. */
    DMA& txDMA;             /**< Reference to DMA channel instance used for transmit. */
    DMA& rxDMA;             /**< Reference to DMA channel instance used for receive. */
    OStream& cout;

    static void stopDmaTransmission(I2C_TypeDef* port);
    static void callbackOnTransmissionCompletePort1(void);
    static void callbackOnTransmissionCompletePort2(void);

};

#endif
