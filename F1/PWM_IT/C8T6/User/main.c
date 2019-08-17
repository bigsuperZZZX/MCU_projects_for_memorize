#include "stm32f10x.h"
#include "stm32f10x_rcc.h"



void delay(u32 n)
{
	while(n--);
}

void EXTI2_IRQHandler(void)  //外部中断2入口函数
{
	if(EXTI_GetITStatus(EXTI_Line2)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);
		delay(80000);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0)
		{
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==0)
				GPIO_SetBits(GPIOB,GPIO_Pin_8);
			else
				GPIO_ResetBits(GPIOB,GPIO_Pin_8);		
		}
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0);
	}
}

void exti_init()  //interrupt 2
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	EXTI_InitTypeDef EXTI_InitStructure;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void LED_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}


void GPIO_Config(void)  
{ 
  GPIO_InitTypeDef GPIO_InitStructure; 
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;      
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
} 


void TIM3_Config(void)  
{  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_OCInitTypeDef  TIM_OCInitStructure;  
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
	
    TIM_TimeBaseStructure.TIM_Period =3000;  
    TIM_TimeBaseStructure.TIM_Prescaler = 60;                             
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                           
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;           
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
    /* PWM1 Mode configuration: Channel1 */  
	
    TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2; //模式2，输入数字代表占空比高
		TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	
		TIM_OC1Init(TIM3, & TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
		TIM_OC2Init(TIM3, & TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);                               
    /* TIM3 enable counter */  
    TIM_Cmd(TIM3, ENABLE);   

		TIM_SetCompare1(TIM3,1500);
		TIM_SetCompare2(TIM3,1400);
} 
	
void key_scan()
{
	static u16 pwm=1500;
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_8);
		delay(80000);
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0)
		{
			TIM_SetCompare2(TIM3,pwm+=200);
			if(pwm>2800)
			{
				pwm=1500;
			}
		}
		while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0);
	}
}

/*******************************************/


int main(void) 
{ 
			int i;
	
    SystemInit(); 
		exti_init();
		LED_Init();
    GPIO_Config(); 
    TIM3_Config(); 
		GPIO_Write(GPIOB	,(u16)0xff);
	
	
			
			for(i=1000;i<2800;i++)
			{
				TIM_SetCompare1(TIM3,i);
				delay(50000);
			}
			for(i=2800;i>1000;i--)
			{
				TIM_SetCompare1(TIM3,i);
				delay(50000);
			}
			
			for(i=1000;i<2800;i++)
			{
				TIM_SetCompare1(TIM3,i);
				delay(50000);
			}
			for(i=2800;i>1000;i--)
			{
				TIM_SetCompare1(TIM3,i);
				delay(50000);
			}
			
		TIM_SetCompare2(TIM3,1500);
    while (1) 
    { 
		//	key_scan();
  /*   for(i=0;i<1000;i++)
			{
				TIM_SetCompare1(TIM3,i);
				delay(40000);
			}
			for(i=1000;i>0;i--)
			{
				TIM_SetCompare1(TIM3,i);
				delay(40000);
			}*/
			
	/*		for(i=1000;i<2800;i++)
			{
				TIM_SetCompare1(TIM3,i);
				delay(100000);
			}
			for(i=2800;i>1000;i--)
			{
				TIM_SetCompare1(TIM3,i);
				delay(100000);
			}*/
		TIM_SetCompare2(TIM3,2000);
				delay(800000);
				
		}
} 
