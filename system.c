#include "system.h"

void Clock_Init(unsigned char PLL)
{
	unsigned char temp=0;
	//������ʱ�Ӹ�λ
	RCC->APB1RSTR = 0x00000000;//��λ����			 
	RCC->APB2RSTR = 0x00000000; 
  	RCC->AHBENR = 0x00000014;  //˯��ģʽ�����SRAMʱ��ʹ��.�����ر�.	  
  	RCC->APB2ENR = 0x00000000; //����ʱ�ӹر�.			   
  	RCC->APB1ENR = 0x00000000;   
	RCC->CR |= 0x00000001;     //ʹ���ڲ�����ʱ��HSION	 															 
	RCC->CFGR &= 0xF8FF0000;   //��λSW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]					 
	RCC->CR &= 0xFEF6FFFF;     //��λHSEON,CSSON,PLLON
	RCC->CR &= 0xFFFBFFFF;     //��λHSEBYP	   	  
	RCC->CFGR &= 0xFF80FFFF;   //��λPLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE 
	RCC->CIR = 0x00000000;     //�ر������ж�		 
	//����ʱ��
	RCC->CR |= 0x00010000;			//ʹ��HSE �ⲿ����ʱ��
	while(!(RCC->CR >>17));	//�ȴ�HSE�������ȶ���
	PLL = PLL - 2;						//��2��ԭ��4����010,��2��5����011����3,������
	RCC->CFGR |= PLL<<18;	//����PLLMUL ��PLL��Ƶϵ��
	RCC->CFGR |= 1<<16;		//PLLSRC ѡ��PLLʱ��Դ
	FLASH->ACR|=0x32;	 		 //FLASH 2����ʱ����
	RCC->CR |= 1<<24;			//ʹ��PLL ���໷��Ƶ
	while(!(RCC->CR>>25));	//�ȴ�PLL����
	RCC->CFGR|=0x00000002;//PLL��Ϊϵͳʱ��
	while(temp != 8){					//�ȴ�PLL����
		temp = RCC ->CFGR ;
		temp &= 12;
	}
	
	
	/*
	unsigned char temp=0;   
	//MYRCC_DeInit();		  //��λ������������
 	RCC->CR|=0x00010000;  //�ⲿ����ʱ��ʹ��HSEON
	while(!(RCC->CR>>17));//�ȴ��ⲿʱ�Ӿ���
	RCC->CFGR=0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	PLL-=2;//����2����λ
	RCC->CFGR|=PLL<<18;   //����PLLֵ 2~16
	RCC->CFGR|=1<<16;	  //PLLSRC ON 
	FLASH->ACR|=0x32;	  //FLASH 2����ʱ����

	RCC->CR|=0x01000000;  //PLLON
	while(!(RCC->CR>>25));//�ȴ�PLL����
	RCC->CFGR|=0x00000002;//PLL��Ϊϵͳʱ��	 
	while(temp!=0x02)     //�ȴ�PLL��Ϊϵͳʱ�����óɹ�
	{   
		temp=RCC->CFGR>>2;
		temp&=0x03;
	} 
*/

}
