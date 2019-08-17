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
#include "spi.h"

/**************
TIM2:PWM,TIM3:NRF,TIM4:SetCompare  外部数据输入和输出均在24l01.c的两个中断函数中
******************/

float set_x;
extern long pulse_r,pulse_l;
extern char dir_r,dir_l;  
extern short pwm_r,pwm_l;

int main(void)
{
	delay_init();

	delay_ms(1500);	
	delay_ms(1500);	
	
	OLED_Init();
	photoelectric_init();
	moto_init();
	encoder_ExtiInit();
	
	while(1)
	{
		delay_ms(20);
	}
}

