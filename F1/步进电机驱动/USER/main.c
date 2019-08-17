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
	GPIO_InitTypeDef  GPIO_InitStructure;
	const u8 CCW[8]={0x08,0x0c,0x04,0x06,0x02,0x03,0x01,0x09};   //逆时钟旋转相序表 
  const u8 CW[8]={0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08};    //正时钟旋转相序表 
	
	delay_init();                 //延时初始化
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  
	
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	GPIO_SetBits(GPIOB,GPIO_Pin_8);  
	
	delay_ms(1800);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);

//	11 10 00 01

	GPIO_SetBits(GPIOB,GPIO_Pin_1); 
	while(1)
	{ 
		for(j=0;j<6;j++)
			for(i=0;i<10;i++)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_0); 
				GPIO_SetBits(GPIOB,GPIO_Pin_2); 
				delay_ms(5);
				GPIO_SetBits(GPIOB,GPIO_Pin_0); 
				GPIO_ResetBits(GPIOB,GPIO_Pin_2); 
				delay_ms(5);
				GPIO_ResetBits(GPIOB,GPIO_Pin_0); 
				GPIO_ResetBits(GPIOB,GPIO_Pin_2); 
				delay_ms(5);
				GPIO_ResetBits(GPIOB,GPIO_Pin_0); 
				GPIO_SetBits(GPIOB,GPIO_Pin_2); 
				delay_ms(5);
			}
		delay_ms(1000);
		
		
	for(j=0;j<6;j++)
		for(i=0;i<10;i++)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0); 
			GPIO_ResetBits(GPIOB,GPIO_Pin_2); 
			delay_ms(5);
			GPIO_SetBits(GPIOB,GPIO_Pin_0); 
			GPIO_SetBits(GPIOB,GPIO_Pin_2); 
			delay_ms(5);
			GPIO_ResetBits(GPIOB,GPIO_Pin_0); 
			GPIO_SetBits(GPIOB,GPIO_Pin_2); 
			delay_ms(5);
			GPIO_ResetBits(GPIOB,GPIO_Pin_0); 
			GPIO_ResetBits(GPIOB,GPIO_Pin_2); 
			delay_ms(5);
		}
		
		delay_ms(1000);
	}
}

