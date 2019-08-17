#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "mpu6050.h"
#include "encoder.h"
#include "moto.h"
#include "usart.h"
#include "nrf24l01.h"
#include "spi.h"

/**************
TIM2:PWM,TIM3:NRF,TIM4:SetCompare  外部数据输入和输出均在24l01.c的两个中断函数中
******************/

extern float cur_x,cur_x_v;
extern long pulse_r,pulse_l;
extern char dir_r,dir_l;  
extern short pwm_r,pwm_l;
extern struct
{
	float SumError;
	float LastError;
	float Proportion;
	float Integral;
	float Derivative;	
}pid_r,pid_l;

int main(void)
{
	delay_init();	 
	OLED_Init();
	SPI2_Config();
	//uart_init(115200);
	nrf24l01_init();
	moto_init();
	moto_ControlInit();
	usart3_init();
	encoder_ExtiInit();
	nrf24l01_tim_init();  //打开nrf发送信息中断，兼OLED显示刷新中断
	
	while(NRF24L01_Check()==1)
	{
		OLED_ShowString(0,0,"Checking...");
	}
	OLED_Clear();
	OLED_ShowString(0,0,"OK!");
	delay_ms(1000);
	
	while(1)
	{
		delay_ms(20);
	}
}

