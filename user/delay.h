#ifndef _delay_h_
#define _delay_h_
#include "system.h"

void delay_init(u8 SYSCLK);
void delayus(u32 us);
void delayms(u16 ms);

#endif
