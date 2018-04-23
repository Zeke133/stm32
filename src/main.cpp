#include <SysClock.h>
#include <led.h>
#include <usart.h>
#include <DS_1wire.h>

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
    Delay delayObj;
    USART usart1Obj(1);
    // Take global resourses
    USART& usart = *usart1;
    Delay& wait = *delay;

    // Init LED
    LED led(GPIOC, GPIO_Pin_13);

    // usart test
    usart << "Hello.\r\nUSART1 is ready.\r\n";

    // 1-wire
    unsigned char buf[8];

    DS_1Wire_controller one_wire(GPIOA, GPIO_Pin_8);

    usart << "one_wire send: 0x33, ";
    if (one_wire.GetCodes(0x33, buf)) {
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
        DS_1Wire_controller oneWire(GPIOA, GPIO_Pin_8, timeouts[0], timeouts[1], timeouts[2]);
        usart << "one_wire send: 0x33, ";
        if (one_wire.GetCodes(0x33, buf)) {
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




