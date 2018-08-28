#include <led.h>

LED::LED(GPIO_TypeDef* port, uint16_t pin)
    : port(port), pin(pin) {

    GPIO_Init_My(port, pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    state = 1;
}

void LED::on() {

    GPIO_ResetBits(port, pin);
    state = 1;
}

void LED::off() {

    GPIO_SetBits(port, pin);
    state = 0;
}

void LED::invert() {

    state ? off() : on();
}
