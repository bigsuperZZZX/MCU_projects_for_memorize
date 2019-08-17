#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "DataScope_DP.h"
#include "usart.h"	

#define SCLH()  GPIO_SetBits(GPIOA,GPIO_Pin_12)
#define SCLL()  GPIO_ResetBits(GPIOA,GPIO_Pin_12)
#define SDOH()  GPIO_SetBits(GPIOA,GPIO_Pin_11)
#define SDOL()  GPIO_ResetBits(GPIOA,GPIO_Pin_11)
#define SDO()   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)
#define SDO_IN()  {GPIOA->CRH&=0XFFFF0FFF;GPIOA->CRH|=8<<12;}
#define SDO_OUT() {GPIOA->CRH&=0XFFFF0FFF;GPIOA->CRH|=3<<12;}

void GPIO_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11;	        
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOD3,6
}

u16 get_key(void)
{
	u16 KeyBuff=0;
	char i;
	SDO_OUT();
	SCLL();
	SDOL();
	delay_us(10);
	SDOH();
	delay_us(93);
	SDOL();
	SDO_IN();
	delay_us(15);
	for(i=0;i<16;i++)
	{
		SCLH();
		delay_us(6);
		SCLL();
		KeyBuff <<= 1;
		KeyBuff |= SDO();
		delay_us(4);
	}
	return KeyBuff;
}

int main()
{
	u16 Key=0;
	char i;
	GPIO_init();
	delay_init();
	uart_init(128000);
	while(1)
	{
		Key = get_key();
		for(i=0;i<16;i++)
		{
			if((Key&(0x8000>>i)) != 0)
				printf("%d  \r",i+1);
		}
		printf("\r\n");
		delay_ms(10);
	}
}

