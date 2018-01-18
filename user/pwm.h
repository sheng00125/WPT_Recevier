#ifndef _pwm_h_
#define _pwm_h_
#include "system.h"

void delay_init(u8 SYSCLK);
void delayus(u32 us);
void delayms(u16 ms);
void TIM1_Mode_Config(void) ;
#endif
