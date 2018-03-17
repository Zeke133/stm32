#include "inc/led.h"



LED::LED(GPIO_TypeDef* port, uint16_t pin)
	: port(port), pin(pin) {

	GPIO_Init_My(port, pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
}

void LED::on() {
    GPIO_ResetBits(port, pin);
}

void LED::off() {
    GPIO_SetBits(port, pin);
}


