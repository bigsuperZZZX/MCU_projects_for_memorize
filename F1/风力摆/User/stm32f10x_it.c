/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "delay.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "control.h"
#include "moto.h"

unsigned char counter=0;        //���������ݵ�ʱ�����
static unsigned char Temp[11];  //mpu6050���ݽ����ж�
float cur_x,cur_y,cur_z,offset_x=0,offset_y=0;							//��ǰ��x�ǣ�y��  �����������x������y���꣩
u8 set_n;

extern struct
{
	float SumError;
	float LastError;
	float Proportion;
	float Integral;
	float Derivative;	
}PID_X,PID_Y;


unsigned char SET_buf[8],set_counter=0;  //�洢�����յ���ÿһ���� //���������ݵ�ʱ�����
static unsigned char set_Temp[6];   //���ڿ����ж�
int set_a=0,set_r=0;									//�趨���ҳ��ͽǶ� ��ˮƽֽ���ϵģ�
u8 set_mode=0;												//ģʽ���㶨λ����Բ��д�֣�
unsigned char set_quadrant=1;					//����
float set_x,set_y;										//�趨��x�ǣ�y��  �����������x������y���꣩

u8 sigh_ModeChange=0;              //ģʽ�ı��־



void USART1_IRQHandler(void)   //����ģʽ�趨
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
     {
			set_Temp[set_counter] = USART_ReceiveData(USART1);
      set_counter++; 
			if(set_Temp[0]=='p'||set_Temp[0]=='l')
			{
				sigh_ModeChange=1;
				PID_X.LastError=0;
				PID_X.SumError=0;
				PID_Y.LastError=0;
				PID_Y.SumError=0;
				switch (set_counter)
				{
					case 1:  set_mode=set_Temp[0];break;
					case 3: {memcpy(&SET_buf[0],&set_Temp[1],2);
									 SET_buf[2]='\0'; 
									 set_r=atoi(&SET_buf[0]); 
									 break;
									}
					case 6: {memcpy(&SET_buf[2],&set_Temp[3],3); 
									 SET_buf[5]='\0'; 
									 set_a=atoi(&SET_buf[2]);
									 set_counter=0;
										
										set_x=set_r*cos(set_a/conver);
										set_y=set_r*sin(set_a/conver);
					/**********�趨x��y����Ƕ�**************/				 
									 break;
									}
					default: break;
				}
				if(set_Temp[0]=='l')
				{
					set_x*=1.14;
					set_y*=1.14;
				}
			}
			else if(set_Temp[0]=='c')
			{
				sigh_ModeChange=1;
				switch (set_counter)
				{
					case 1: set_mode=set_Temp[0];break;
					case 3: {memcpy(&SET_buf[0],&set_Temp[1],2); 
									 SET_buf[2]='\0'; 
									 set_r=atoi(&SET_buf[0])*1.15;
									 set_counter=0;
                   break;
									}
					default: break;
				}
			}
			else if(set_Temp[0]=='w')
			{
				sigh_ModeChange=1;
				switch (set_counter)
				{
					case 1: set_mode=set_Temp[0];break;
					case 2: {memcpy(&SET_buf[0],&set_Temp[1],1); 
									 SET_buf[1]='\0'; 
									 set_n=atoi(&SET_buf[0]);
									 set_counter=0;						
									 PID_X.SumError=0;
									 PID_X.LastError=0;
									 PID_Y.SumError=0;
									 PID_Y.LastError=0;
                   break;
									}
					default: break;
				}
			}
			else if(set_Temp[0]=='+')
			{
				switch (set_counter)
				{
					case 3: {memcpy(&SET_buf[0],&set_Temp[1],2);
									 SET_buf[2]='\0'; 
									 PID_X.Proportion+=atoi(&SET_buf[0]); 
									 PID_Y.Proportion=PID_X.Proportion; 						
									 break;
									}
					case 4: {SET_buf[3]=set_Temp[3];
									 SET_buf[4]='\0'; 
									 PID_X.Integral += (float)atoi(&SET_buf[3])/100;
									 PID_Y.Integral=PID_X.Integral;						
									 break;
									}
					case 6: {memcpy(&SET_buf[5],&set_Temp[4],2); 
									 SET_buf[7]='\0'; 
									 PID_X.Derivative += atoi(&SET_buf[5]);
									 PID_Y.Derivative=PID_X.Derivative;
									 set_counter=0;			 
									 break;
									}
					default: break;
				}
			}
			else if(set_Temp[0]=='-')
			{
				switch (set_counter)
				{
					case 3: {memcpy(&SET_buf[0],&set_Temp[1],2);
									 SET_buf[2]='\0'; 
									 PID_X.Proportion-=atoi(&SET_buf[0]); 
									 PID_Y.Proportion=PID_X.Proportion; 						
									 break;
									}
					case 4: {SET_buf[3]=set_Temp[3];
									 SET_buf[4]='\0'; 
									 PID_X.Integral -= (float)atoi(&SET_buf[3])/100;
									 PID_Y.Integral=PID_X.Integral;						
									 break;
									}
					case 6: {memcpy(&SET_buf[5],&set_Temp[4],2); 
									 SET_buf[7]='\0'; 
									 PID_X.Derivative -= atoi(&SET_buf[5]);
									 PID_Y.Derivative=PID_X.Derivative;
									 set_counter=0;			 
									 break;
									}
					default: break;
				}
			}
			else if(set_Temp[0]=='j')
			{
				offset_x+=cur_x;
				offset_y+=cur_y;
				set_Temp[0]=0;				
				set_counter=0;
			}
			else if(set_Temp[0]=='s')
			{
				if(set_counter==5)
				{
					memcpy(&SET_buf[0],&set_Temp[1],4); 
					SET_buf[4]='\0'; 
					TIM_SetCompare4(TIM2,atoi(&SET_buf[0]));
					set_counter=0;		
				}
			}
			else
			{
				set_mode=set_Temp[0];
				set_counter=0;
			}
		 }
}

/***************************************************************************/

void USART3_IRQHandler(void)
{
	static unsigned char n=0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж���Ч,���������ݼĴ�����
     {
      Temp[counter] = USART_ReceiveData(USART3);   //��������
			if(counter == 0 && Temp[0] != 0x55) return;      //�� 0 �����ݲ���֡ͷ������
      counter++; 
      if(counter==11) //���յ� 11 ������
      { 
         counter=0; //���¸�ֵ��׼����һ֡���ݵĽ��� 
         if(Temp[1]==0x53)
         {
           cur_y = H*tan(((short)(Temp[3]<<8| Temp[2]))/32768.0*180/conver)-offset_y;   //X���ת�ǣ�y �ᣩ
           cur_x = H*tan(((short)(Temp[5]<<8| Temp[4]))/32768.0*180/conver)-offset_x;   //Y�ḩ���ǣ�x �ᣩ
//					 cur_z = ((short)(Temp[7]<<8| Temp[6]))/32768.0*180;
					 switch (set_mode)
					 {
						 case 'p':{       //����
							 if(sigh_ModeChange==1)
							 {
								 sigh_ModeChange=0;
								 PID_X.SumError=0;
								 PID_X.LastError=0;
								 PID_Y.SumError=0;
								 PID_Y.LastError=0;
							 }
							FixPoint();
							break;}
						 case 'c':{				//��Բ
							 if(sigh_ModeChange==1)
							 {
									n=0;
								  sigh_ModeChange=0;
								  PID_X.SumError=0;
								  PID_X.LastError=0;
								  PID_Y.SumError=0;
									PID_Y.LastError=0;
							 }						 
							 if(n==40)
							 {
								 n=0;
								 circle_PointMove(set_r);
							 }
							 FixPoint();
							 n++;
							 break;}
						 
						 case 'l':{       //����
							 if(sigh_ModeChange==1)
							 {
								 sigh_ModeChange=0;						 
							 }
							 line_PointMove();							 
							FixPoint();
							break;}
						 
						 case 'w':{       //д����
							 if(n==127)
							 {
								 n=0;
								 number_PointMove();
							 }
							 FixPoint();
							 n++;
							break;}
						 
						 case 's':{					//�����������
							 TIM_SetCompare1(TIM2,atoi(&SET_buf[0]));
							 TIM_SetCompare2(TIM2,atoi(&SET_buf[0]));
							 TIM_SetCompare3(TIM2,atoi(&SET_buf[0]));
							 TIM_SetCompare4(TIM2,atoi(&SET_buf[0]));
						 break;}
						 default: {
							 TIM_SetCompare1(TIM2,0);
							 TIM_SetCompare2(TIM2,0);
							 TIM_SetCompare3(TIM2,0);
							 TIM_SetCompare4(TIM2,0);
						 break;}
						 
					 }						 
         }
      }
		 }
}



/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

void EXTI2_IRQHandler(void)  //�ⲿ�ж�2��ں���
{
}


void TIM3_IRQHandler(void)  //��ʱ��3��ں���
{
/*	static u8 i=0;
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	GPIO_Write(GPIOC,(u16)(0x01<<i++));
	if(i==8)
		i=0;*/
}
