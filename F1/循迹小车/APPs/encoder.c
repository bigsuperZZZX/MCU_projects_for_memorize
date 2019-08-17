#include "encoder.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"

u32 counter=0;
extern u16 PWM_l,PWM_r;
extern u8 corner_in;   //是否正在过弯
extern u16 dist_count_last;

void encoder_exit_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		EXTI_ClearITPendingBit(EXTI_Line5);
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE); //使能PB端口时钟
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	            //端口配置
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;         //上拉输入
		GPIO_Init(GPIOG, &GPIO_InitStructure);					      //根据设定参数初始化GPIOB 
	
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource5);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
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
