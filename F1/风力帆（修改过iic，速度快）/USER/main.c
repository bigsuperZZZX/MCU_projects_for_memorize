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
		delay_init();	    	            //=====��ʱ������ʼ��	
		uart_init(128000);	            //=====���ڳ�ʼ��Ϊ
    OLED_Init();                    //=====OLED��ʼ��	
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
	
    IIC_Init();                     //=====IIC��ʼ��
		delay_ms(50);
    MPU6050_initialize();           //=====MPU6050��ʼ��	
    DMP_Init();                     //=====��ʼ��DMP
		delay_ms(50);
		Key_init(); 
		TTP229_init();	        //���̳�ʼ��
	  PB5_EXTI_Init();        //=====MPU6050 5ms��ʱ�жϳ�ʼ��
		Motor_Init();
		while(1)
		{
		}
}

