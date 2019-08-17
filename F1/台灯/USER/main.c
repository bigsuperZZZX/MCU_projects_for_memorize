#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "DataScope_DP.h"
#include "usart.h"	

unsigned char i,j;          //计数变量


void PWM_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);           //
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 400;
	TIM_TimeBaseStructure.TIM_Prescaler = 4;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2; //模式2，输入数字代表占空比高
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OC2Init(TIM2, & TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM2, & TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM2, ENABLE);
	
	TIM_SetCompare2(TIM2,0);   
	TIM_SetCompare4(TIM2,0);
}

void GPIO_INIT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//速度50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
 	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}

int main(void)
{ 
	signed int i=1;
	signed char Flag_on=0 ,dir =0;
	delay_init();                 //延时初始化

	PWM_INIT();
	GPIO_INIT();
	
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	
	for(i=1;i<40;i++)
	{
		TIM_SetCompare2(TIM2,250);   // PA1
		delay_ms(i*25);
		TIM_SetCompare2(TIM2,0);   // PA1
		delay_ms(1000-i*25);
	}
	
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1))
		{
			delay_ms(100);
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1))
			{
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13))
				{
					Flag_on =0;
					GPIO_ResetBits(GPIOC,GPIO_Pin_13);
				}
				else
				{
					Flag_on =1;
					GPIO_SetBits(GPIOC,GPIO_Pin_13);
				}
				TIM_SetCompare2(TIM2,250);   // PA1
				TIM_SetCompare4(TIM2,0);
				delay_ms(1800);
				delay_ms(1800);
				delay_ms(1800);
				delay_ms(1800);
				delay_ms(1800);
				TIM_SetCompare2(TIM2,0);   // PA1
			}
		}
		delay_ms(100);
		if(!Flag_on)
		{
			TIM_SetCompare4(TIM2,i);
			if(dir)
				i*=2;
			else
				i/=2;
			if(i > 300) dir=0;
			if(i <= 1) dir=1,i=1;
			
			if(i > 300) i=300;
			if(i <= 00) i=1;
		}
		else
		{
			i=1;
			TIM_SetCompare4(TIM2,1);
		}
	}
}

