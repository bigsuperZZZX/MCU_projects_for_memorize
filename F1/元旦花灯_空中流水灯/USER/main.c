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


int main(void)
{ 
	int i,j;
	int short_interval = 80 , long_interval = 400;
	u16 word_weite = 0x0001;
	GPIO_InitTypeDef  GPIO_InitStructure;
	delay_init();                 //延时初始化
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | 
																GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | 
																GPIO_Pin_10 | GPIO_Pin_11;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  
	while(1)
	{ 
		PAout(0) = 1;
		PAout(11) = 1;
		delay_ms(120);
		PAout(11) = 0;
		delay_ms(long_interval);
		for(i=1;i<=8;i++)
		{
				GPIO_Write(GPIOA,word_weite<<i);
				delay_ms(short_interval);
		}
		GPIO_Write(GPIOA,0);
		PAout(9) = 1;
		PAout(10) = 1;
		delay_ms(150);
		PAout(10) = 0;
		delay_ms(long_interval);
		for(i=9;i>=1;i--)
		{
				GPIO_Write(GPIOA,word_weite<<i);
				delay_ms(short_interval);
		}
		GPIO_Write(GPIOA,0);
	}
}

