#ifndef _SYS_CLOCK_H
#define _SYS_CLOCK_H

// using
extern "C" {
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_system.h>
#include <stm32f1xx_ll_utils.h>
}

/*
System clock initialization API
*/
void SetSysClockTo72(void);

#endif
