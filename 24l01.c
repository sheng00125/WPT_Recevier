#include "NRF24L01.H"

const u8 TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const u8 RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01}; //���յ�ַ

void NRF_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_10;  //CE,CSN,SCK,MOSI,IRQ����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                       //�ٶ�
  GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;                                            //��ͨ���

  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;                                               //MISO����ܽ�
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                       //�ٶ�
  GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU;                                             //��ͨ����

  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}


/****************************************************************************************
		NRF24L01�����
***************************************************************************************/

u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};                      //�������
	u8 buf1[5];                                                //���ռ������
	u8 i;   	                                                 //����
	SPI_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);								 //д��5���ֽڵĵ�ַ.	
	SPI_Read_Buf(NRF_READ_REG+TX_ADDR,buf1,5);                 //����д��ĵ�ַ  	
	for(i=0;i<5;i++)                                           //�Զ��������5������У��
	{

	  if(buf1[i]!=0XA5)
		{	
		    break;					                                     //У��ʧ�ܣ��˳�
		}
	}
	if(i!=5)
	{
		return 1;                               								 //NRF24L01����λ
	}		
	return 0;		                                	  					 //NRF24L01��λ
}	
/****************************************************************************************
		NRF24L01��ʼ��
***************************************************************************************/
void init_NRF24L01(void)
{
 	CE_L;    // chip enable
 	CSN_H;   // Spi  disable 
 	SCK_L;    
	SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, (u8*)TX_ADDRESS, TX_ADR_WIDTH);    // д���ص�ַ	
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (u8*)RX_ADDRESS, RX_ADR_WIDTH); // д���ն˵�ַ
	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01);      														// Ƶ��0�Զ�	ACKӦ������	
	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);  														// ������յ�ַֻ��Ƶ��0�������Ҫ��Ƶ�����Բο�Page21  
	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 0x6e);        													// �����ŵ�����Ϊ2.4GHZ���շ�����һ��
	SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); 										// ���ý������ݳ��ȣ���������Ϊ32�ֽ�
	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x27);   														// ���÷�������Ϊ1MB/S�����书��Ϊ���ֵ+7dB��������X2401L���ţ�ʵ��+21dbm���
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG2, 0x0f);   															// IRQ�շ�����ж���Ӧ��16λCRC	��������
	CE_H; 
}
/****************************************************************************************************
		������uint SPI_RW(uint uchar)
		���ܣ�NRF24L01��SPIдʱ�� ������� SPI��ؼ��ĺ���������λ�Ĵ�����˼�룩���������������ô˺�����ɹ��ܣ�
					��ȷ�������������ȷ�Ժ󣬺�������Ͷ��ǶԵ���.
***************************************************************************************************/
u8 SPI_RW(u8 ucharb)
{
	u8 bit_ctr;

		for(bit_ctr=0;bit_ctr<8;bit_ctr++)
		{
			if(ucharb &0x80)                                      //forѭ���ж����ݽ������ƣ���ÿ�ζ�ȡ���λ.�����ζ�ȡ����
			{
				MOSI_H;																							//����λΪ1����1
			}
			else
			{
				MOSI_L;                                             //����λΪ0����0
			}
			SCK_H;																								//SCK�ø߷�������
			delayus(2);                                          //�ȶ�����
			
			ucharb <<= 1;                                         //����������ƣ�׼����һλ���ݶ�ȡ�����ճ�һλ׼����ȡ����
			
			if(MISO_Read)																					//��ȡMISO�ĵ�ƽ�������ݣ�,��Ϊ1�����ƿճ���λ��1�������������Ϊ����ʱ���˸�0�����ұ���.���������������1�����ٶ���дelse��
			{
				ucharb |= 1;
			}
			delayus(2);                                          //��ȶ�һ��
			SCK_L;                                                //�õͣ������������ݶ�ȡ. 
		}
		return ucharb;
}
/****************************************************************************************************
������uchar SPI_Read(uchar reg)
���ܣ�NRF24L01��SPIʱ��
****************************************************************************************************/
u8 SPI_Read(u8 reg)
{
	unsigned char reg_val;
	
	CSN_L;                  // CSN low, initialize SPI communication...SPIʹ��
	delayus(2);
	SPI_RW(reg);            // Select register to read from..				   ѡ����Ҫ��ȡ�ļĴ���
	delayus(2);
	reg_val = SPI_RW(0);    // ..then read registervalue   					   ����ֽڴ�nRF24L01����
	delayus(2);
	CSN_H;                  // CSN high, terminate SPI communication   ������ȡ
	
	return(reg_val);        // return register value     						   ���ؼĴ���ֵ
}
/****************************************************************************************************
���ܣ�NRF24L01��д�Ĵ�������
****************************************************************************************************/
u8 SPI_RW_Reg(u8 reg, u8 value)
{
	u8 status;
	
	CSN_L;                     // CSN low, init SPI transaction
	status = SPI_RW(reg);      // select register  							�ȷ��ͼĴ�����������״̬
	SPI_RW(value);             // ..and write value to it..     ����ֵ
	CSN_H;                     // CSN high again
	
	return(status);            // return nRF24L01 status uchar
}
/****************************************************************************************************
������uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)
����: ���ڶ����ݣ�reg��Ϊ�Ĵ�����ַ��pBuf��Ϊ���������ݵ�ַ��uchars���������ݵĸ���
****************************************************************************************************/
u8 SPI_Read_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
	u8 status,uchar_ctr;
	
	CSN_L;                    		  // Set CSN low, init SPI tranaction
	status = SPI_RW(reg);       		// Select register to write to and read status uchar
	for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
	{	
			pBuf[uchar_ctr] = SPI_RW(0);                   //����0��������0000 0000 ȥ����8λ���ݻ���
	}
	CSN_H;                           
	
	return(status);                 // return nRF24L01 status uchar
}
/*********************************************************************************************************
������uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
����: ����д���ݣ�Ϊ�Ĵ�����ַ��pBuf��Ϊ��д�����ݵ�ַ��uchars��д�����ݵĸ���
*********************************************************************************************************/
u8 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
	u8 status,uchar_ctr;
	
	CSN_L;            //SPIʹ��       
	status = SPI_RW(reg);  
	for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) 
	{
		SPI_RW(*pBuf++);
	}
	CSN_H;           //�ر�SPI
	return(status);     
}

/******************************************************************************************************
������unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
���ܣ����ݶ�ȡ�����rx_buf���ջ�������
******************************************************************************************************/
u8 NRF24L01_RxPacket(u8* rx_buf)
{
  u8 revale=0;
	u8 sta;
	sta=SPI_Read(STATUS);															// ��ȡ״̬�Ĵ������ж����ݽ���״��
	if(sta&RX_OK)																			// �ж��Ƿ���յ�����
	{
	  CE_L; 																					//SPIʹ��
		SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);//read receive payload from RX_FIFO buffer
		revale =1;																			//��ȡ������ɱ�־
	}
	SPI_RW_Reg(NRF_WRITE_REG+STATUS,sta);   				  //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�־
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
