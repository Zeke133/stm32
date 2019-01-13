#include <_tacho.h>

uint32_t Tacho::currentTicks;
uint32_t Tacho::ticks;

void TIM2_IRQHandler(void) {

    if (LL_TIM_IsActiveFlag_CC2(TIM2)) {    // Capture/Compare 2 interrupt flag

        LL_TIM_ClearFlag_CC2(TIM2);

        uint16_t prevTicks = Tacho::currentTicks;
        Tacho::currentTicks = LL_TIM_IC_GetCaptureCH2(TIM2);
        Tacho::ticks = (Tacho::currentTicks >= prevTicks) ? (Tacho::currentTicks - prevTicks) : (UINT16_MAX - prevTicks + Tacho::currentTicks);
        
        // over-capture: The counter value has been captured in TIMx_CCR2 register while CC2IF flag was already set
        if (LL_TIM_IsActiveFlag_CC2OVR(TIM2)) {
            
            LL_TIM_ClearFlag_CC2OVR(TIM2);
            // ...
        }

        // if (GPIO_ReadInputDataBit(GPIOC, LL_GPIO_PIN_13)) LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13);
        // else LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_13);
    }
}

Tacho::Tacho() {

    // Pins configuration for capture
    GPIO::initPins(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_INPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP, LL_GPIO_SPEED_FREQ_HIGH);

    /* Timer TIM2 enable clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

    /* Timer TIM2 settings */
    LL_TIM_SetPrescaler(TIM2, timerPrescaler);

    /* Signal capture settings:
    - Channel: 2
    - Count: Up
    - Source: Input
    - Divider: Disable
    - Filter: Disable */
    LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);


    /* Enable Interrupt by overflow */
    LL_TIM_EnableIT_CC2(TIM2);
    /* Timer TIM2 Enable */
    LL_TIM_EnableCounter(TIM2);
    /* Enable Interrupt of Timer TIM2 */
    NVIC_EnableIRQ(TIM2_IRQn);
}

uint32_t Tacho::getRpm(void) {

    return 60 * timerClk / ticks;
}

uint32_t Tacho::getTicksPerRotation(void) {

    return ticks;
}

uint32_t Tacho::getSpeed(void) {

    // https://ru.wikipedia.org/wiki/%D0%9C%D0%B5%D1%82%D1%80_%D0%B2_%D1%81%D0%B5%D0%BA%D1%83%D0%BD%D0%B4%D1%83
    return (100000*rotateLength*3600)/(1000000*ticks);
}

void Tacho::setRotationLength(uint32_t len) {

    rotateLength = len;
}
