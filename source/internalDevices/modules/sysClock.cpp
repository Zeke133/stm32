#include <sysClock.h>

void SetSysClockTo72(void) {

    LL_RCC_DeInit();

    // SYSCLK, HCLK, PCLK2 and PCLK1 configuration
    LL_UTILS_PLLInitTypeDef pllParams {         // 8 Mhz x 9 = 72 Mhz
        .PLLMul = LL_RCC_PLL_MUL_9,             // x 9
        .Prediv = LL_RCC_PREDIV_DIV_1           // / 1
    };
    LL_UTILS_ClkInitTypeDef clkParams {
        .AHBCLKDivider = LL_RCC_SYSCLK_DIV_1,   // 72 Mhz HCLK = SYSCLK
        .APB1CLKDivider = LL_RCC_APB1_DIV_2,    // 36 Mhz PCLK1 = HCLK/2
        .APB2CLKDivider = LL_RCC_APB2_DIV_1     // 72 Mhz PCLK2 = HCLK
    };
    LL_PLL_ConfigSystemClock_HSE(
        8000000,                                // 8 000 000 Hz - HSE freq
        LL_UTILS_HSEBYPASS_OFF,                 // Bypass OFF for crystal HSE
        &pllParams,
        &clkParams);

    // Flash timings configuration - needed???
    /* Prefetch Buffer */
    LL_FLASH_EnablePrefetch();
    /* FLASH Latency.
    FLASH_Latency_0 - 0 < SYSCLK<= 24 MHz
    FLASH_Latency_1 - 24 MHz < SYSCLK <= 48 MHz
    FLASH_Latency_2 - 48 MHz < SYSCLK <= 72 MHz */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

}
