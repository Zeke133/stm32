#include <sysClock.h>

void SetSysClockTo72(void) {

    LL_RCC_DeInit();

    LL_RCC_HSE_DisableBypass();
    LL_RCC_HSE_Enable();
    while(!LL_RCC_HSE_IsReady()) {}

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLL_MUL_9);
    LL_RCC_PLL_Enable();
    while(!LL_RCC_PLL_IsReady()) {}

    // Flash timings configuration - needed???
    // FLASH Latency.
    // FLASH_Latency_0 - 0 < SYSCLK<= 24 MHz
    // FLASH_Latency_1 - 24 MHz < SYSCLK <= 48 MHz
    // FLASH_Latency_2 - 48 MHz < SYSCLK <= 72 MHz
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
    LL_FLASH_EnablePrefetch();
    
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);    // 72 Mhz HCLK = SYSCLK
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);     // 36 Mhz PCLK1 = HCLK/2
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);     // 72 Mhz PCLK2 = HCLK

    SystemCoreClockUpdate();

}
