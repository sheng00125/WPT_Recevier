
#include "stm32f10x_conf.h"
#include "pwm.h"
void TIM1_Mode_Config(void) 
{ 
      GPIO_InitTypeDef GPIO_InitStructure; 
      TIM_TimeBaseInitTypeDef TIM_BaseInitStructure; 
      TIM_OCInitTypeDef TIM_OCInitStructure; 

   //  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE); 

         RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
 GPIO_Init(GPIOB, &GPIO_InitStructure); 
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ; 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
 GPIO_Init(GPIOA, &GPIO_InitStructure); 

    //TIM1???????(??PWM??) 
    //??=TIM1_CLK/(ARR+1) 
    TIM_BaseInitStructure.TIM_Period = 384-1;//576-1;//   //125kHZ  (72M/125k)-1=575 
    TIM_BaseInitStructure.TIM_Prescaler = 0; 
    TIM_BaseInitStructure.TIM_ClockDivision = 0; 
    TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_BaseInitStructure.TIM_RepetitionCounter = 0; 
    TIM_TimeBaseInit(TIM1, &TIM_BaseInitStructure); 
    //??ARR??????(?????????????) 
    TIM_ARRPreloadConfig(TIM1, ENABLE); 

    //TIM1_OC1????(??1?????) 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
    TIM_OCInitStructure.TIM_OCNPolarity =TIM_OCNPolarity_High; //TIM_OCNPolarity_Low; 
 TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Set;         //???????1 
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;   //?????????0  
    TIM_OCInitStructure.TIM_Pulse = 192;///288;  // 50%???= 576/2 
    TIM_OC1Init(TIM1, &TIM_OCInitStructure); 

    //??CCR1?????????(?????????????) 
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);    
    //TIM1?? 
    TIM_Cmd(TIM1, ENABLE); 
    //TIM1_OC????PWM(????) 
    TIM_CtrlPWMOutputs(TIM1, ENABLE); 
} 


