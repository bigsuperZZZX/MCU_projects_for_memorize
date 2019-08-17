#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "oled.h"
#include "mpu6050.h"
#include "AngleOutput.h"
#include "math.h"
#include "stm32f10x_tim.h"
#include "moto.h"

extern float cur_x,offset_x;
extern MPU6050_TypeDef	MPU6050_InitStructure;
extern float x;


int main(void)
{
//	u16 i;
	
	delay_init();
//	moto_init() ;
	OLED_Init();
	usart3_init(); 
	NVIC_init();
	TIM3_Configuration();
	
	
	while(1)
	{
		OLED_ShowNum(0,0,x,8,16);
		
	}
}
