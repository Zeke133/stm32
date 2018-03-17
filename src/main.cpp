

#include "drivers/inc/SysClock.h"
#include "drivers/inc/led.h"
#include "drivers/inc/uart.h"
#include "drivers/inc/DS_1wire.h"

#include "utils/inc/delay.h"


int main(void) {

	SetSysClockTo72();

    // Init LED
    LED led(GPIOC, GPIO_Pin_13);

    // Initialize USART
    usart_init();
    USARTSend(" Hello.\r\nUSART1 is ready.\r\n");

    // Init 1-Wire
    USARTSend("\xd\xa Initialisation of 1-Wire...");

    DS_1Wire_controller ds_one_wire(GPIOA, GPIO_Pin_8);

    unsigned char rd[8];
    if (ds_one_wire.GetCodes(0x33, rd)) {
    	USARTSend("\xd\xa Get succ");
        USARTSend("\xd\xa read[8]:");
        char pr[2] = "0";
        for(int i=0; i<8; i++) {
            pr[0] = rd[i] + 0x30;
            USARTSend(pr);
        }
    } else {
    	USARTSend("\xd\xa Get err");
    }	

    while (1) {
        if (RX_FLAG_END_LINE == 1) {
            // Reset END_LINE Flag
            RX_FLAG_END_LINE = 0;

            USARTSend("\r\nI has received a line:\r\n");
            USARTSend(RX_BUF);
            USARTSend("\r\n");

            if (RX_BUF[0] == 'O' &&
                RX_BUF[1] == 'N' &&
                RX_BUF[2] == '\r') {
                USARTSend("\r\nTHIS IS A COMMAND \"ON\"!!!\r\n");
                led.on();
            }

            if (RX_BUF[0] == 'O' &&
                RX_BUF[1] == 'F' &&
                RX_BUF[2] == 'F' &&
                RX_BUF[3] == '\r') {
                USARTSend("\r\nTHIS IS A COMMAND \"OFF\"!!!\r\n");
                led.off();
            }

            clear_RXBuffer();
        }
    }

}




