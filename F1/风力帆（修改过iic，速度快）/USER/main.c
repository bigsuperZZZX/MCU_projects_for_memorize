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
#include "IOI2C.h"
#include "MPU6050.h"
#include "control.h" 
#include "TTP229.h"

u8 start_flag=0;

int main(void)
{ 
		delay_init();	    	            //=====延时函数初始化	
		uart_init(128000);	            //=====串口初始化为
    OLED_Init();                    //=====OLED初始化	
		delay_ms(10);
		
		OLED_ShowCHinese(0,0,0);
		OLED_ShowCHinese(16,0,1);
		OLED_ShowCHinese(32,0,4);
		OLED_ShowCHinese(48,0,5);
		OLED_ShowChar(64,0,':');
		OLED_ShowCHinese(0,3,2);
		OLED_ShowCHinese(16,3,3);
		OLED_ShowCHinese(32,3,4);
		OLED_ShowCHinese(48,3,5);
		OLED_ShowChar(64,3,':');
	
    IIC_Init();                     //=====IIC初始化
		delay_ms(50);
    MPU6050_initialize();           //=====MPU6050初始化	
    DMP_Init();                     //=====初始化DMP
		delay_ms(50);
		Key_init(); 
		TTP229_init();	        //键盘初始化
	  PB5_EXTI_Init();        //=====MPU6050 5ms定时中断初始化
		Motor_Init();
		while(1)
		{
		}
}

