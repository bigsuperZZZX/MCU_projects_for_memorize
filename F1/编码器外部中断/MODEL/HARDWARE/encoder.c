#include "encoder.h"
#include "stm32f10x.h"
#include "stm32f10x_exti.h"

long pulse_r=0,pulse_l=0;
char dir_r=0,dir_l=0;   //0是前，1是后  右电机绿线中短线，下降沿进入，白线为低则为前，为高则为后 // 左 白 下 绿 低 前 高 后

void encoder_ExtiInit(void)  //interrupt 2
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	EXTI_InitTypeDef EXTI_InitStructure;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_IRQHandler(void)  //外部中断0入口函数
{
	if(EXTI_GetITStatus(EXTI_Line0)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line0);
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)
		{
			dir_r=0;  //前
			pulse_r++;
		}
		else
		{
			dir_r=1;    //后
			pulse_r--;
		}
	}
}

signed long read_encoder_pulse()
{
	return pulse_r;
}

char read_encoder_dirction()
{
	return dir_r;
}



