#ifndef _SYS_CLOCK_H
#define _SYS_CLOCK_H

// using
extern "C" {
#include <stm32f10x_rcc.h>
#include <stm32f10x_flash.h>
}

/*
System clock initialization API
*/
void SetSysClockTo72(void);

#endif
