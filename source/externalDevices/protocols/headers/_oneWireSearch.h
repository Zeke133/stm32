#ifndef _ONE_WIRE_SEARCH_H
#define _ONE_WIRE_SEARCH_H

// using
#include <IOneWire.h>
#include <crc.h>

/*
Alpha implementation of 1-Wire search ROM algorithm
*/
class OneWireSearch {

public:

    OneWireSearch(IOneWire& driver);
    // delete copy constructor and assignment operator
    OneWireSearch(const OneWireSearch&) = delete;
    OneWireSearch& operator=(const OneWireSearch&) = delete;

    // storage of connected devices
    const static uint32_t MaxDevicesConnected = 5;      // Maximum number of connected 1-Wire devices

    const uint8_t* getDeviceROM(uint32_t index) const;
    uint32_t getDevicesCount(void) const;

    uint32_t searchAllDevices(void);
    uint32_t searchOnly(uint8_t family);
    uint32_t searchExcept(uint8_t family);

private:

    uint8_t devicesConnected[MaxDevicesConnected][8];   // Array of connected devices ROMs
    uint32_t devicesConnectedCount;                     // Number of connected devices

    // hardware driver
    IOneWire& driver;

    // private methods data
    uint8_t ROM_NO[8];
    uint32_t LastDiscrepancy;
    uint32_t LastFamilyDiscrepancy;
    uint32_t LastDeviceFlag;
    
    // private methods
    uint32_t first();                           // Find the 'first' devices on the 1-Wire bus
    uint32_t next();                            // Find the 'next' devices on the 1-Wire bus
    uint32_t search();                          // Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing search state.
    uint32_t verify();                          // Verify the device with the ROM number in ROM_NO buffer is present.
    void targetSetup(uint8_t family_code);      // Setup the search to find the device type 'family_code' on the next call to next() if it is present.
    void familySkipSetup(void);                 // Setup the search to skip the current device type on the next call to next().

};

#endif
