/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes -------------------------------------*/
#include "exti.h"
#include "stm32f10x_exti.h" 
#include "stm32f10x_tim.h" 
#include "stm32f10x_gpio.h"
#include "TIM.h" 
#include "motor_control.h"

extern float speed_1,speed_2;
extern long pulse_1,pulse_2;
extern char clockwise_1,clockwise_2;
extern struct pid
{ 
	short SetPulse;//定义设定值
	short ActualPulse;//定义实际值
	short err; //定义偏差值 
	short err_last;//定义上一个偏差值 
	float Kp,Ki,Kd;//定义比例、积分、微分系数
	short PWM;//定义PWM值（控制执行器的变量）
	short integral;//定义积分值 
}moto_angle_1,moto_angle_2;


void EXTI0_IRQHandler(void)  //外部中断2入口函数
{
	if(EXTI_GetITStatus(EXTI_Line0)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line0);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==0)
		{
			clockwise_1=1;
			pulse_1++;
		}
		else
		{
			clockwise_1=0;
			pulse_1--;
		}
	}
}

void EXTI2_IRQHandler(void)  //外部中断2入口函数
{
	if(EXTI_GetITStatus(EXTI_Line2)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0)
		{
			clockwise_2=1;
			pulse_2++;
		}
		else
		{
			clockwise_2=0;
			pulse_2--;
		}
	}
}

void TIM3_IRQHandler(void)  
{  
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
	moto_1_pid();
	if(moto_angle_1.PWM>0)
	{
		TIM_SetCompare2(TIM4,0);
		TIM_SetCompare1(TIM4,moto_angle_1.PWM);
	}
	else if(moto_angle_1.PWM<=0)
	{
		TIM_SetCompare1(TIM4,0);
		TIM_SetCompare2(TIM4,-moto_angle_1.PWM);
	}
	moto_2_pid();
	if(moto_angle_2.PWM>0)
	{
		TIM_SetCompare4(TIM4,0);
		TIM_SetCompare3(TIM4,moto_angle_2.PWM);
	}
	else if(moto_angle_2.PWM<=0)
	{
		TIM_SetCompare3(TIM4,0);
		TIM_SetCompare4(TIM4,-moto_angle_1.PWM);
	}
}  

 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
