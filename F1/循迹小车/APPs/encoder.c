#include "encoder.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"

u32 counter=0;
extern u16 PWM_l,PWM_r;
extern u8 corner_in;   //�Ƿ����ڹ���
extern u16 dist_count_last;

void encoder_exit_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		EXTI_ClearITPendingBit(EXTI_Line5);
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE); //ʹ��PB�˿�ʱ��
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	            //�˿�����
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;         //��������
		GPIO_Init(GPIOG, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOB 
	
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource5);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
}


void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line5);
		counter++;
	}
}
