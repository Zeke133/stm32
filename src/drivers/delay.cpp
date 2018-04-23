#include <delay.h>

Delay* delay;

// SysTick_Handler function will be called every 1 ms
void SysTick_Handler() {

	if (Delay::msTicks > 0)
		Delay::msTicks--;
}

// Class implementation
uint32_t Delay::msTicks = 0;

Delay::Delay() {

	// Update SystemCoreClock value
	SystemCoreClockUpdate();

	// Configure the SysTick timer to overflow every 1 ms
	SysTick_Config(SystemCoreClock / 1000);

	// DWT_Init
	ClkCyclPerUs = SystemCoreClock / 1000000;

	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {

		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CYCCNT = 0;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}

	delay = this;
}

void Delay::ms(uint32_t ms) {

	// Reload ms value
	msTicks = ms;
	// Wait until usTick reach zero
	while (msTicks)
		;
}

void Delay::us(uint32_t us) {

	int32_t tp = DWT->CYCCNT + us * ClkCyclPerUs;
	while (((int32_t)DWT->CYCCNT - tp) < 0) ;
}
