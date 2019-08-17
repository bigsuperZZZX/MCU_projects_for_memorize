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

int sys_pulsecount;
extern int rcv_time_count;
extern int USART1_RcvCnt;
extern u8 USART1_RcvCpltFlag;
extern u8 USART1_RcvNewByteFlag;
extern u8 USART1_DataBuf[200];
extern int SIM900A_WaitCnt;
extern int Read_BufLen;
extern u8  circle_flag;

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		sys_pulsecount++;
		
		rcv_time_count++;
		if(rcv_time_count == 50 && USART1_RcvNewByteFlag==1) 
		{
			rcv_time_count = 0;
			Read_BufLen = USART1_RcvCnt;
			USART1_DataBuf[USART1_RcvCnt] = 0;
			USART1_RcvCnt = 0;
			USART1_RcvCpltFlag = 1;
			USART1_RcvNewByteFlag = 0;
		}
		if(sys_pulsecount % 5000 == 0) circle_flag = 1;
		if(sys_pulsecount == 50000) sys_pulsecount=0;
	}
}


