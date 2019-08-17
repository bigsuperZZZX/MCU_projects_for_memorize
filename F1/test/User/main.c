#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "oled.h"
#include "mpu6050.h"
#include "AngleOutput.h"
#include "math.h"
#include "stm32f10x_tim.h"

int main(void)
{
	u16 i;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	delay_init();
	OLED_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 10000;
	TIM_TimeBaseStructure.TIM_Prescaler = 140;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
//	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2; //模式2，输入数字代表占空比高
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OC1Init(TIM2, & TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM2, ENABLE);
	
	
	while(1)
	{
	/*		TIM_SetCompare1(TIM2,255);
		OLED_ShowNum(0,0,510,4,16);
		delay_ms(3000);
			TIM_SetCompare1(TIM2,750);
		OLED_ShowNum(0,0,766,4,16);
		delay_ms(3000);
			TIM_SetCompare1(TIM2,1276);
		OLED_ShowNum(0,0,1021,4,16);
		delay_ms(3000);*/    
		
		//253   0度     768  90度     1301 180度   5.8222每度
		
		for(i=1270;i<1400;i++)
		{
			TIM_SetCompare1(TIM2,i);
		OLED_ShowNum(0,0,i	,4,16);
		}
	}
}
