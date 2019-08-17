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
	u16 count=0;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	delay_init();                 //延时初始化
	OLED_Init();
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	   
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	
	while(1)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_1);
		delay_ms(10);
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) == 0);
		count=0;
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) != 0)
		{
			count++;
			delay_us(10);
		}
		OLED_ShowNum(0,0,count,5,16);
		delay_ms(500);
	}
}

