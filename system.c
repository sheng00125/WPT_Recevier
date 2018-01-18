#include "system.h"

void Clock_Init(unsigned char PLL)
{
	unsigned char temp=0;
	//把所有时钟复位
	RCC->APB1RSTR = 0x00000000;//复位结束			 
	RCC->APB2RSTR = 0x00000000; 
  	RCC->AHBENR = 0x00000014;  //睡眠模式闪存和SRAM时钟使能.其他关闭.	  
  	RCC->APB2ENR = 0x00000000; //外设时钟关闭.			   
  	RCC->APB1ENR = 0x00000000;   
	RCC->CR |= 0x00000001;     //使能内部高速时钟HSION	 															 
	RCC->CFGR &= 0xF8FF0000;   //复位SW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]					 
	RCC->CR &= 0xFEF6FFFF;     //复位HSEON,CSSON,PLLON
	RCC->CR &= 0xFFFBFFFF;     //复位HSEBYP	   	  
	RCC->CFGR &= 0xFF80FFFF;   //复位PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE 
	RCC->CIR = 0x00000000;     //关闭所有中断		 
	//配置时钟
	RCC->CR |= 0x00010000;			//使能HSE 外部高速时钟
	while(!(RCC->CR >>17));	//等待HSE就绪（稳定）
	PLL = PLL - 2;						//减2的原因：4倍：010,即2；5倍：011，即3,；……
	RCC->CFGR |= PLL<<18;	//设置PLLMUL 即PLL倍频系数
	RCC->CFGR |= 1<<16;		//PLLSRC 选择PLL时钟源
	FLASH->ACR|=0x32;	 		 //FLASH 2个延时周期
	RCC->CR |= 1<<24;			//使能PLL 锁相环倍频
	while(!(RCC->CR>>25));	//等待PLL锁定
	RCC->CFGR|=0x00000002;//PLL作为系统时钟
	while(temp != 8){					//等待PLL锁定
		temp = RCC ->CFGR ;
		temp &= 12;
	}
	
	
	/*
	unsigned char temp=0;   
	//MYRCC_DeInit();		  //复位并配置向量表
 	RCC->CR|=0x00010000;  //外部高速时钟使能HSEON
	while(!(RCC->CR>>17));//等待外部时钟就绪
	RCC->CFGR=0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	PLL-=2;//抵消2个单位
	RCC->CFGR|=PLL<<18;   //设置PLL值 2~16
	RCC->CFGR|=1<<16;	  //PLLSRC ON 
	FLASH->ACR|=0x32;	  //FLASH 2个延时周期

	RCC->CR|=0x01000000;  //PLLON
	while(!(RCC->CR>>25));//等待PLL锁定
	RCC->CFGR|=0x00000002;//PLL作为系统时钟	 
	while(temp!=0x02)     //等待PLL作为系统时钟设置成功
	{   
		temp=RCC->CFGR>>2;
		temp&=0x03;
	} 
*/

}
