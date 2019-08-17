#include "encoder.h"
#include "stm32f10x.h"
#include "stm32f10x_exti.h"

long pulse_r=0,pulse_l=0;
char dir_r=0,dir_l=0;   //0��ǰ��1�Ǻ�  �ҵ�������ж��ߣ��½��ؽ��룬����Ϊ����Ϊǰ��Ϊ����Ϊ�� // �� �� �� �� �� ǰ �� ��

void encoder_ExtiInit(void)  //interrupt 2
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	EXTI_InitTypeDef EXTI_InitStructure;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_IRQHandler(void)  //�ⲿ�ж�0��ں���
{
	if(EXTI_GetITStatus(EXTI_Line0)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line0);
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)==0)
		{
			dir_r=0;  //ǰ
			pulse_r++;
		}
		else
		{
			dir_r=1;    //��
			pulse_r--;
		}
	}
}

void EXTI2_IRQHandler(void)  //�ⲿ�ж�2��ں���
{
	if(EXTI_GetITStatus(EXTI_Line2)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)==0)
		{
			dir_l=0;              //0��ǰ��1�Ǻ�
			pulse_l++;
		}
		else
		{
			dir_l=1;
			pulse_l--;
		}
	}
}



