#include <bmp280.h>

Bmp280::Bmp280(I2c& i2c, IDelayer& delayer, OStream& cout, uint8_t address)
    : i2c(i2c),
    delayer(delayer),
    cout(cout),
    address(address) {

    initialization();
}

void Bmp280::initialization() {

    uint8_t buf;

    cout << "\r\nStart";
    
    for (int i = 10; i > 0; --i) {
        if (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
            cout << "\n!I2C_FLAG_BUSY";
            break;
        }
        else cout << "\nI2C_FLAG_BUSY";
    }
    
    I2C_GenerateSTART(I2C1, ENABLE);
    for (int i = 10; i > 0; --i) {
        if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
            cout << "\nI2C_EVENT_MASTER_MODE_SELECT";
            break;
        }
        else cout << "\n!I2C_EVENT_MASTER_MODE_SELECT";
    }
    
    I2C_Send7bitAddress(I2C1, 0x76 << 1, I2C_Direction_Transmitter);
    for (int i = 10; i > 0; --i) {
        if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
            cout << "\nI2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED";
            break;
        }
        else cout << "\n!I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED";
    }
    
    I2C_SendData(I2C1, static_cast<uint8_t>(Bmp280::Register::id));
    for (int i = 10; i > 0; --i) {
        if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
            cout << "\nI2C_EVENT_MASTER_BYTE_TRANSMITTED";
            break;
        }
        else cout << "\n!I2C_EVENT_MASTER_BYTE_TRANSMITTED";
    }
        
    // --- restart

    I2C_GenerateSTART(I2C1, ENABLE);
    for (int i = 10; i > 0; --i) {
        if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
            cout << "\nI2C_EVENT_MASTER_MODE_SELECT";
            break;
        }
        else cout << "\n!I2C_EVENT_MASTER_MODE_SELECT";
    }
    
    I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Next); // check with I2C_NACKPosition_Next
    cout << "\nI2C_NACKPosition_Next";
    
    I2C_Send7bitAddress(I2C1, 0x76 << 1, I2C_Direction_Receiver);
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    
    for (int i = 10; i > 0; --i) {
        if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
            cout << "\nI2C_EVENT_MASTER_RECEIVER_MODE_SELECTED";
            break;
        }
        else cout << "\n!I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED";
    }
    
    I2C_GenerateSTOP(I2C1, ENABLE);
    cout << "\nI2C_GenerateSTOP";
    
    for (int i = 5, flags; i > 0; --i) {
        flags = I2C_GetLastEvent(I2C1);
        cout << "\nI2C_GetLastEvent:" << itoa(flags, 16, 8);
        if (flags & I2C_FLAG_RXNE) {
            buf = I2C_ReceiveData(I2C1);
            cout << "\nRead:";
            cout << itoa(buf,16,2); // 58 BMP280 60 BME280
        }
    }
    
    // case with Bytes to rec N>2
    // EV7_2: BTF = 1, DataN-2 in DR and DataN-1 in shift register, program ACK = 0, Read DataN-2 in DR.
    // Program STOP = 1, read DataN-1.
    
}
