#include "TIM.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"

/*void TIM3_Init(void)    
{  

	NVIC_InitTypeDef NVIC_InitStructure;  
	
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
    
  TIM_TimeBaseStructure.TIM_Period = 7200;//频率：72MHz  72000000/36000=2000   进入中断时间间隔  TIM_Period/（72M/(TIM_Prescaler+1)）s
  TIM_TimeBaseStructure.TIM_Prescaler = 499;//36000-1=35999  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
    
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );  
  TIM_Cmd(TIM3,ENABLE);    
      
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);    
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;    
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;    
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;    
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
  NVIC_Init(&NVIC_InitStructure);    
}  */


void TIM3_Init(void) 
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);	
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	
	TIM_TimeBaseStructure.TIM_Period = 800;
	TIM_TimeBaseStructure.TIM_Prescaler = 4499;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/*中断优先级配置*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  /*中断名*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );
	
	TIM_Cmd(TIM3, ENABLE);
}

