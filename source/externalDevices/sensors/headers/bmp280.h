#ifndef _BMP280_H
#define _BMP280_H

// using
#include <i2c.h>
#include <IDelayer.h>
#include <OStream.h>    // ??? debug
#include <stm32f10x_i2c.h>

/**
Bosch BMP280 digital pressure / temperature sensor API

## Connection in I2C or SPI mode
Interface selection is done automatically based on CSB (chip select) status.
If CSB is connected to V, the I²C interface is active.
If CSB is pulled down, the SPI interface is activated.

## Address in I2C mode
Connecting SDO to GND results in slave address 1110110 (0x76);
connection it to V results in slave address 1110111 (0x77),
which is the same as BMP180’s I²C address.
The SDO pin cannot be left floating; if left floating, the I²C address will be undefined.
*/
class Bmp280 {

public:

    Bmp280(I2c& i2c, IDelayer& delayer, OStream& cout, uint8_t address = 0x76);

    // delete copy constructor and assignment operator
    Bmp280(const Bmp280&) = delete;
    Bmp280& operator=(const Bmp280&) = delete;

    // configuration

    // control temperature measurements

    // status

    // configuration storage in EEPROM

private:

    // procedures
    void initialization(void);

    // hardware commands

    // -
    I2c& i2c;
    IDelayer& delayer;
    OStream& cout;  // ??? debug
    const uint8_t address;
    
    /// Memory map
    enum class Register : uint8_t {
        dig_T1_lsb  = 0x88,
        dig_T1_msb  = 0x89,
        dig_T2_lsb  = 0x8A,
        dig_T2_msb  = 0x8B,
        dig_T3_lsb  = 0x8C,
        dig_T3_msb  = 0x8D,
        dig_P1_lsb  = 0x8E,
        dig_P1_msb  = 0x8F,
        dig_P2_lsb  = 0x90,
        dig_P2_msb  = 0x91,
        dig_P3_lsb  = 0x92,
        dig_P3_msb  = 0x93,
        dig_P4_lsb  = 0x94,
        dig_P4_msb  = 0x95,
        dig_P5_lsb  = 0x96,
        dig_P5_msb  = 0x97,
        dig_P6_lsb  = 0x98,
        dig_P6_msb  = 0x99,
        dig_P7_lsb  = 0x9A,
        dig_P7_msb  = 0x9B,
        dig_P8_lsb  = 0x9C,
        dig_P8_msb  = 0x9D,
        dig_P9_lsb  = 0x9E,
        dig_P9_msb  = 0x9F,
        reserved_1  = 0xA0,
        reserved_2  = 0xA1,
        id          = 0xD0,     /// 0x58 can be read after power ON
        reset       = 0xE0,     /// Write 0xB6 to soft reset; read 0x00
        status      = 0xF3,     /// Bit-field Status
        ctrl_meas   = 0xF4,     /// Bit-field Ctrl_meas
        config      = 0xF5,     /// Bit-field Config
        press_msb   = 0xF7,
        press_lsb   = 0xF8,
        press_xlsb  = 0xF9,
        temp_msb    = 0xFA,
        temp_lsb    = 0xFB,
        temp_xlsb   = 0xFC
    };
    
    /// Status register bit-field
    /// Contains two bits which indicate the status of the device
    struct Status {
        unsigned int            : 4;    /// Unused
        unsigned int measuring  : 1;    /// Bit 3; 1 - running, 0 - finished
        unsigned int            : 2;    /// Unused
        unsigned int im_update  : 1;    /// Bit 0; 1 - NVM data are being copied
                                        /// to image registers and back to ‘0’
                                        /// when the copying is done
    };
    /// Oversampling modes
    enum class Osrs_mode : uint8_t {
        Skipped         = 0,    /// Output set to 0x80000
        OversamplingX1  = 1,
        OversamplingX2  = 2,
        OversamplingX4  = 3,
        OversamplingX8  = 4,
        OversamplingX16 = 5     /// Values 6 and 7 are also correct
    };

    /// Power modes
    enum class Power_mode : uint8_t {
        Sleep   = 0,
        Forced  = 1,            /// Value 2 is also correct
        Normal  = 3
    };
    
    /// Ctrl_meas register bit-field
    /// Sets the data acquisition options of the device
    struct Ctrl_meas {
        enum Osrs_mode osrs_t   : 3;    /// Controls oversampling of temperature data
        enum Osrs_mode osrs_p   : 3;    /// Controls oversampling of pressure data
        enum Power_mode mode    : 2;    /// Controls the power mode of device
    };

    /// Standby time settings
    /// t_standby
    enum class T_standby : uint8_t {
        ms_0_5  = 0,            /// 0.5 ms
        ms_62_5 = 1,            /// 62.5 ms
        ms_125  = 2,            /// 125 ms
        ms_250  = 3,            /// 250 ms
        ms_500  = 4,            /// 500 ms
        ms_1000 = 5,            /// 1000 ms
        ms_2000 = 6,            /// 2000 ms
        ms_4000 = 7             /// 4000 ms
    };

    /// Filter settings
    /// ??? Data-sheet have no values
    enum class Filter_mode : uint8_t {
        Off         = 0,        /// Filter off
        Unused      = 1,
        X2          = 2,
        X4          = 3,
        X8          = 4,
        X16         = 5
    };
    
    /// Config register bit-field
    /// Sets the rate, filter and interface options of the device.
    /// Writes to the “config” register in normal mode may be ignored.
    /// In sleep mode writes are not ignored.
    struct Config {
        enum T_standby t_sb     : 3;    /// Controls inactive duration t_standby in normal mode
        enum Filter_mode filter : 3;    /// Controls the time constant of the IIR filter
        unsigned int            : 1;    /// Unused
        unsigned int spi3w_en   : 1;    /// Enables 3-wire SPI interface when set to ‘1’
    };
    
    /// Pressure register 0xF7...0xF9 “press”
    /// Raw 20 bit pressure measurement output data
    struct PressureRaw {
        unsigned int press_msb  : 8;    /// MSB part up[19:12] of raw data
        unsigned int press_lsb  : 8;    /// LSB part up[11:4] of raw data
        unsigned int press_xlsb : 4;    /// XLSB part up[3:0] of raw data
    };
    
    /// Temperature register 0xFA...0xFC “temp”
    /// Raw 20 bit temperature measurement output data
    struct TemperatureRaw {
        unsigned int temp_msb   : 8;    /// MSB part up[19:12] of raw data
        unsigned int temp_lsb   : 8;    /// LSB part up[11:4] of raw data
        unsigned int temp_xlsb  : 4;    /// XLSB part up[3:0] of raw data
    };
    
    
    

};

#endif
