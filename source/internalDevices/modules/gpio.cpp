#include <gpio.h>

/**
  * @brief  Configure GPIO for a dedicated pins on dedicated port.
  * @param  port GPIO Port
  * @param  pins This parameter can be one of the following values:
  *         LL_GPIO_PIN_0 ... LL_GPIO_PIN_15 or combination '|' of pins
  * @param  mode This parameter can be one of the following values:
  *         LL_GPIO_MODE_ANALOG
  *         LL_GPIO_MODE_FLOATING
  *         LL_GPIO_MODE_INPUT
  *         LL_GPIO_MODE_OUTPUT
  *         LL_GPIO_MODE_ALTERNATE
  * @param  outType This parameter can be one of the following values:
  *         LL_GPIO_OUTPUT_PUSHPULL
  *         LL_GPIO_OUTPUT_OPENDRAIN
  * @param  pull This parameter can be one of the following values:
  *         LL_GPIO_PULL_DOWN
  *         LL_GPIO_PULL_UP
  * @param  speed This parameter can be one of the following values:
  *         LL_GPIO_SPEED_FREQ_LOW
  *         LL_GPIO_SPEED_FREQ_MEDIUM
  *         LL_GPIO_SPEED_FREQ_HIGH - 50Mhz
  * @retval None
  */
void GPIO::initPins(GPIO_TypeDef* port, uint32_t pins, uint32_t mode, uint32_t outType, uint32_t pull, uint32_t speed) {

    uint32_t APB2_Periph;

    if (port == GPIOA) APB2_Periph = LL_APB2_GRP1_PERIPH_GPIOA;
    else if (port == GPIOB) APB2_Periph = LL_APB2_GRP1_PERIPH_GPIOB;
    else if (port == GPIOC) APB2_Periph = LL_APB2_GRP1_PERIPH_GPIOC;
    else if (port == GPIOD) APB2_Periph = LL_APB2_GRP1_PERIPH_GPIOD;
    else if (port == GPIOE) APB2_Periph = LL_APB2_GRP1_PERIPH_GPIOE;
    else return;

    // Turn ON Clock for specified port
    LL_APB2_GRP1_EnableClock(APB2_Periph);

    LL_GPIO_InitTypeDef params {
        .Pin = pins,
        .Mode = mode,
        .Speed = speed,
        .OutputType = outType,
        .Pull = pull
    };

    LL_GPIO_Init(port, &params);
}
