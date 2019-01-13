#include <bmp280.h>

Bmp280::Bmp280(I2c& i2c, IDelayer& delayer, OStream& cout, uint8_t address)
    : i2c(i2c),
      delayer(delayer),
      cout(cout),
      address(address) {

    initialization();
}

void Bmp280::initialization() {

    uint8_t receivedData[100];
    for (uint32_t i = 0; i < sizeof receivedData; ++i) {
        receivedData[i] = 0x33;
    }
    // I2c::Status status;
    
    // can try to use without i2c.stop inside
    i2c.send(address << 1, (uint8_t)Bmp280::Register::id);

    // ---
    // receivedData[0] = i2c.receive(address << 1);
    // ---
    i2c.receiveBufferized(address << 1, receivedData, 10);
    // ---

    cout << "\r\nreceivedData: ";
    for (uint32_t i = 0; i < 13; ++i) {
        cout << itoa(receivedData[i],16,2) << '_';
    }
    
    // case with Bytes to rec N>2
    // EV7_2: BTF = 1, DataN-2 in DR and DataN-1 in shift register, program ACK = 0, Read DataN-2 in DR.
    // Program STOP = 1, read DataN-1.
    
}
