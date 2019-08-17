#include "control.h"	
#include "oled.h"
#include "MPU6050.h"
#include "stm32f10x_tim.h"
#include "usart.h"

extern float Pitch,Roll,Set_Pitch,Set_Roll; 

void EXTI9_5_IRQHandler(void) 
{
	if(PBin(5)==0)		
	{   
		 EXTI->PR=1<<5;                                                      //清除LINE5上的中断标志位   
	   Read_DMP();		 //===更新姿态
		 printf("%.2f %.2f\r\n",Pitch,Roll);
	}
}

