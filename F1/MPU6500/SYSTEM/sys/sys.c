#include "sys.h"

void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

}



//��TIM4��ϵͳ�ṩʱ��
void sys_it(void)
{
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
	 NVIC_InitTypeDef NVIC_InitStructure;
	
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); 
    
  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  
    
  TIM_TimeBaseStructure.TIM_Period = 1000;//Ƶ�ʣ�72MHz  72000000/36000=2000   �����ж�ʱ����  TIM_Period/��72M/(TIM_Prescaler+1)��s
  TIM_TimeBaseStructure.TIM_Prescaler = 71;//36000-1=35999  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);  
    
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE );  
  TIM_Cmd(TIM4,ENABLE); 
      
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    
      
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;    
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;    
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;    
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
  NVIC_Init(&NVIC_InitStructure);    	
}	





