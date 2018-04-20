#include <SysClock.h>
#include <led.h>
#include <usart.h>
#include <DS_1wire.h>

#include <delay.h>
#include <convertation.h>



int main(void) {

	SetSysClockTo72();

    // Init LED
    LED led(GPIOC, GPIO_Pin_13);

    // Initialize USART
    USART usart1(1);
    usart1 << "Hello.\r\nUSART1 is ready.\r\n";

    // 1-wire
    unsigned char buf[8];

    DS_1Wire_controller one_wire(GPIOA, GPIO_Pin_8);

    usart1 << "one_wire send: 0x33 \r\n";
    one_wire.GetCodes(0x33, buf); //Read Rom [33h]

    usart1 << "one_wire resp: ";
    for(int i = 0; i < 8; i++) {
        usart1 << itoa(buf[i],16);
        usart1 << ",";
    }

    while (1) {

        // Delay metering check
        Delay(1000);
        led.invert();

        // Take user timeouts for one-wire bus
        //...

        // Run and print results
        //...

    }

}




