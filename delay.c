#include "delay.h"

static u32 fac_us;
static u32 fac_ms;
//========================
//滴答定时器设置，定时器初始化
//========================
void delay_init(u8 SYSCLK)  		//系统时钟是72MHz,SYSCLK=72  
{  
    SysTick->CTRL &= 0xFFFFFFFB ; 	//bit2清0,也就是配置选择外部时钟  
    fac_us=SYSCLK/8; 						//硬件8分频,fac_us得出的值是要给下面的时钟函数用的  
    fac_ms =fac_us*1000;  
}  
//======================================
//延时？us 。9个时钟1us。24位定时器最大延时1864135us
//======================================
void delayus(u32 us)  
{  
    u32 temp;  
    SysTick->LOAD = us*20;  		//延时1us的话就是  1*9=9,装到load寄存器中  
    SysTick->VAL=0x00;						//计数器清0
    SysTick->CTRL = 0x01;					//systick定时器使能
    do
	{  
       temp = SysTick->CTRL;  				//时间到了之后,CTRL第16位将被硬件置1,但被查询后自动清0  
    }
	while( !(temp & (1<<16) ) ); 			//第16位为1时跳出循环
   // while(temp & 0x01 && !(temp &(1<<16))); //查询   
    SysTick->CTRL = 0x00;  					//关闭计数器  
    SysTick->VAL = 0x00;   					//清空val  
}  
//=====================================
//延时？ms 最大延时1864.135ms
//=====================================
void delayms(u16 ms)  
{  
    u32 temp;  
    SysTick->LOAD = ms*fac_ms;  		//延时1ms的话就是  1*9000=9000,装到load寄存器中  
    SysTick->VAL=0x00;						
    SysTick->CTRL = 0x01;					
    do
	{  
       temp = SysTick->CTRL;  				
    }  
    while(!(temp &(1<<16))); 				//第16位为1时跳出循环 
    SysTick->CTRL = 0x00;  					//关闭计数器  
    SysTick->VAL = 0x00;   					//清空val  
}

