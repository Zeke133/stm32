#include <delay.h>

Delay* delay;

// SysTick_Handler function will be called every 1 ms
void SysTick_Handler() {

	if (Delay::msTicks > 0)
		Delay::msTicks--;
}

// Class implementation
uint32_t Delay::msTicks = 0;

Delay::Delay()
	: ClkCyclPerUs(SystemCoreClock / 1000000)
	{

	// Update SystemCoreClock value
	SystemCoreClockUpdate();

	// Configure the SysTick timer to overflow every 1 ms
	SysTick_Config(SystemCoreClock / 1000);

	// DWT_Init
	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {

		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CYCCNT = 0;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}

	delay = this;
}

void Delay::ms(uint32_t ms) const {

	// Reload ms value
	msTicks = ms;
	// Wait until usTick reach zero
	while (msTicks)
		;
}

void Delay::us(uint32_t us) const {

	int32_t tp = DWT->CYCCNT + us * ClkCyclPerUs;
	while (((int32_t)DWT->CYCCNT - tp) < 0) ;
}
