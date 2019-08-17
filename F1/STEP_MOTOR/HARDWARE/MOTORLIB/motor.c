#include "motor.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "delay.h"

/*******************
PB10 PUL
PB11 DIR
*********************/
 
StepMotor_TypeDef  StepMotor;   //0��ת�̵����1-5˿�˵����˳ʱ������

void TIM1_UP_IRQHandler(void)   
{
	static uint32_t it_count = 0;
//	GPIO_SetBits(GPIOC,GPIO_Pin_13);
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//���ָ����TIM�жϷ������:TIM �ж�Դ   
  {  
		TIM1->ARR = 210000/StepMotor.SetSpeed ;
//		TIM1->ARR = 21;
		if(it_count%2)
		{
			if(StepMotor.SetState>StepMotor.CurState && (it_count%2))
			{
				StepMotor.CurState++;	
				GPIO_ResetBits(GPIOB,GPIO_Pin_13);
				GPIO_SetBits(GPIOB,GPIO_Pin_14);
			}
			else if(StepMotor.SetState<StepMotor.CurState && (it_count%2))
			{
				StepMotor.CurState--;
				GPIO_SetBits(GPIOB,GPIO_Pin_13);
				GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			}
			else 
				return;
			GPIO_SetBits(GPIOB,GPIO_Pin_12);
		}
		else
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		}
		it_count++;
	}
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//���TIMx���жϴ�����λ:TIM �ж�Դ
//	GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	
} 

unsigned char read_key(void)
{
	static u8 flag_key=1;//�������ɿ���־
	u8 key_state=GPIO_ReadInputDataBit(KEY_PORT,KEY_PIN);
	if(flag_key && key_state==0)
	{
		flag_key=0;
		return 1;	// ��������
	}
	else if(1==key_state)
		flag_key=1;
	return 0;//�ް�������
}


void motor_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;  
//	TIM_OCInitTypeDef TIM_OCInitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_TIM1, ENABLE);
	/*********�˿�����**************************/
	/*GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	/////////// 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);*/
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;	/////////// 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	/* Time base configuration */
	StepMotor.SetSpeed = 1000;
  TIM_TimeBaseStructure.TIM_Period = 10000;        
  TIM_TimeBaseStructure.TIM_Prescaler = 9;            //3��Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;      //
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);          //��ʼ��TIM3

  
 /* TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;    //����ȽϷ�תģʽ
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //ʹ��ͨ��1
  TIM_OCInitStructure.TIM_Pulse = 50000;                       //��װ������ȽϼĴ����е�����ֵ     
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         //���Ϊ���߼� 
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);        //д������

  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);          //ʹ�ܻ���ʧ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_CtrlPWMOutputs(TIM1, ENABLE);*/
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    
      
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;    
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;    
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
  NVIC_Init(&NVIC_InitStructure);  

	//TIM_Cmd(TIM1, ENABLE);  //ʹ��TIMx���� 
}

/*void TIM1_CC_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)               //���ָ����TIM�жϷ������
  {
		uint16_t temp = GPIOA->ODR;
		uint16_t capture = TIM_GetCapture1(TIM1);
		static uint16_t CCR1_Val ;
		static uint8_t it_count = 0;
		it_count++;
		if(StepMotor.CurState>StepMotor.SetState && (it_count%2)) 
		{
			StepMotor.CurState--;
			GPIO_SetBits(GPIOA,GPIO_Pin_9);
			GPIO_ResetBits(GPIOA,GPIO_Pin_10);
		}
		else if(StepMotor.CurState<StepMotor.SetState && (it_count%2))
		{
			StepMotor.CurState++;
			GPIO_ResetBits(GPIOA,GPIO_Pin_9);
			GPIO_SetBits(GPIOA,GPIO_Pin_10);
		}
		else
			return ;
		
		CCR1_Val =120000/StepMotor.SetSpeed ;     //���ٶȼ���Ƚ�ֵ
		
		if(temp&0x0100) GPIOA->ODR &= 0xfeff;     //��ת
		else GPIOA->ODR |= 0x0100;
    TIM_SetCompare1(TIM1, capture + CCR1_Val );        //������ֵ���Ϸ�ת������ֵд������ȽϼĴ�����
		////////////////////////////////////////////////
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC1 );               //���TIMx���жϴ�����λ
  }
}*/



