#include "drivers/inc/SysClock.h"
#include "drivers/inc/led.h"
#include "drivers/inc/uart.h"

#include "utils/inc/delay.h"



int main(void) {

	SetSysClockTo72();

    // Init LED
    LED led(GPIOC, GPIO_Pin_13);
    ledGlobalPtr = &led;

    // Initialize USART
    usart_init();
    USARTSend(" Hello.\r\nUSART1 is ready.\r\n");

    while (1) {
        Delay(1000);
        led.invert();
    }

}




