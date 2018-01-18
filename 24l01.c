#include "NRF24L01.H"

const u8 TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01}; //发送地址
const u8 RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01}; //接收地址

void NRF_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_10;  //CE,CSN,SCK,MOSI,IRQ引脚
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                       //速度
  GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;                                            //普通输出

  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;                                               //MISO输入管脚
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                       //速度
  GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU;                                             //普通输入

  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}


/****************************************************************************************
		NRF24L01检测用
***************************************************************************************/

u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};                      //检测数据
	u8 buf1[5];                                                //接收检测数据
	u8 i;   	                                                 //计数
	SPI_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);								 //写入5个字节的地址.	
	SPI_Read_Buf(NRF_READ_REG+TX_ADDR,buf1,5);                 //读出写入的地址  	
	for(i=0;i<5;i++)                                           //对读出存入的5个数据校验
	{

	  if(buf1[i]!=0XA5)
		{	
		    break;					                                     //校验失败，退出
		}
	}
	if(i!=5)
	{
		return 1;                               								 //NRF24L01不在位
	}		
	return 0;		                                	  					 //NRF24L01在位
}	
/****************************************************************************************
		NRF24L01初始化
***************************************************************************************/
void init_NRF24L01(void)
{
 	CE_L;    // chip enable
 	CSN_H;   // Spi  disable 
 	SCK_L;    
	SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, (u8*)TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址	
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (u8*)RX_ADDRESS, RX_ADR_WIDTH); // 写接收端地址
	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01);      														// 频道0自动	ACK应答允许	
	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);  														// 允许接收地址只有频道0，如果需要多频道可以参考Page21  
	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 0x6e);        													// 设置信道工作为2.4GHZ，收发必须一致
	SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); 										// 设置接收数据长度，本次设置为32字节
	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x27);   														// 设置发射速率为1MB/S，发射功率为最大值+7dB，由于有X2401L功放，实际+21dbm输出
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG2, 0x0f);   															// IRQ收发完成中断响应，16位CRC	，主接收
	CE_H; 
}
/****************************************************************************************************
		函数：uint SPI_RW(uint uchar)
		功能：NRF24L01的SPI写时序 ★★★★★★ SPI最关键的函数（即移位寄存器的思想），其他函数均调用此函数完成功能）
					请确保这个函数的正确性后，后面基本就都是对的了.
***************************************************************************************************/
u8 SPI_RW(u8 ucharb)
{
	u8 bit_ctr;

		for(bit_ctr=0;bit_ctr<8;bit_ctr++)
		{
			if(ucharb &0x80)                                      //for循环中对数据进行左移，并每次读取最高位.即依次读取数据
			{
				MOSI_H;																							//若该位为1则置1
			}
			else
			{
				MOSI_L;                                             //若该位为0则置0
			}
			SCK_H;																								//SCK置高发送数据
			delayus(2);                                          //稳定数据
			
			ucharb <<= 1;                                         //发送完后左移，准备下一位数据读取，并空出一位准备读取数据
			
			if(MISO_Read)																					//读取MISO的电平（即数据）,若为1则左移空出的位置1，否则继续（因为左移时候补了个0在最右边了.所以这里如果不是1不用再额外写else）
			{
				ucharb |= 1;
			}
			delayus(2);                                          //额，稳定一下
			SCK_L;                                                //置低，结束本次数据读取. 
		}
		return ucharb;
}
/****************************************************************************************************
函数：uchar SPI_Read(uchar reg)
功能：NRF24L01的SPI时序
****************************************************************************************************/
u8 SPI_Read(u8 reg)
{
	unsigned char reg_val;
	
	CSN_L;                  // CSN low, initialize SPI communication...SPI使能
	delayus(2);
	SPI_RW(reg);            // Select register to read from..				   选择需要读取的寄存器
	delayus(2);
	reg_val = SPI_RW(0);    // ..then read registervalue   					   逐个字节从nRF24L01读出
	delayus(2);
	CSN_H;                  // CSN high, terminate SPI communication   结束读取
	
	return(reg_val);        // return register value     						   返回寄存器值
}
/****************************************************************************************************
功能：NRF24L01读写寄存器函数
****************************************************************************************************/
u8 SPI_RW_Reg(u8 reg, u8 value)
{
	u8 status;
	
	CSN_L;                     // CSN low, init SPI transaction
	status = SPI_RW(reg);      // select register  							先发送寄存器，并读回状态
	SPI_RW(value);             // ..and write value to it..     发送值
	CSN_H;                     // CSN high again
	
	return(status);            // return nRF24L01 status uchar
}
/****************************************************************************************************
函数：uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)
功能: 用于读数据，reg：为寄存器地址，pBuf：为待读出数据地址，uchars：读出数据的个数
****************************************************************************************************/
u8 SPI_Read_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
	u8 status,uchar_ctr;
	
	CSN_L;                    		  // Set CSN low, init SPI tranaction
	status = SPI_RW(reg);       		// Select register to write to and read status uchar
	for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
	{	
			pBuf[uchar_ctr] = SPI_RW(0);                   //发送0，即发送0000 0000 去换回8位数据回来
	}
	CSN_H;                           
	
	return(status);                 // return nRF24L01 status uchar
}
/*********************************************************************************************************
函数：uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
功能: 用于写数据：为寄存器地址，pBuf：为待写入数据地址，uchars：写入数据的个数
*********************************************************************************************************/
u8 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
	u8 status,uchar_ctr;
	
	CSN_L;            //SPI使能       
	status = SPI_RW(reg);  
	for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) 
	{
		SPI_RW(*pBuf++);
	}
	CSN_H;           //关闭SPI
	return(status);     
}

/******************************************************************************************************
函数：unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
功能：数据读取后放如rx_buf接收缓冲区中
******************************************************************************************************/
u8 NRF24L01_RxPacket(u8* rx_buf)
{
  u8 revale=0;
	u8 sta;
	sta=SPI_Read(STATUS);															// 读取状态寄存其来判断数据接收状况
	if(sta&RX_OK)																			// 判断是否接收到数据
	{
	  CE_L; 																					//SPI使能
		SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);//read receive payload from RX_FIFO buffer
		revale =1;																			//读取数据完成标志
	}
	SPI_RW_Reg(NRF_WRITE_REG+STATUS,sta);   				  //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标志
	CE_H;                            								
	return revale;
}
void TX_Mode(void)
{														 
	CE_L;	    
  	SPI_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);    //?TX???? 
  	SPI_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //??TX????,??????ACK	  

  	SPI_RW_Reg(NRF_WRITE_REG+EN_AA,0x01);//0x01);    //????0?????    
  	SPI_RW_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//0x01);  //????0?????  
  	SPI_RW_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//0x1a);//??????????:500us + 86us;????????:10?
  	SPI_RW_Reg(NRF_WRITE_REG+RF_CH,40);       //??RF???40
  	SPI_RW_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //??TX????,0db??,2Mbps,???????   
  	
	SPI_RW_Reg(NRF_WRITE_REG+CONFIG2,0x0e);    //???????????;PWR_UP,EN_CRC,16BIT_CRC,????,??????
	CE_H;                                  //CE??,10us?????
}
//??SPI???? ,regaddr:??????
u8 NRF24L01_Read_Reg(u8 regaddr)
{
	u8 reg_val;	    
 	CSN_L;                //??SPI??		
  	SPI_RW(regaddr);     //??????
  	reg_val=SPI_RW(0XFF);//???????
  	CSN_H;                //??SPI??		    
  	return(reg_val);                 //?????
}	
void NRF24L01_TxPacket(u8 *txbuf)
{
 vu8 state; 
  CE_L;
  SPI_Write_Buf(NRF_WRITE_REG+RX_ADDR_P1,(u8*)RX_ADDRESS,RX_ADR_WIDTH); 
  SPI_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//????TX BUF  32???
  SPI_RW_Reg(NRF_WRITE_REG+CONFIG2,0x0e);
	CE_H;
	while(IRQ_1!=0);                              //??????
	state=SPI_Read(STATUS);                     //?????????
	SPI_RW_Reg(NRF_WRITE_REG+STATUS,state);
}		
