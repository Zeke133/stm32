#include <delay.h>



// SysTick_Handler function will be called every 1 ms
void SysTick_Handler() {

	if (Delay::msCount) Delay::msCount--;
}

// Class implementation
uint32_t Delay::msCount = 0;
uint32_t Delay::sysTicksPerUs = SystemCoreClock / 1000000;

Delay::Delay() {

	/* Configuring SysTick timer for implementation of 1ms delays by timer interrupt */

	// Update SystemCoreClock value
	SystemCoreClockUpdate();

	// Configure the SysTick timer to overflow every 1 ms
	SysTick_Config(SystemCoreClock / 1000);

	/* Configuring DWT timer for implementation of 1us delays by cycle */

	// DWT_Init
	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {

		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CYCCNT = 0;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}

	// For measuring of execution time
	profilingStart = 0;
}

void Delay::ms(uint32_t ms) const {

	// Reload ms value
	msCount = ms;
	// Wait until usTick reach zero
	while (msCount) {}
}

void Delay::us(uint32_t us) const {

	uint32_t ticksToOverflow = 0xFFFFFFFF - getSysTickValue();
	uint32_t delayTicks = us * sysTicksPerUs;			// Value of delay in sysTicks

	while ( ticksToOverflow + getSysTickValue() < delayTicks ) {};
}

uint32_t Delay::getSysTickValue(void) const {

	return DWT->CYCCNT;
}

void Delay::startProfiling(void) {

	profilingStart = getSysTickValue();
}

uint32_t Delay::getExecutionTime(void) const {

	uint32_t ticksNow = getSysTickValue();

	// overflow happened
	// no overflow or execution time is more than 59 seconds what is unlikely

	return (profilingStart > ticksNow) ? (0xffffffff - profilingStart + ticksNow) : (ticksNow - profilingStart);
}

uint32_t Delay::sysTicks2us(uint32_t ticks) {

	return ticks / sysTicksPerUs;
}


