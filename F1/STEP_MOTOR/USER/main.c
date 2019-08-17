#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "DataScope_DP.h"
#include "usart.h"
#include "stm32f10x_tim.h"
#include "motor.h"
#include "MPU6050.h"
#include "ioi2c.h"

extern StepMotor_TypeDef  StepMotor;
extern float Pitch,Roll,Set_Pitch,Set_Roll; 


int main(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	/////////// 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	delay_init();                 //延时初始化
	uart_init(256000);   //用到了定时器3的50ms中断
//	sys_pulse();
	IIC_Init();
	MPU6050_initialize();
	DMP_Init();
	PB5_EXTI_Init();
	motor_Init();
	StepMotor.SetSpeed = 3000;
	
	Set_Roll = 0;
	
	while(1)
	{
		if(Roll < Set_Roll)
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_SetBits(GPIOB,GPIO_Pin_14);
		}
		else if(Roll > Set_Roll)
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
		}
		else
		{
			continue;
		}
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		delay_us(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		delay_us(100);
	}
}

