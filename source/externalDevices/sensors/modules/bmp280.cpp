#include <bmp280.h>

Bmp280::Bmp280(I2c& i2c, IDelayer& delayer, OStream& cout, uint8_t address)
    : i2c(i2c),
    delayer(delayer),
    cout(cout),
    address(address) {

    initialization();
}

void Bmp280::initialization() {

    cout << "\r\n1";
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    cout << "\r\n2";
    I2C_GenerateSTART(I2C1, ENABLE);
    cout << "\r\n3";
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    cout << "\r\n4";
    I2C_Send7bitAddress(I2C1, 0x76 << 1, I2C_Direction_Transmitter);
    cout << "\r\n5";
    while (1) {
        if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
            cout << "\r\nTrm_sel"; break;
        } else
        if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
            cout << "\r\nRcv_sel"; break;
        }
    }
    cout << "\r\n6";
    i2c.write(static_cast<uint8_t>(Bmp280::Register::id));
    cout << "\r\n7";
    I2C_GenerateSTART(I2C1, DISABLE);
    cout << "\r\n71";
    // while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    cout << "\r\n8";
    I2C_GenerateSTART(I2C1, ENABLE);
    cout << "\r\n9";
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    cout << "\r\n10";
    I2C_Send7bitAddress(I2C1, 0x76 << 1, I2C_Direction_Receiver);
    cout << "\r\n11";
    while (1) {
        if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
            cout << "\r\nTrm_sel"; break;
        } else
        if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
            cout << "\r\nRcv_sel"; break;
        }
    }
    cout << "\r\n12";
    uint8_t buf;
    buf = i2c.read();
    cout << "\r\n13";
    i2c.stopTransmit();
    
    cout << "\nRead:";
    cout << itoa(buf,16,2) << ' ';
    
}
