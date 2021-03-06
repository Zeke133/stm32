#include <oneWire.h>

OneWire::OneWire(IDelayer &delayer, GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
    : delayer(delayer),
      GPIOx(GPIOx),
      GPIO_Pin(GPIO_Pin) {

    GPIO::initPins(GPIOx, GPIO_Pin, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_DOWN, LL_GPIO_SPEED_FREQ_HIGH);
}

// to gpio PINS driver!

void OneWire::WriteTimeslot(uint8_t bit) {

    // Both types of write time slots are initiated by the
    // master pulling the 1-Wire bus low
    LL_GPIO_ResetOutputPin(GPIOx, GPIO_Pin);
    delayer.us(WriteTOs[0]); // should release within 15us

    if (bit) {
        // To generate a Write 1 time slot, after pulling the 1-Wire
        // bus low, the bus master must release the 1-Wire bus within 15us
        LL_GPIO_SetOutputPin(GPIOx, GPIO_Pin);
    }
    // To generate a Write 0 time slot, after pulling the 1-Wire bus low, the bus master must
    // continue to hold the bus low for the duration of the time slot (at least 60us).
    delayer.us(WriteTOs[1]);

    // All write time slots must be a minimum of 60us in duration
    // with a minimum of a 1us recovery time between individual write slots.
    // Both types of write time slots are initiated by the master pulling the 1-Wire bus low
    LL_GPIO_SetOutputPin(GPIOx, GPIO_Pin);
    delayer.us(WriteTOs[2]); // minimum 1us recovery between time slots
}

uint8_t OneWire::ReadTimeslot(void) {

    // A read time slot is initiated by the master device pulling the
    // 1-Wire bus low for a minimum of 1us and then releasing the bus
    LL_GPIO_ResetOutputPin(GPIOx, GPIO_Pin);
    delayer.us(ReadTOs[0]);
    // After the master initiates the read time slot, the Ds18b20 will begin transmitting a 1
    // or 0 on bus
    LL_GPIO_SetOutputPin(GPIOx, GPIO_Pin);
    // Output data from the Ds18b20 is valid for 15us after the falling edge that
    // initiated the read time slot. Therefore, the master must
    // release the bus and then sample the bus state within
    // 15us from the start of the slot.
    delayer.us(ReadTOs[1]);
    uint8_t stat = LL_GPIO_IsInputPinSet(GPIOx, GPIO_Pin);
    delayer.us(ReadTOs[2]);
    return stat;
}

// 0 - success
// 1 - error
uint8_t OneWire::Initialization(void) {

    // reset pulse by pulling the 1-Wire bus low for a minimum of 480us
    LL_GPIO_ResetOutputPin(GPIOx, GPIO_Pin);
    delayer.us(InitTOs[0] * 10);

    // When the Ds18b20 detects this rising edge, it waits 15us to 60us and then
    // transmits a presence pulse by pulling the 1-Wire bus low for 60us to 240us.
    LL_GPIO_SetOutputPin(GPIOx, GPIO_Pin);
    delayer.us(InitTOs[1] * 10);

    uint8_t stat = LL_GPIO_IsInputPinSet(GPIOx, GPIO_Pin);
    if (stat) return 1;

    delayer.us(InitTOs[2] * 10);
    return 0;
}

void OneWire::WriteByte(uint8_t byte) {

    for (int i = 0; i < 8; i++) {
        WriteTimeslot(byte & 0x01);
        byte >>= 1;
    }
}

uint8_t OneWire::ReadByte(void) {

    uint8_t result = 0;
    for (int i = 0; i < 8; i++) {
        result >>= 1;
        result |= (ReadTimeslot() << 7);
    }
    return result;
}

uint8_t OneWire::ReadROM(uint8_t *resp) {

    uint8_t crc = 0;

    if (Initialization()) return 1;

    WriteByte(0x33);
    for (int i = 0; i < 8; i++) {
        resp[i] = ReadByte();
        crc = calcCRC_1wireQuick(crc, resp[i]);
    }
    return (crc == 0 ? 0 : 1);
}

uint8_t OneWire::MatchROM(const uint8_t *romCode /*64 bit RomCode*/) {

    if (Initialization()) return 1;

    WriteByte(0x55);
    for (int i = 0; i < 8; i++) {
        WriteByte(romCode[i]);
    }
    return 0;
}

uint8_t OneWire::SkipROM(void) {

    if (Initialization()) return 1;

    WriteByte(0xCC);
    return 0;
}

// not implemented
// for alpha implementation of 1-Wire search algurithms see oneWireSearch.h
// uint8_t OneWire::AlarmSearch(void) { /* 0xEC */ }
