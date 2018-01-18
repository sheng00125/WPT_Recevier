#include "nokia_5110.h"
#include "ascii_8x6.h"
#include "chinese_string.h"

void delay_1us(void)                 //1us��ʱ����
  {
    unsigned char a,b;
    for(b=3;b>0;b--)
        for(a=10;a>0;a--);
  }

  void delay_1ms(void)                 //1ms��ʱ����
  {
    unsigned char a,b;
    for(b=233;b>0;b--)
        for(a=153;a>0;a--);
}
  
void delay_nms(unsigned int n)       //N ms��ʱ����
  {
   unsigned int i=0;
   for (i=0;i<n;i++)
		delay_1ms();
  }


void LCD_init(void)
  {
	  
//	RCC->APB2ENR|=1<<0;     //��������ʱ��	   
//	AFIO->MAPR&=0XF8FFFFFF; //���MAPR��[26:24]
//	AFIO->MAPR|=1<<25;       //����jtagģʽ

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		
	GPIOA -> CRL &= 0xFFFFF0FF;		//PA2
	GPIOA -> CRL |= 0x00000300;
	GPIOA -> BSRR= 1 << 2;
		
	GPIOA -> CRL &= 0xFFFF0FFF;		//PA3
	GPIOA -> CRL |= 0x00003000;
	GPIOA -> BSRR= 1 << 3;
	
	GPIOA -> CRL &= 0xFFF0FFFF;		//PA4
	GPIOA -> CRL |= 0x00030000;
	GPIOA -> BSRR= 1 << 4;
	
	GPIOA -> CRL &= 0xFF0FFFFF;		//PA5
	GPIOA -> CRL |= 0x00300000;
	GPIOA -> BSRR= 1 << 5;
	
	GPIOA -> CRL &= 0xF0FFFFFF;		//PA6
	GPIOA -> CRL |= 0x03000000;
	GPIOA -> BSRR= 1 << 6;
	
	GPIOA -> CRL &= 0x0FFFFFFF;		//PA7
	GPIOA -> CRL |= 0x30000000;
	GPIOA -> BSRR= 0 << 7;
//	
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.GPIO_Pin= GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
//	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_Out_PP;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

//  GPIO_Init(GPIOA,&GPIO_InitStruct);

	
    // ����һ����LCD��λ�ĵ͵�ƽ����
	 RST_5110_0;
    delay_1us();
	 RST_5110_1;
		// �ر�LCD
	CE_5110_0;
    delay_1us();
		// ʹ��LCD
	CE_5110_1;
    delay_1us();

    LCD_write_byte(0x21, 0);	// ʹ����չ��������LCDģʽ
   //LCD_write_byte(0xc8, 0);	// ����ƫ�õ�ѹ
	LCD_write_byte(0xbf, 0);	// ����ƫ�õ�ѹ
    LCD_write_byte(0x06, 0);	// �¶�У��
    LCD_write_byte(0x13, 0);	// 1:48
    LCD_write_byte(0x20, 0);	// ʹ�û�������
    LCD_clear();	        // ����
    LCD_write_byte(0x0c, 0);	// �趨��ʾģʽ��������ʾ
    CE_5110_0;		// �ر�LCD
  }

/*-----------------------------------------------------------------------
LCD_clear         : LCD��������
-----------------------------------------------------------------------*/
void LCD_clear(void)
  {
    unsigned int i;

    LCD_write_byte(0x0c, 0);			
    LCD_write_byte(0x80, 0);			

    for (i=0; i<504; i++)
      LCD_write_byte(0, 1);			
  }

/*-----------------------------------------------------------------------
LCD_set_XY        	 : ����LCD���꺯��
���������	X       	��0��83
					Y       	��0��5
-----------------------------------------------------------------------*/
void LCD_set_XY(unsigned char X, unsigned char Y)
  {
    LCD_write_byte(0x40 | Y, 0);				// column
    LCD_write_byte(0x80 | X, 0);          	// row
  }

/*-----------------------------------------------------------------------
LCD_write_char    : ��ʾӢ���ַ�

���������c       ����ʾ���ַ���
-----------------------------------------------------------------------*/
void LCD_write_char(unsigned char c)
  {
    unsigned char line;

    c -= 32;

    for (line=0; line<6; line++)
      LCD_write_byte(font6x8[c][line], 1);
  }

/*-----------------------------------------------------------------------
LCD_write_english_String  : Ӣ���ַ�����ʾ����

���������*s      ��Ӣ���ַ���ָ�룻
          X��Y    : ��ʾ�ַ�����λ��,x 0-83 ,y 0-5
-----------------------------------------------------------------------*/
void LCD_write_english_string(unsigned char X,unsigned char Y,char *s)
  {
    LCD_set_XY(X,Y);
    while (*s) 
      {
	 LCD_write_char(*s);
	 s++;
      }
  }
/*-----------------------------------------------------------------------
LCD_write_chinese_string: ��LCD����ʾ����

���������X��Y    ����ʾ���ֵ���ʼX��Y���ꣻ
          ch_with �����ֵ���Ŀ��
          num     ����ʾ���ֵĸ�����  
          line    �����ֵ��������е���ʼ����
          row     ��������ʾ���м��
-----------------------------------------------------------------------*/                        
void LCD_write_chinese_string(unsigned char X, unsigned char Y, 
                   unsigned char ch_with,unsigned char num,
                   unsigned char line,unsigned char row)
  {
    unsigned char i,n;
    
    LCD_set_XY(X,Y);                             //���ó�ʼλ��
    
    for (i=0;i<num;)
      {
      	for (n=0; n<ch_with*2; n++)              //дһ������
      	  { 
      	    if (n==ch_with)                      //д���ֵ��°벿��
      	      {
      	        if (i==0) LCD_set_XY(X,Y+1);
      	        else
      	           LCD_set_XY((X+(ch_with+row)*i),Y+1);
              }
      	    LCD_write_byte(write_chinese[line+i][n],1);
      	  }
      	i++;
      	LCD_set_XY((X+(ch_with+row)*i),Y);
      }
  }
  
/*-----------------------------------------------------------------------
LCD_draw_map      : λͼ���ƺ���

���������X��Y    ��λͼ���Ƶ���ʼX��Y���ꣻ
          *map    ��λͼ�������ݣ�
          Pix_x   ��λͼ���أ�����
          Pix_y   ��λͼ���أ���
-----------------------------------------------------------------------*/
void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                  unsigned char Pix_x,unsigned char Pix_y)
  {
    unsigned int i,n;
    unsigned char row;
    
    if (Pix_y%8==0) row=Pix_y/8;      //����λͼ��ռ����
      else
        row=Pix_y/8+1;
    
    for (n=0;n<row;n++)
      {
      	LCD_set_XY(X,Y);
        for(i=0; i<Pix_x; i++)
          {
            LCD_write_byte(map[i+n*Pix_x], 1);
          }
        Y++;                         //����
      }      
  }

/*-----------------------------------------------------------------------
LCD_write_byte    : ʹ��SPI�ӿ�д���ݵ�LCD

���������data    ��д������ݣ�
          command ��д����/����ѡ��
-----------------------------------------------------------------------*/
void LCD_write_byte(unsigned char dat, unsigned char command)
  {
    unsigned char i;
	    CE_5110_0;
    
    if (command == 0)
		DC_5110_0;		// ��������
    else
		DC_5110_1;		// ��������
	
		for(i=0;i<8;i++)
		{
			if(dat&0x80)
				DIN_5110_1;
			else
				DIN_5110_0;
			
			CLK_5110_0;
			dat = dat << 1;
			CLK_5110_1;
		}	
		CE_5110_1;			// �ر�LCD
  }


