#include <bmp280.h>

Bmp280::Bmp280(I2c& i2c, IDelayer& delayer, OStream& cout, uint8_t address)
    : i2c(i2c),
    delayer(delayer),
    cout(cout),
    address(address) {

    initialization();
}

void Bmp280::initialization() {

    cout << "\r\nStart";
    
    if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
        cout << "\nI2C_FLAG_BUSY";
    else
        cout << "\n!I2C_FLAG_BUSY";
    
    I2C_GenerateSTART(I2C1, ENABLE);
    cout << "\nI2C_GenerateSTART";
    
    if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
        cout << "\nI2C_EVENT_MASTER_MODE_SELECT";
    else
        cout << "\n!I2C_EVENT_MASTER_MODE_SELECT";
    
    I2C_Send7bitAddress(I2C1, 0x76 << 1, I2C_Direction_Transmitter);
    cout << "\nI2C_Send7bitAddress";
    
    while (1) {
        if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
            cout << "\nI2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED";
            break;
        }
    }
    
    I2C_SendData(I2C1, static_cast<uint8_t>(Bmp280::Register::id));
    cout << "\nI2C_SendData";
    if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        cout << "\nI2C_EVENT_MASTER_BYTE_TRANSMITTED";
    else
        cout << "\n!I2C_EVENT_MASTER_BYTE_TRANSMITTED";
    
    // maybe not needed
    // I2C_GenerateSTOP(I2C1, ENABLE);
    // cout << "\nGenSTOP";
    
    // this stuck
    // while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    I2C_GenerateSTART(I2C1, ENABLE);
    cout << "\nI2C_GenerateSTART";
    
    if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
        cout << "\nI2C_EVENT_MASTER_MODE_SELECT";
    else
        cout << "\n!I2C_EVENT_MASTER_MODE_SELECT";
    
    I2C_Send7bitAddress(I2C1, 0x76 << 1, I2C_Direction_Receiver);
    cout << "\nI2C_Send7bitAddress";
    
    /*
    cout << "\nI2C_GetLastEvent";
    cout << "\n" << itoa(I2C_GetLastEvent(I2C1), 16, 8);
    cout << "\n" << itoa(I2C_GetLastEvent(I2C1), 16, 8);
    cout << "\n" << itoa(I2C_GetLastEvent(I2C1), 16, 8);
    */
    
    while (1) {
        if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
            cout << "\nI2C_EVENT_MASTER_RECEIVER_MODE_SELECTED";
            break;
        }
    }
    
    I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Next);
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    uint8_t buf = I2C_ReceiveData(I2C1);
    cout << "\nRead:";
    cout << itoa(buf,16,2) << ' '; // 58 BMP280 60 BME280
    
    //while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
    //    cout << "\n" << itoa(I2C_ReceiveData(I2C1),16,2);
    //}
    I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Next);
    I2C_AcknowledgeConfig(I2C1, DISABLE);

    I2C_GenerateSTOP(I2C1, ENABLE);
    cout << "\nI2C_GenerateSTOP";
    
    cout << "\nI2C_GetLastEvent";
    cout << "\n" << itoa(I2C_GetLastEvent(I2C1), 16, 8);
    cout << "\n" << itoa(I2C_GetLastEvent(I2C1), 16, 8);
    cout << "\n" << itoa(I2C_GetLastEvent(I2C1), 16, 8);
    cout << "\n" << itoa(I2C_GetLastEvent(I2C1), 16, 8);
    cout << "\n" << itoa(I2C_GetLastEvent(I2C1), 16, 8);
    cout << "\n" << itoa(I2C_GetLastEvent(I2C1), 16, 8);
    cout << "\n" << itoa(I2C_GetLastEvent(I2C1), 16, 8);
    
    if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        cout << "\nI2C_EVENT_MASTER_BYTE_TRANSMITTED";
    else
        cout << "\n!I2C_EVENT_MASTER_BYTE_TRANSMITTED";
    
    
    
}
