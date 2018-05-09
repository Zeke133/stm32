#include <ds18b20.h>



DS18B20::DS18B20(Delay& timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t InitTOs[3], uint8_t WriteTOs[3], uint8_t ReadTOs[3], enum Resolution res)
    : OneWire(timer, GPIOx, GPIO_Pin, InitTOs, WriteTOs, ReadTOs) {

    // trying 2 times to read
    if (readScratchpad()) {

        readScratchpad();
    }

    if (configRegister != res) {

        configRegister = res;
        writeScratchpad();
        copyScratchpad();
    }
}

void DS18B20::setResolution(enum Resolution res) {

    configRegister = res;
}

enum DS18B20::Resolution DS18B20::getResolution(void) {

    return configRegister;
}

void DS18B20::setAlarmTemp(int8_t Th, int8_t Tl) {

    // To check!
    tempTrigerHigh = (Th < 0) ? (0xFF - Th) : Th;
    tempTrigerLow = (Tl < 0) ? (0xFF - Tl) : Tl;
}

void DS18B20::writeScratchpad(void) {

    // Data must be transmitted least significant bit first.
    // All  three bytes MUST be written before the master issues a reset, or the data may be corrupted.
    WriteByte(0x4E);
    WriteByte(tempTrigerHigh);
    WriteByte(tempTrigerLow);
    WriteByte(static_cast<uint8_t>(configRegister));
}

uint8_t DS18B20::readScratchpad(void) {

    // The data transfer starts with the least significant bit of byte 0
    // and continues through the scratchpad until the 9th byte (byte 8 – CRC) is read.
    // The master may issue a reset to terminate reading at any time if only part of the scratchpad data is needed.
    WriteByte(0xBE);

    uint8_t scratchpad[9];
    uint8_t crc = 0;
    for (uint8_t i = 0; i < 9; i++) {

        scratchpad[i] = ReadByte();
        calcCRC_1wireQuick(crc, scratchpad[i]);
    }

    // check CRC8
    if (crc != scratchpad[8]) {

        return 1;
    } else {

        temperature = scratchpad[1];
        temperature <<= 8;
        temperature += scratchpad[0];
        tempTrigerHigh = scratchpad[2];
        tempTrigerLow = scratchpad[3];
        configRegister = static_cast<enum DS18B20::Resolution>(scratchpad[4]);
        // BYTE 5 RESERVED (FFh)
        // BYTE 6 RESERVED
        // BYTE 7 RESERVED (10h)
        // BYTE 8 CRC*
        return 0;
    }

}

void DS18B20::copyScratchpad(void) {

    // If  the  device  is  being  used  in  parasite  power mode,
    // within 10μs (max) after this command is issued the 
    // master must enable a strong pullup on the 1-Wire bus for at least 10ms
    WriteByte(0x48);

    // Pull-Up for parasite powered on delay
    if (powerMode == PowerMode::parasite) {

        wait.ms(10);
    } else {
        // If the DS18B20 is powered by an external supply,
        // the master can issue read time slots after the command and the DS18B20 
        // will respond by transmitting a 0 while in progress and a 1 when done.
        for (uint16_t i = 10; i > 0; i--) {

        wait.ms(1);
        if (ReadTimeslot()) break;
    }
    }
}

void DS18B20::recallEE(void) {

    WriteByte(0xB8);

    for (uint16_t i = 10; i > 0; i--) {

        wait.ms(1);
        if (ReadTimeslot()) break;
    }

}

enum DS18B20::PowerMode DS18B20::readPowerSupply(void) {

    WriteByte(0xB4);

    powerMode = static_cast<enum DS18B20::PowerMode>(ReadTimeslot());

    return powerMode;
}

void DS18B20::convertT(void) {

    // Time to Strong Pullup On t SPON Start convert T command issued 10μs
    WriteByte(0x44);

    // Pull-Up for parasite powered on delay
    if (powerMode == PowerMode::parasite) {

        // Bus is pulled to Vsup by every Read/Write command
        // Wait max convertion time
        switch (configRegister) {

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
        // If the DS18B20 is powered by an external supply,
        // the master can issue read time slots after the Convert T command and the DS18B20 
        // will respond by transmitting a 0 while the temperature conversion is in progress
        // and a 1 when the conversion is done.
        for (uint16_t i = 1000; i > 0; i -= 10) {

            wait.ms(10);
            if (ReadTimeslot()) break;
        }
    }

}


