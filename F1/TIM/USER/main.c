#include "delay.h"
#include "oled.h"
#include "bmp.h"
#include "stm32f10x.h"

/* 
*说明： 
*PA0:KEY1;PA1:KEY2; 
*PA2:LED1;PA3:LED2; 
*PA9:USART1_TX;PA10:USART1_RX 
*/  
#include "stm32f10x.h"  
#include "stm32f10x_rcc.h"  
#include "stm32f10x_gpio.h"  
#include "stm32f10x_tim.h"  
#include "stm32f10x_exti.h"  
#include "system_stm32f10x.h"  
#include "misc.h"  

//定时器超声波测距

extern u32 t;
  
void RCC_Configuration(void);  
void GPIO_Configuration(void);   
void TIM3_Configuration(void);  
void NVIC_Configuration(void);  
  
int main()  
{  
    SystemInit(); 
		delay_init();	    	 //延时函数初始化	  
		OLED_Init();	
      
    RCC_Configuration();  
    GPIO_Configuration();   
    TIM3_Configuration();  
    NVIC_Configuration();
	
	//	OLED_DrawPoint(20,15,1);
	
    while(1)
		{
			t=0;
			GPIO_SetBits(GPIOA,GPIO_Pin_0);
			delay_us(20);
			GPIO_ResetBits(GPIOA,GPIO_Pin_0);
			delay_ms(800);
			OLED_ShowNum(0,0,t/100,2,16);
			OLED_ShowNum(22,0,t%100,2,16);
			
		}			
}  
  
void RCC_Configuration(void)    
{     
  ////USART2和USART3都在在APB1上而USART1是在APB2上的  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);     
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);     
}   
  
void GPIO_Configuration(void)    
{    
  GPIO_InitTypeDef GPIO_InitStructure;    
    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;     
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;     
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
}  
  
void TIM3_Configuration(void)    
{  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
    
  TIM_TimeBaseStructure.TIM_Period = 121;//频率：72MHz  72000000/36000=2000   进入中断时间间隔  TIM_Period/（72M/(TIM_Prescaler+1)）s
  TIM_TimeBaseStructure.TIM_Prescaler = 34;//36000-1=35999  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
    
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );  
  TIM_Cmd(TIM3,ENABLE);  
}  
  
void NVIC_Configuration(void)//配置中断优先级    
{    
  NVIC_InitTypeDef NVIC_InitStructure;    
      
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);    
      
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;    
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;    
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
  NVIC_Init(&NVIC_InitStructure);    
}    
