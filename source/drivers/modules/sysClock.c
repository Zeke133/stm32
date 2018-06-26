#include <SysClock.h>


void SetSysClockTo72(void) {

    ErrorStatus HSEStartUpStatus;

    // SYSCLK, HCLK, PCLK2 and PCLK1 configuration
    RCC_DeInit();

    RCC_HSEConfig( RCC_HSE_ON);

    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus == SUCCESS) {

        /* Prefetch Buffer */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        /* FLASH Latency.
        FLASH_Latency_0 - 0 < SYSCLK<= 24 MHz
        FLASH_Latency_1 - 24 MHz < SYSCLK <= 48 MHz
        FLASH_Latency_2 - 48 MHz < SYSCLK <= 72 MHz */
        FLASH_SetLatency(FLASH_Latency_2);

        /* HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config(RCC_HCLK_Div2);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        RCC_PLLCmd( ENABLE);

        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {}

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        while (RCC_GetSYSCLKSource() != 0x08) {}
    }
    else {
        while (1) {}
    }
}

