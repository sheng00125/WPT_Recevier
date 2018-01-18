#ifndef _Adc_Timer_h_
#define _Adc_Timer_h_
#include "system.h"


void  Adc_Init(void);
u16 Get_Adc(u8 ch);
void Timerx_Init(u16 arr,u16 psc);
void  Adc2_Init(void);
u16 Get_Adc2(u8 ch);

#endif
