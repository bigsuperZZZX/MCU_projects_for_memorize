#include "sys.h"
#include "stm32f10x_tim.h"


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//ϵͳ�жϷ������û�		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/10
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************  
void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

}

//��TIM4��ϵͳ�ṩʱ��
void sys_pulse(void)
{
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
	 NVIC_InitTypeDef NVIC_InitStructure;
	
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); 
    
  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  
    
  TIM_TimeBaseStructure.TIM_Period = 100;//Ƶ�ʣ�72MHz  72000000/36000=2000   �����ж�ʱ����  TIM_Period/��72M/(TIM_Prescaler+1)��s
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

int sys_pulsecount1=0,sys_pulsecount2=0;
u8 last_state1=0,last_state2=0;
u8 source_celect=0;
u8 ch7_state=0;

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		/*******************************/
		PBout(15) = 1;
		//////////////////
		if(PAin(6) == 1)
		{
			if(last_state1 == 0)
			{
				sys_pulsecount1 = 0;
			}
			sys_pulsecount1++;
			last_state1 = 1;
		}
		else
		{
			if(last_state1 == 1)
			{
				if(sys_pulsecount1 >18 )
				{
					source_celect =1;
				}
				else
				{
					source_celect = 0;
				}
			}
			last_state1 = 0;
			sys_pulsecount1=0;
		}
		///////////////////////
		if(PAin(7) == 1)
		{
			if(last_state2 == 0)
			{
				sys_pulsecount2 = 0;
			}
			sys_pulsecount2++;
			last_state2 = 1;
		}
		else
		{
			if(last_state2 == 1)
			{
				if(sys_pulsecount2 > 13)
				{
					ch7_state =1;
				}
				else
				{
					ch7_state = 0;
				}
			}
			last_state2 = 0;
			sys_pulsecount2=0;
		}
		PBout(15) = 0;
		/*********************************/
	}
}


