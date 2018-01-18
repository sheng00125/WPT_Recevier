#include "delay.h"

static u32 fac_us;
static u32 fac_ms;
//========================
//�δ�ʱ�����ã���ʱ����ʼ��
//========================
void delay_init(u8 SYSCLK)  		//ϵͳʱ����72MHz,SYSCLK=72  
{  
    SysTick->CTRL &= 0xFFFFFFFB ; 	//bit2��0,Ҳ��������ѡ���ⲿʱ��  
    fac_us=SYSCLK/8; 						//Ӳ��8��Ƶ,fac_us�ó���ֵ��Ҫ�������ʱ�Ӻ����õ�  
    fac_ms =fac_us*1000;  
}  
//======================================
//��ʱ��us ��9��ʱ��1us��24λ��ʱ�������ʱ1864135us
//======================================
void delayus(u32 us)  
{  
    u32 temp;  
    SysTick->LOAD = us*20;  		//��ʱ1us�Ļ�����  1*9=9,װ��load�Ĵ�����  
    SysTick->VAL=0x00;						//��������0
    SysTick->CTRL = 0x01;					//systick��ʱ��ʹ��
    do
	{  
       temp = SysTick->CTRL;  				//ʱ�䵽��֮��,CTRL��16λ����Ӳ����1,������ѯ���Զ���0  
    }
	while( !(temp & (1<<16) ) ); 			//��16λΪ1ʱ����ѭ��
   // while(temp & 0x01 && !(temp &(1<<16))); //��ѯ   
    SysTick->CTRL = 0x00;  					//�رռ�����  
    SysTick->VAL = 0x00;   					//���val  
}  
//=====================================
//��ʱ��ms �����ʱ1864.135ms
//=====================================
void delayms(u16 ms)  
{  
    u32 temp;  
    SysTick->LOAD = ms*fac_ms;  		//��ʱ1ms�Ļ�����  1*9000=9000,װ��load�Ĵ�����  
    SysTick->VAL=0x00;						
    SysTick->CTRL = 0x01;					
    do
	{  
       temp = SysTick->CTRL;  				
    }  
    while(!(temp &(1<<16))); 				//��16λΪ1ʱ����ѭ�� 
    SysTick->CTRL = 0x00;  					//�رռ�����  
    SysTick->VAL = 0x00;   					//���val  
}

