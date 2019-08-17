#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "moto.h"
#include "stm32f10x_tim.h"
#include "encoder.h"
#include "usart.h"
#include "myiic.h"
#include "LDC1314.h"
#include "math.h"

#define CONVERT 577//一个脉冲0.0577cm(15cm一圈)

extern u32 counter;
extern u16 PWM_M_L,PWM_M_R,PWM_C;
extern u8 stop_flag;
extern u16 coil_0,coil_1,coil_2,coil_3;
extern u32 time;

int main(void)
{
	u16 t=50;
	
	delay_init();	    	 //延时函数初始化	  
	OLED_Init();			//初始化OLED 
	PWM_init();
	uart_init(38400);
	IIC_Init();
	LDC1314Multi_init();
	encoder_exit_init();
	
	OLED_ShowString(0,0,"ECOD:");
	OLED_ShowString(0,2,"P_M:    P_C:");
	OLED_ShowString(0,4,"TIME:");
	OLED_ShowString(0,6,"dist:");
	
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	LDC1314_DataPrepare();
	time_init();
	while(1)
	{
		if(t==100)
		{
			t=0;
			printf("X0X%d\r",LDC_Read_Reg(0X00));
//			OLED_ShowNum(22,4,coil_0,4,16);
			printf("X1X%d\r",LDC_Read_Reg(0X02));
//			OLED_ShowNum(86,4,coil_1,4,16);
			printf("X2X%d\r",LDC_Read_Reg(0X04));
	//		OLED_ShowNum(22,6,coil_2,4,16);
			printf("X3X%d\r\n",LDC_Read_Reg(0X06));
	//		OLED_ShowNum(86,6,coil_3,4,16);
		}
		moto_control();
		OLED_ShowNum(38,0,counter,6,16);
		OLED_ShowNum(30,2,PWM_M_L,4,16);
		OLED_ShowNum(94,2,PWM_C,4,16);
		OLED_ShowNum(40,4,time/20,4,16);
		OLED_ShowNum(40,6,counter*577/10000,4,16);
		
		t++;
	}
	
}

