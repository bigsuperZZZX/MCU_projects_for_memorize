#include "PWM_to_PPM.h"
#include "sys.h"

static u16 			Rc_Pwm_In[4];
static T_RC_Data 	Rc_Data;//1000~2000

void CAP_GPIO_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

u8 RC_GetCplt = 0;
extern int sys_pulsecount;

void CAP_data(void)
{
	static u16 temp_cnt0 = 0,temp_cnt1 = 0,temp_cnt2 = 0,temp_cnt3 = 0,temp_cnt4 = 0,temp_cnt5 = 0,temp_cnt6 = 0;
	static u16 last_cnt0 = 0,last_cnt1 = 0,last_cnt2 = 0,last_cnt3 = 0,last_cnt4 = 0,last_cnt5 = 0,last_cnt6 = 0;
	static u8  last_state0 = 0,last_state1 = 0,last_state2 = 0,last_state3 = 0,last_state4 = 0,last_state5 = 0,last_state6 = 0;
	static u8 CH_Select = 0;
	static u8 intever_time_count = 0;
	u16 Get_Data = (u16)GPIOA->IDR;
	//////
	if(Get_Data&0x01) 
	{
		temp_cnt0++;
		last_state0 = 1;
	}
	if((Get_Data & 0x01)==0 && last_state0==1)
	{
		last_state0 = 0;
		if(temp_cnt0>0x2D && temp_cnt0<0x6A)
		{
			Rc_Data.ROLL = (int)(0.741*temp_cnt0);
			last_cnt0 = Rc_Data.ROLL;
		}
		else
			Rc_Data.ROLL = last_cnt0;
		temp_cnt0 = 0;
	}
	//////
	if(Get_Data&0x02) 
	{
		temp_cnt1++;
		last_state1 = 1;
	}
	if((Get_Data & 0x02)==0 && last_state1==1)
	{
		last_state1 = 0;
		if(temp_cnt1>0x2D && temp_cnt1<0x6A)
		{
			Rc_Data.PITCH = (int)(0.741*temp_cnt1);
			last_cnt1 = Rc_Data.PITCH;
		}
		else
			Rc_Data.PITCH = last_cnt1;
		temp_cnt1 = 0;
	}
	//////
	if(Get_Data&0x04) 
	{
		temp_cnt2++;
		last_state2 = 1;
	}
	if((Get_Data & 0x04)==0 && last_state2==1)
	{
		last_state2 = 0;
		if(temp_cnt2>0x32 && temp_cnt2<0x5A)
		{
			Rc_Data.THROTTLE = (int)(0.7142*temp_cnt2);
			last_cnt2 = Rc_Data.THROTTLE;
		}
		else
			Rc_Data.THROTTLE = last_cnt2;
		temp_cnt2 = 0;
	}
	//////
	if(Get_Data&0x08) 
	{
		temp_cnt3++;
		last_state3 = 1;
	}
	if((Get_Data & 0x08)==0 && last_state3==1)
	{
		last_state3 = 0;
		if(temp_cnt3>0x2D && temp_cnt3<0x6A)
		{
			Rc_Data.YAW = (int)(0.741*temp_cnt3);
			last_cnt3 = Rc_Data.YAW ;
		}
		else
			Rc_Data.YAW = last_cnt3;
		temp_cnt3 = 0;
	}
	//////
	if(Get_Data&0x10) 
	{
		temp_cnt4++;
		last_state4 = 1;
	}
	if((Get_Data & 0x10)==0 && last_state4==1)
	{
		last_state4 = 0;
		if(temp_cnt4>0x2D && temp_cnt4<0x6A)
			Rc_Data.AUX1 = (int)(0.69*temp_cnt4);
		temp_cnt4 = 0;
	}
	//////
	if(Get_Data&0x20) 
	{
		temp_cnt5++;
		last_state5 = 1;
	}
	if((Get_Data & 0x20)==0 && last_state5==1)
	{
		last_state5 = 0;
		if(temp_cnt5>0x2D && temp_cnt5<0x6A)
			Rc_Data.AUX2 = (int)(0.69*temp_cnt5);
		temp_cnt5 = 0;
	}
	//////
	if(Get_Data&0x40) 
	{
		temp_cnt6++;
		last_state6 = 1;
	}
	if((Get_Data & 0x40)==0 && last_state6==1)
	{
		last_state6 = 0;
		if(temp_cnt6>0x2D && temp_cnt6<0x6A)
			Rc_Data.AUX3 = (int)(0.69*temp_cnt6);
		temp_cnt6 = 0;
		RC_GetCplt = 1;
	}
	//////////////////////////////////////////////////
	if(RC_GetCplt==1)
	{
		if(CH_Select == 0)
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_7);
			intever_time_count++;
			if(intever_time_count == 21)
			{
					intever_time_count = 0;
					CH_Select = 1;
			}
		}
		////////////	
		if(CH_Select == 1)
		{
			if(Rc_Data.ROLL>0)
			{
				Rc_Data.ROLL--;
				GPIO_SetBits(GPIOA, GPIO_Pin_7);
			}
			else
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_7);
				intever_time_count ++;
				if(intever_time_count == 21)
				{
					intever_time_count = 0;
					CH_Select = 2;
				}
			}
		}
		////////////
		if(CH_Select == 2)
		{
			if(Rc_Data.PITCH>0)
			{
				Rc_Data.PITCH--;
				GPIO_SetBits(GPIOA, GPIO_Pin_7);
			}
			else
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_7);
				intever_time_count ++;
				if(intever_time_count == 21)
				{
					intever_time_count = 0;
					CH_Select = 3;
				}
			}
		}
		////////////
		if(CH_Select == 3)
		{
			if(Rc_Data.THROTTLE>0)
			{
				Rc_Data.THROTTLE--;
				GPIO_SetBits(GPIOA, GPIO_Pin_7);
			}
			else
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_7);
				intever_time_count ++;
				if(intever_time_count == 21)
				{
					intever_time_count = 0;
					CH_Select = 4;
				}
			}
		}
		////////////
		if(CH_Select == 4)
		{
			if(Rc_Data.YAW>0)
			{
				Rc_Data.YAW--;
				GPIO_SetBits(GPIOA, GPIO_Pin_7);
			}
			else
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_7);
				intever_time_count ++;
				if(intever_time_count == 21)
				{
					intever_time_count = 0;
					CH_Select = 5;
				}
			}
		}
		////////////
		if(CH_Select == 5)
		{
			if(Rc_Data.AUX1>0)
			{
				Rc_Data.AUX1--;
				GPIO_SetBits(GPIOA, GPIO_Pin_7);
			}
			else
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_7);
				intever_time_count ++;
				if(intever_time_count == 21)
				{
					intever_time_count = 0;
					CH_Select = 6;
				}
			}
		}
		////////////
		if(CH_Select == 6)
		{
			if(Rc_Data.AUX2>0)
			{
				Rc_Data.AUX2--;
				GPIO_SetBits(GPIOA, GPIO_Pin_7);
			}
			else
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_7);
				intever_time_count ++;
				if(intever_time_count == 21)
				{
					intever_time_count = 0;
					CH_Select = 7;
				}
			}
		}
		////////////
		if(CH_Select == 7)
		{
			if(Rc_Data.AUX3>0)
			{
				Rc_Data.AUX3--;
				GPIO_SetBits(GPIOA, GPIO_Pin_7);
			}
			else
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_7);
				intever_time_count ++;
				if(intever_time_count == 21)
				{
					intever_time_count = 0;
					CH_Select = 0;
					RC_GetCplt = 0;
				}
			}
		}
	}
	else
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_7);
	}
	
}


void CAP_init(void)
{
	
	 GPIO_InitTypeDef GPIO_InitStructure;
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	 TIM_ICInitTypeDef  TIM_ICInitStructure;
	 NVIC_InitTypeDef NVIC_InitStructure;
	
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3,ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
	//GPIO 初始化
	//TIM2 CH1-4 //TIM3 CH1-2
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_6|GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//TIM3 CH3-4
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
              /* Enable the TIM3 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
		
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	 TIM_DeInit(TIM2);
   TIM_InternalClockConfig(TIM2);
   TIM_TimeBaseStructure.TIM_Prescaler = 71;//预分频系数为36000-1，这样计数器时钟为10kHZ
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//设置时钟分割
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //设置计数器模式为向上计数模式
   TIM_TimeBaseStructure.TIM_Period = 0xffff-1; //设置计数溢出大小，每计2000个数就产生一个更新事件  
   TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);//将配置应用到TIM2中  
   //下面是对 TIM_ICInitStructure的配置
   TIM_ICInitStructure.TIM_Channel = TIM_Channel_1|TIM_Channel_2|TIM_Channel_3|TIM_Channel_4;
   TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
   TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
   TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;    
   TIM_ICInitStructure.TIM_ICFilter = 0x01;       /*选择输入比较滤波器，滤波设置，经历几个周期跳变认定波形稳定0x0～0xF*/
   TIM_ICInit(TIM2, &TIM_ICInitStructure);
 
  //开启TIM2的中断
		TIM_ClearFlag(TIM2, TIM_IT_CC1);
		TIM_ClearFlag(TIM2, TIM_IT_CC2);
		TIM_ClearFlag(TIM2, TIM_IT_CC3);
		TIM_ClearFlag(TIM2, TIM_IT_CC4);
		
		TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
		
		TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);
		TIM_ITConfig(TIM2,TIM_IT_CC2,ENABLE);
		TIM_ITConfig(TIM2,TIM_IT_CC3,ENABLE);
		TIM_ITConfig(TIM2,TIM_IT_CC4,ENABLE);
 
		
		
	 TIM_DeInit(TIM3);
   TIM_InternalClockConfig(TIM3);
   TIM_TimeBaseStructure.TIM_Prescaler = 71;//预分频系数为36000-1，这样计数器时钟为72MHz/36000 = 2kHz
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//设置时钟分割
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //设置计数器模式为向上计数模式
   TIM_TimeBaseStructure.TIM_Period = 0xffff-1; //设置计数溢出大小，每计2000个数就产生一个更新事件  
   TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//将配置应用到TIM2中  
   //下面是对 TIM_ICInitStructure的配置
   TIM_ICInitStructure.TIM_Channel = TIM_Channel_1|TIM_Channel_2|TIM_Channel_3|TIM_Channel_4;
   TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
   TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
   TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;    
   TIM_ICInitStructure.TIM_ICFilter = 0x01;       /*选择输入比较滤波器，滤波设置，经历几个周期跳变认定波形稳定0x0～0xF*/
   TIM_ICInit(TIM3, &TIM_ICInitStructure);
 
  //开启TIM2的中断
		TIM_ClearFlag(TIM3, TIM_IT_CC1);
		TIM_ClearFlag(TIM3, TIM_IT_CC2);
		TIM_ClearFlag(TIM3, TIM_IT_CC3);
		TIM_ClearFlag(TIM3, TIM_IT_CC4);
		
		TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
		
		TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
		TIM_ITConfig(TIM3,TIM_IT_CC2,ENABLE);
		TIM_ITConfig(TIM3,TIM_IT_CC3,ENABLE);
		TIM_ITConfig(TIM3,TIM_IT_CC4,ENABLE);
 

}


void TIM2_IRQHandler(void)
{
	static u16 temp_cnt1,temp_cnt1_2,temp_cnt2,temp_cnt2_2,temp_cnt3,temp_cnt3_2,temp_cnt4,temp_cnt4_2;
	
	if(TIM2->SR & TIM_IT_CC1)//if(TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET)
	{
		u16 ccr1 = TIM2->CCR1;
		TIM2->SR &= ~TIM_FLAG_CC1OF;
		if(GPIOA->IDR & GPIO_Pin_0)
		{
			temp_cnt1 = ccr1;
			TIM2->CCER |= (1<<1);
		}
		else
		{
			temp_cnt1_2 = ccr1;
			TIM2->CCER &= ~(1<<1);
			if(temp_cnt1_2>=temp_cnt1)
				Rc_Pwm_In[0] = temp_cnt1_2-temp_cnt1;
			else
				Rc_Pwm_In[0] = 0xffff-temp_cnt1+temp_cnt1_2;
		}
	}
	if(TIM2->SR & TIM_IT_CC2)
	{
		u16 ccr2 = TIM2->CCR2;
		if(GPIOA->IDR & GPIO_Pin_1)
		{
			temp_cnt2 = ccr2;
			TIM2->CCER |= (1<<5);
		}
		else
		{
			temp_cnt2_2 = ccr2;
			TIM2->CCER &= ~(1<<5);
			if(temp_cnt2_2>=temp_cnt2)
				Rc_Pwm_In[1] = temp_cnt2_2-temp_cnt2;
			else
				Rc_Pwm_In[1] = 0xffff-temp_cnt2+temp_cnt2_2;
		}
	}
	if(TIM2->SR & TIM_IT_CC3)
	{
		u16 ccr3 = TIM2->CCR3;
		if(GPIOA->IDR & GPIO_Pin_2)
		{
			temp_cnt3 = ccr3;
			TIM2->CCER |= (1<<9);
		}
		else
		{
			temp_cnt3_2 = ccr3;
			TIM2->CCER &= ~(1<<9);
			if(temp_cnt3_2>=temp_cnt3)
				Rc_Pwm_In[2] = temp_cnt3_2-temp_cnt3;
			else
				Rc_Pwm_In[2] = 0xffff-temp_cnt3+temp_cnt3_2;
		}
	}
	if(TIM2->SR & TIM_IT_CC4)
	{
		u16 ccr4 = TIM2->CCR4;
		if(GPIOA->IDR & GPIO_Pin_3)
		{
			temp_cnt4 = ccr4;
			TIM2->CCER |= (1<<13);
		}
		else
		{
			temp_cnt4_2 = ccr4;
			TIM2->CCER &= ~(1<<13);
			if(temp_cnt4_2>=temp_cnt4)
				Rc_Pwm_In[3] = temp_cnt4_2-temp_cnt4;
			else
				Rc_Pwm_In[3] = 0xffff-temp_cnt4+temp_cnt4_2;
				
			Rc_Data.THROTTLE	=	Rc_Pwm_In[2];
			Rc_Data.YAW				=	Rc_Pwm_In[3];
			Rc_Data.ROLL			=	Rc_Pwm_In[0];
			Rc_Data.PITCH			=	Rc_Pwm_In[1];
		}
		
	}
	
}

void TIM3_IRQHandler(void)
{
	static u16 temp_cnt1,temp_cnt1_2,temp_cnt2,temp_cnt2_2,temp_cnt3,temp_cnt3_2;
	
	if(TIM3->SR & TIM_IT_CC1)//if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET)
	{
		u16 ccr1 = TIM3->CCR1;
		TIM3->SR &= ~TIM_FLAG_CC1OF;
		if(GPIOA->IDR & GPIO_Pin_6)
		{
			temp_cnt1 = ccr1;
			TIM3->CCER |= (1<<1);
		}
		else
		{
			temp_cnt1_2 = ccr1;
			TIM3->CCER &= ~(1<<1);
			if(temp_cnt1_2>=temp_cnt1)
				Rc_Pwm_In[0] = temp_cnt1_2-temp_cnt1;
			else
				Rc_Pwm_In[0] = 0xffff-temp_cnt1+temp_cnt1_2;
		}
	}
	if(TIM3->SR & TIM_IT_CC2)
	{
		u16 ccr2 = TIM3->CCR2;
		if(GPIOA->IDR & GPIO_Pin_7)
		{
			temp_cnt2 = ccr2;
			TIM3->CCER |= (1<<5);
		}
		else
		{
			temp_cnt2_2 = ccr2;
			TIM3->CCER &= ~(1<<5);
			if(temp_cnt2_2>=temp_cnt2)
				Rc_Pwm_In[1] = temp_cnt2_2-temp_cnt2;
			else
				Rc_Pwm_In[1] = 0xffff-temp_cnt2+temp_cnt2_2;
		}
	}
	if(TIM3->SR & TIM_IT_CC3)
	{
		u16 ccr3 = TIM3->CCR3;
		if(GPIOB->IDR & GPIO_Pin_0)
		{
			temp_cnt3 = ccr3;
			TIM3->CCER |= (1<<9);
		}
		else
		{
			temp_cnt3_2 = ccr3;
			TIM3->CCER &= ~(1<<9);
			if(temp_cnt3_2>=temp_cnt3)
				Rc_Pwm_In[2] = temp_cnt3_2-temp_cnt3;
			else
				Rc_Pwm_In[2] = 0xffff-temp_cnt3+temp_cnt3_2;
				
			Rc_Data.AUX1	    =	Rc_Pwm_In[0];
			Rc_Data.AUX2			=	Rc_Pwm_In[1];
			Rc_Data.AUX3			=	Rc_Pwm_In[2];
		}
	}
	
}

/*void CAP_data(void)
{
	static u16 temp_cnt0 = 0,temp_cnt1 = 0,temp_cnt2 = 0,temp_cnt3 = 0,temp_cnt4 = 0,temp_cnt5 = 0,temp_cnt6 = 0;
	static u8  last_state0 = 0,last_state1 = 0,last_state2 = 0,last_state3 = 0,last_state4 = 0,last_state5 = 0,last_state6 = 0;
	u16 Get_Data = (u16)GPIOA->IDR;
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	//////
	if(Get_Data&0x01) 
	{
		temp_cnt0++;
		last_state0 = 1;
	}
	if((Get_Data & 0x01)==0 && last_state0==1)
	{
		last_state0 = 0;
		if(temp_cnt0>0x5A && temp_cnt0<0xD4)
			Rc_Data.ROLL = temp_cnt0;
		temp_cnt0 = 0;
	}
	//////
	if(Get_Data&0x02) 
	{
		temp_cnt1++;
		last_state1 = 1;
	}
	if((Get_Data & 0x02)==0 && last_state1==1)
	{
		last_state1 = 0;
		if(temp_cnt1>0x5A && temp_cnt1<0xD4)
			Rc_Data.PITCH = temp_cnt1;
		temp_cnt1 = 0;
	}
	//////
	if(Get_Data&0x04) 
	{
		temp_cnt2++;
		last_state2 = 1;
	}
	if((Get_Data & 0x04)==0 && last_state2==1)
	{
		last_state2 = 0;
		if(temp_cnt2>0x5A && temp_cnt2<0xD4)
			Rc_Data.THROTTLE = temp_cnt2;
		temp_cnt2 = 0;
	}
	//////
	if(Get_Data&0x08) 
	{
		temp_cnt3++;
		last_state3 = 1;
	}
	if((Get_Data & 0x08)==0 && last_state3==1)
	{
		last_state3 = 0;
		if(temp_cnt3>0x5A && temp_cnt3<0xD4)
			Rc_Data.YAW = temp_cnt3;
		temp_cnt3 = 0;
	}
	//////
	if(Get_Data&0x10) 
	{
		temp_cnt4++;
		last_state4 = 1;
	}
	if((Get_Data & 0x10)==0 && last_state4==1)
	{
		last_state4 = 0;
		if(temp_cnt4>0x5A && temp_cnt4<0xD4)
			Rc_Data.AUX1 = temp_cnt4;
		temp_cnt4 = 0;
	}
	//////
	if(Get_Data&0x20) 
	{
		temp_cnt5++;
		last_state5 = 1;
	}
	if((Get_Data & 0x20)==0 && last_state5==1)
	{
		last_state5 = 0;
		if(temp_cnt5>0x5A && temp_cnt5<0xD4)
			Rc_Data.AUX2 = temp_cnt5;
		temp_cnt5 = 0;
	}
	//////
	if(Get_Data&0x40) 
	{
		temp_cnt6++;
		last_state6 = 1;
	}
	if((Get_Data & 0x40)==0 && last_state6==1)
	{
		last_state6 = 0;
		if(temp_cnt6>0x5A && temp_cnt6<0xD4)
			Rc_Data.AUX3 = temp_cnt6;
		temp_cnt6 = 0;
	}
	//////////////////////////////////////////////////
	if(last_state6==0)
	{
		RC_GetCplt = 1;
	}
	
	
	
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
}*/

