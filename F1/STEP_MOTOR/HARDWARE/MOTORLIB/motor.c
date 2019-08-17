#include "motor.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "delay.h"

/*******************
PB10 PUL
PB11 DIR
*********************/
 
StepMotor_TypeDef  StepMotor;   //0，转盘电机，1-5丝杆电机，顺时针排序

void TIM1_UP_IRQHandler(void)   
{
	static uint32_t it_count = 0;
//	GPIO_SetBits(GPIOC,GPIO_Pin_13);
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断源   
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
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源
//	GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	
} 

unsigned char read_key(void)
{
	static u8 flag_key=1;//按键按松开标志
	u8 key_state=GPIO_ReadInputDataBit(KEY_PORT,KEY_PIN);
	if(flag_key && key_state==0)
	{
		flag_key=0;
		return 1;	// 按键按下
	}
	else if(1==key_state)
		flag_key=1;
	return 0;//无按键按下
}


void motor_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;  
//	TIM_OCInitTypeDef TIM_OCInitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_TIM1, ENABLE);
	/*********端口启动**************************/
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
  TIM_TimeBaseStructure.TIM_Prescaler = 9;            //3分频
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;      //
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);          //初始化TIM3

  
 /* TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;    //输出比较翻转模式
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //使能通道1
  TIM_OCInitStructure.TIM_Pulse = 50000;                       //待装入输出比较寄存器中的脉冲值     
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         //输出为正逻辑 
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);        //写入配置

  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);          //使能或者失能TIMx在CCR1上的预装载寄存器
	TIM_CtrlPWMOutputs(TIM1, ENABLE);*/
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    
      
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;    
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;    
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
  NVIC_Init(&NVIC_InitStructure);  

	//TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设 
}

/*void TIM1_CC_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)               //检查指定的TIM中断发生与否
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
		
		CCR1_Val =120000/StepMotor.SetSpeed ;     //由速度计算比较值
		
		if(temp&0x0100) GPIOA->ODR &= 0xfeff;     //翻转
		else GPIOA->ODR |= 0x0100;
    TIM_SetCompare1(TIM1, capture + CCR1_Val );        //将计数值加上翻转的脉冲值写入输出比较寄存器中
		////////////////////////////////////////////////
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC1 );               //清除TIMx的中断待处理位
  }
}*/



