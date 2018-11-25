#include <bmp280.h>

Bmp280::Bmp280(I2c& i2c, IDelayer& delayer, OStream& cout, uint8_t address)
    : i2c(i2c),
    delayer(delayer),
    cout(cout),
    address(address) {

    initialization();
}

void Bmp280::initialization() {

    cout << "\nBmp280 initialization";
    /*
    i2c.startTransmit(address << 1);
    i2c.write(static_cast<uint8_t>(Bmp280::Register::id));
    
    i2c.startTransmit(address << 1, I2C_Direction_Receiver);
    uint8_t buf;
    buf = i2c.read();
    i2c.stopTransmit();
    
    cout << "\nRead:";
    cout << itoa(buf,16,2) << ' ';
    */
}
