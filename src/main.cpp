#include <SysClock.h>
#include <led.h>
#include <usart.h>
#include <dallas_1wire.h>
#include <i2c.h>
#include <lcd.h>
#include <delay.h>


#include <convertation.h>

void TakeTimeouts(uint8_t* timeoutsPtr) {
    
    USART& usart = *usart1;
    Delay& wait = *delay;

    usart << "\xd\xaWait for receive of timeouts...";

    if (usart.getCount() < 9) {
        wait.ms(100);
    }

    usart << "\xd\xaReceived:";
    for (int i = 0; i < 9; i++) {
        usart << "\xd\xa-";
        usart << itoa((usart.getData())[i], 10);

        timeoutsPtr[i] = (usart.getData())[i];
    }

}

int main(void) {

	SetSysClockTo72();

    // Init resourses
    Delay delayObj;                 // Delay based on SysTickTimer
    USART usart1Obj(1);             // USART1
    LED led(GPIOC, GPIO_Pin_13);    // LED on gpio
    I2c i2cPort(1, 0x27);           // I2C parallel converter

    Lcd lcd(i2cPort, delayObj, 2);  // LCD on I2C adaptor

    // Take global resourses
    USART& usart = *usart1;
    Delay& wait = *delay;

    // usart test
    usart << "Hello.\r\nUSART1 is ready.\r\n";

    // lcd test
    lcd.backlightSet(1);
    lcd.puts("Hello LCD");

    // 1-wire test
    unsigned char buf[8];

    Dallas_1wire_controller one_wire(wait, GPIOA, GPIO_Pin_8);

    usart << "one_wire send: 0x33, ";
    if (one_wire.ReadROM(buf)) {
        usart << "Succ";
    } else {
        usart << "Err";
    }

    usart << "\r\none_wire resp: ";
    for(int i = 0; i < 8; i++) {
        usart << itoa(buf[i],16);
        usart << ",";
    }

    uint8_t timeouts[3][3];

    while (1) {

        // Delay metering check
        wait.ms(1000);
        led.invert();

        // Take user timeouts for one-wire bus
        TakeTimeouts((uint8_t*)timeouts);

        // Run and print results
        Dallas_1wire_controller oneWire(wait, GPIOA, GPIO_Pin_8, timeouts[0], timeouts[1], timeouts[2]);
        usart << "one_wire send: 0x33, ";
        if (one_wire.ReadROM(buf)) {
            usart << "Succ";
        } else {
            usart << "Err";
        }
        usart << "\r\none_wire resp: ";
        for(int i = 0; i < 8; i++) {
            usart << itoa(buf[i],16);
            usart << ",";
        }
    }

}




