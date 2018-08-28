#include <gpio.h>

void GPIO_Init_My(GPIO_TypeDef* port, uint16_t pins, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed) {

    uint32_t RCC_APB2Periph_GPIOx;

    if (port == GPIOA) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
    else if (port == GPIOB) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
    else if (port == GPIOC) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOC;
    else if (port == GPIOD) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOD;
    else if (port == GPIOE) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOE;
    else if (port == GPIOF) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOF;
    else if (port == GPIOG) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOG;
    else return;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx , ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = pins;
    GPIO_InitStructure.GPIO_Mode = mode;
    GPIO_InitStructure.GPIO_Speed = speed;
    GPIO_Init( port , &GPIO_InitStructure);
}
