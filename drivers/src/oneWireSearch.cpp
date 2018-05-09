#include <oneWireSearch.h>

OneWireSearch::OneWireSearch(OneWire& drv)
    : driver(drv) {
    
    LastDiscrepancy = 0;
    LastDeviceFlag = 0;
    LastFamilyDiscrepancy = 0;
    
	devicesConnectedCount = 0;
}

// Find the 'first' devices on the 1-Wire bus
// Return 1 : device found, ROM number in ROM_NO buffer
//        0 : no device present
uint32_t OneWireSearch::first() {

    // reset the search state
    LastDiscrepancy = 0;
    LastDeviceFlag = 0;
    LastFamilyDiscrepancy = 0;
    return search();
}

// Find the 'next' devices on the 1-Wire bus
// Return 1 : device found, ROM number in ROM_NO buffer
//        0 : device not found, end of search
uint32_t OneWireSearch::next() {

    // leave the search state alone
    return search();
}

// Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
// search state.
// Return 1 : device found, ROM number in ROM_NO buffer
//        0 : device not found, end of search
uint32_t OneWireSearch::search() {

    uint32_t search_result = 0;

    // if the last call was not the last one
    if (!LastDeviceFlag) {

        uint8_t crc8 = 0;

        // 1-Wire reset
        if (driver.Initialization()) {

            // reset the search
            LastDiscrepancy = 0;
            LastDeviceFlag = 0;
            LastFamilyDiscrepancy = 0;
            return 0;
        }
        // issue the search command 
        driver.WriteByte(0xF0);

        uint32_t id_bit_number = 1;
        uint32_t last_zero = 0;
        uint32_t rom_byte_number = 0;
        
        // loop to do the search
        do {            
            
            uint8_t rom_byte_mask = 1;

            // read a bit and its complement
            uint32_t id_bit = driver.ReadTimeslot();
            uint32_t cmp_id_bit = driver.ReadTimeslot();

            // check for no devices on 1-wire
            if ((id_bit == 1) && (cmp_id_bit == 1)) {

                break;
            }
            else {

                uint8_t search_direction;
                
                // all devices coupled have 0 or 1
                if (id_bit != cmp_id_bit) {

                    search_direction = id_bit;  // bit write value for search
                }
                else {

                    // if this discrepancy if before the Last Discrepancy
                    // on a previous next then pick the same as last time
                    if (id_bit_number < LastDiscrepancy) {

                        search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
                    }
                    else {

                        // if equal to last pick 1, if not then pick 0
                        search_direction = (id_bit_number == LastDiscrepancy);
                    }

                    // if 0 was picked then record its position in LastZero
                    if (search_direction == 0) {

                        last_zero = id_bit_number;
                        // check for Last discrepancy in family
                        if (last_zero < 9) {
                            LastFamilyDiscrepancy = last_zero;
                        }
                    }
                }

                // set or clear the bit in the ROM byte rom_byte_number
                // with mask rom_byte_mask
                if (search_direction == 1)
                    ROM_NO[rom_byte_number] |= rom_byte_mask;
                else
                    ROM_NO[rom_byte_number] &= ~rom_byte_mask;

                // serial number search direction write bit
                driver.WriteTimeslot(search_direction);

                // increment the byte counter id_bit_number
                // and shift the mask rom_byte_mask
                id_bit_number++;
                rom_byte_mask <<= 1;
                // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
                if (rom_byte_mask == 0) {

                    crc8 = calcCRC_1wireQuick(crc8, ROM_NO[rom_byte_number]);  // accumulate the CRC

                    rom_byte_number++;
                    rom_byte_mask = 1;
                }
            }
        }
        while(rom_byte_number < 8);  // loop until through all ROM bytes 0-7
        // if the search was successful then
        if (!((id_bit_number < 65) || (crc8 != 0))) {
            
            // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
            LastDiscrepancy = last_zero;
            // check for last device
            if (LastDiscrepancy == 0)
                LastDeviceFlag = 1;
            search_result = 1;
        }
    }
    // if no device found then reset counters so next 'search' will be like a first
    if (!search_result || !ROM_NO[0]) {

        LastDiscrepancy = 0;
        LastDeviceFlag = 0;
        LastFamilyDiscrepancy = 0;
        search_result = 0;
    }
    return search_result;
}

// Verify the device with the ROM number in ROM_NO buffer is present.
// Return 1 : device verified present
//        0 : device not present
uint32_t OneWireSearch::verify()
{
    uint8_t rom_backup[8];
    uint32_t i, rslt, ld_backup, ldf_backup, lfd_backup;

    // keep a backup copy of the current state
    for (i = 0; i < 8; i++) {
        rom_backup[i] = ROM_NO[i];
    }
    ld_backup = LastDiscrepancy;
    ldf_backup = LastDeviceFlag;
    lfd_backup = LastFamilyDiscrepancy;

    // set search to find the same device
    LastDiscrepancy = 64;
    LastDeviceFlag = 0;
    if (search()) {
        // check if same device found
        rslt = 1;
        for (i = 0; i < 8; i++) {
            if (rom_backup[i] != ROM_NO[i]) {
                rslt = 0;
                break;
            }
        }
    }
    else {
        rslt = 0;
    }

    // restore the search state 
    for (i = 0; i < 8; i++) {
        ROM_NO[i] = rom_backup[i];
    }
    LastDiscrepancy = ld_backup;
    LastDeviceFlag = ldf_backup;
    LastFamilyDiscrepancy = lfd_backup;

    // return the result of the verify
    return rslt;
}

// Setup the search to find the device type 'family_code' on the next call
// to next() if it is present.
void OneWireSearch::targetSetup(uint8_t family_code)
{
    // set the search state to find SearchFamily type devices
    ROM_NO[0] = family_code;
    for (uint32_t i = 1; i < 8; i++) {
        ROM_NO[i] = 0;
    }
    LastDiscrepancy = 64;
    LastFamilyDiscrepancy = 0;
    LastDeviceFlag = 0;
}

// Setup the search to skip the current device type on the next call
// to OWNext().
void OneWireSearch::familySkipSetup(void)
{
    // set the Last discrepancy to last family discrepancy
    LastDiscrepancy = LastFamilyDiscrepancy;
    LastFamilyDiscrepancy = 0;
    // check for end of list
    if (LastDiscrepancy == 0) {
        LastDeviceFlag = 1;
    }
}

uint32_t OneWireSearch::searchAllDevices(void) {

    devicesConnectedCount = 0;
    uint32_t res = OneWireSearch::first();
    while (res) {
        for (uint32_t i = 0; i < 8; i++) {
			devicesConnected[devicesConnectedCount][i] = ROM_NO[i];
        }
		if (++devicesConnectedCount >= MaxDevicesConnected) {
			break;
		}
        res = OneWireSearch::next();
    }
	return devicesConnectedCount;
}

uint32_t OneWireSearch::searchOnly(uint8_t family) {
	
    devicesConnectedCount = 0;
    OneWireSearch::targetSetup(family);
    while (OneWireSearch::next()) {
        // check for incorrect type
        if (ROM_NO[0] != family) {
            break;
        }
        // store device found
        for (uint32_t i = 0; i < 8; i++) {
			devicesConnected[devicesConnectedCount][i] = ROM_NO[i];
        }
		if (++devicesConnectedCount >= MaxDevicesConnected) {
			break;
		}
    }
	return devicesConnectedCount;
}

uint32_t OneWireSearch::searchExcept(uint8_t family) {

    devicesConnectedCount = 0;
    uint32_t res = OneWireSearch::first();
    while (res) {

        // check for incorrect type
        if (ROM_NO[0] == family) {
			OneWireSearch::familySkipSetup();
		}
        else {
            // store device found
            for (uint32_t i = 0; i < 8; i++) {
				devicesConnected[devicesConnectedCount][i] = ROM_NO[i];
			}
            if (++devicesConnectedCount >= MaxDevicesConnected) {
				break;
			}
        }
        res = next();
    }
	return devicesConnectedCount;
}

