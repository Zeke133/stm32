#include <tacho.h>

static uint16_t prevTicks;
static uint16_t currentTicks;
static uint32_t rpm;

void TIM2_IRQHandler(void) {
 
    if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET) {
        /* Reset flag */
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

        prevTicks = currentTicks;
        currentTicks = TIM_GetCapture2(TIM2);
        uint32_t ticks = (currentTicks >= prevTicks) ? (currentTicks - prevTicks) : (UINT16_MAX - prevTicks + currentTicks);
        rpm = 6000000/ticks;
        
        // over-capture: The counter value has been captured in TIMx_CCR1 register while CC1IF flag was already set
        if (TIM_GetFlagStatus(TIM2, TIM_FLAG_CC2OF) != RESET) {
            TIM_ClearFlag(TIM2, TIM_FLAG_CC2OF);
            // ...
        }
    }
}

Tacho::Tacho() {

    // pins for capture
    GPIO_Init_My(GPIOA, GPIO_Pin_1, GPIO_Mode_IPU, GPIO_Speed_50MHz/* ??? */);

    /* Timer TIM2 enable clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* Timer TIM2 settings */
    TIM_TimeBaseInitTypeDef timer_base;
    TIM_TimeBaseStructInit(&timer_base);
    timer_base.TIM_Prescaler = 720;         // 72000000 / 720 = 100000 = 10uS for tick
    TIM_TimeBaseInit(TIM2, &timer_base);

    /* Signal capture settings:
    - Channel: 2
    - Count: Up
    - Source: Input
    - Divider: Disable
    - Filter: Disable */
    TIM_ICInitTypeDef timer_ic;
    timer_ic.TIM_Channel = TIM_Channel_2;
    //timer_ic.TIM_ICPolarity = TIM_ICPolarity_BothEdge; # !!! BothEdge not supported
    timer_ic.TIM_ICPolarity = TIM_ICPolarity_Rising;
    timer_ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
    timer_ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    timer_ic.TIM_ICFilter = 0;
    TIM_ICInit(TIM2, &timer_ic);

    /* Enable Interrupt by overflow */
    TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
    /* Timer TIM2 Enable */
    TIM_Cmd(TIM2, ENABLE);
    /* Enable Interrupt of Timer TIM2 */
    NVIC_EnableIRQ(TIM2_IRQn);
}

uint32_t Tacho::getRpm(void) {
    
    return rpm;
}


