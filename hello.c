
#include "stm32f10x_conf.h"


#include "system.h"
#include "delay.h"
#include "nokia_5110.h"
#include "NRF24L01.H"
#include "pwm.h"
#include "Adc_Timer.h"

//#define TX_Machine 1
u8 num = 0;
u16 temp = 2;
u16 adc1ch1,adc2ch2;
u8 Rx_Buf[]={0};
u8 Tx_Buf1[10];
//=============================
//主函数
//=============================
int main(void)
{	

	Clock_Init(9);
	delay_init(72);
	delayms(500);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);   
	//RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE); 

	NRF_GPIO_Config(); 
	Adc_Init();
	Adc2_Init();
	//======== PC13置零 LED亮 ===========
	RCC ->APB2ENR |= 1 << 4;			//使能PC时钟
	GPIOC -> CRH &= 0xFF0FFFFF;		//清除PC13设置
	GPIOC -> CRH |= 0x00300000;		//通用推挽输出，50MHz
	GPIOC -> BRR = 1<< 13;
		
	NRF_GPIO_Config();   
	while(temp == 1)
	{
			temp = NRF24L01_Check();
	}
	LCD_init(); //初始化液晶    
	LCD_clear();

	TX_Mode();
	Timerx_Init(0,0);
	

	while(1)
	{
		
	//	RX_Mode();	

	}
}


void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 

		adc1ch1=Get_Adc(0);
			
		Tx_Buf1[0]=adc1ch1;
		Tx_Buf1[1]=adc1ch1>>8;
		adc2ch2=Get_Adc2(0);
		Tx_Buf1[2]=adc2ch2;
		Tx_Buf1[3]=adc2ch2>>8;
		NRF24L01_TxPacket(Tx_Buf1);

		}
}







