#include <ds18b20.h>

Ds18b20::Ds18b20(Delay& timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, enum Resolution res)
    : OneWire(timer, GPIOx, GPIO_Pin) {

    useROM = 0;

    initialization(res);
}

Ds18b20::Ds18b20(Delay& timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t * _ROM, enum Resolution res)
    : OneWire(timer, GPIOx, GPIO_Pin) {

    useROM = 1;
    for (int i = 0; i < 8; i++) {
        ROM[i] = _ROM[i];
    }
    
    initialization(res);
}

void Ds18b20::initialization(enum Resolution res) {

    if (readPowerSupply() || readScratchpad()) {
        errorState = 1;
    }

    if (getResolution() != res) {

        setResolution(res);        
        saveSettings();
    }
}

uint8_t Ds18b20::isErrorState(void) {

    return errorState;
}

void Ds18b20::setResolution(enum Resolution res) {

    uint8_t conf = static_cast<uint8_t>(res);
    configRegister &= 0x9F;
    configRegister |= conf;

    if (writeScratchpad()) {
        errorState = 1;
    }
}

enum Ds18b20::Resolution Ds18b20::getResolution(void) {

    return static_cast<enum Resolution>(configRegister & 0x60);
}

void Ds18b20::setAlarmTemp(int8_t Th, int8_t Tl) {

    // To check !!! 777
    tempTrigerHigh = (Th < 0) ? (0xFF - Th) : Th;
    tempTrigerLow = (Tl < 0) ? (0xFF - Tl) : Tl;

    if (writeScratchpad()) {
        errorState = 1;
    }
}

uint16_t Ds18b20::getAlarmTemp(void) {

    uint16_t alarm = tempTrigerHigh;
    alarm <<= 8;
    alarm += tempTrigerLow;
    return alarm;
}

int32_t Ds18b20::getTemperature(void) {

    if (convertT() || readScratchpad()) errorState = 1;

    return ds18b20Temp2decimal(temperature);
}

enum Ds18b20::PowerMode Ds18b20::getPowerMode(void) {
    
    if (readPowerSupply()) errorState = 1;

    return powerMode;
}

void Ds18b20::saveSettings(void) {

    if (copyScratchpad()) errorState = 1;
}

void Ds18b20::restoreSettings(void) {

    if (recallEE() || readScratchpad()) errorState = 1;
}

uint8_t Ds18b20::selectDevice(void) {

    return useROM ? MatchROM(ROM) : SkipROM();
}

uint8_t Ds18b20::writeScratchpad(void) {

    if (selectDevice()) return 1;
    // Data must be transmitted least significant bit first.
    // All  three bytes MUST be written before the master issues a reset, or the data may be corrupted.
    WriteByte(0x4E);
    WriteByte(tempTrigerHigh);
    WriteByte(tempTrigerLow);
    WriteByte(configRegister);
    return 0;
}

uint8_t Ds18b20::readScratchpad(void) {

    if (selectDevice()) return 1;
    // The data transfer starts with the least significant bit of byte 0
    // and continues through the scratchpad until the 9th byte (byte 8 – CRC) is read.
    // The master may issue a reset to terminate reading at any time if only part of the scratchpad data is needed.
    WriteByte(0xBE);

    uint8_t scratchpad[9];
    uint8_t crc = 0;
    for (uint8_t i = 0; i < 9; i++) {

        scratchpad[i] = ReadByte();
        crc = calcCRC_1wireQuick(crc, scratchpad[i]);
    }

    // check CRC8
    if (crc != 0) {

        return 1;
    } else {

        temperature = scratchpad[1];
        temperature <<= 8;
        temperature += scratchpad[0];
        tempTrigerHigh = scratchpad[2];
        tempTrigerLow = scratchpad[3];
        configRegister = scratchpad[4];
        // BYTE 5 RESERVED (FFh)
        // BYTE 6 RESERVED
        // BYTE 7 RESERVED (10h)
        // BYTE 8 CRC*
        return 0;
    }

}

uint8_t Ds18b20::copyScratchpad(void) {

    if (selectDevice()) return 1;
    // If  the  device  is  being  used  in  parasite  power mode,
    // within 10μs (max) after this command is issued the 
    // master must enable a strong pullup on the 1-Wire bus for at least 10ms
    WriteByte(0x48);

    // Pull-Up for parasite powered on delay
    if (powerMode == PowerMode::parasite) {

        wait.ms(10);
    } else {
        // If the Ds18b20 is powered by an external supply,
        // the master can issue read time slots after the command and the Ds18b20 
        // will respond by transmitting a 0 while in progress and a 1 when done.
        for (uint16_t i = 10; i > 0; i--) {

            wait.ms(1);
            if (ReadTimeslot()) break;
        }
    }
    return 0;
}

uint8_t Ds18b20::recallEE(void) {

    if (selectDevice()) return 1;

    WriteByte(0xB8);

    for (uint16_t i = 10; i > 0; i--) {

        wait.ms(1);
        if (ReadTimeslot()) break;
    }
    return 0;
}

uint8_t Ds18b20::readPowerSupply(void) {

    if (selectDevice()) return 1;

    WriteByte(0xB4);

    powerMode = static_cast<enum Ds18b20::PowerMode>(ReadTimeslot());

    return 0;
}

uint8_t Ds18b20::convertT(void) {

    if (selectDevice()) return 1;

    // Time to Strong Pullup On t SPON Start convert T command issued 10μs
    WriteByte(0x44);

    // Pull-Up for parasite powered on delay
    if (powerMode == PowerMode::parasite) {

        // Bus is pulled to Vsup by every Read/Write command
        // Wait max convertion time
        switch (getResolution()) {

            case Resolution::_9bit: wait.ms(94);
                break;

            case Resolution::_10bit: wait.ms(188);
                break;

            case Resolution::_11bit: wait.ms(375);
                break;

            case Resolution::_12bit: wait.ms(750);
                break;

            default:
                break;
        }
    } else {
        // If the Ds18b20 is powered by an external supply,
        // the master can issue read time slots after the Convert T command and the Ds18b20 
        // will respond by transmitting a 0 while the temperature conversion is in progress
        // and a 1 when the conversion is done.
        for (uint16_t i = 1000; i > 0; i -= 10) {

            wait.ms(10);
            if (ReadTimeslot()) break;
        }
    }
    return 0;
}


