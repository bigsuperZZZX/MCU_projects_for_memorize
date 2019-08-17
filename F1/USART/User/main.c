#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "oled.h"
#include "bmp.h"

int main()
{	
	usart1_init(); 
	NVIC_init();
		delay_init();	    	 //延时函数初始化	  
		OLED_Init();			//初始化OLED  
	OLED_ShowChar(10,10,'A');
	while(1)
	{
		USART_SendData(USART1,0x55);
		delay_ms(1000);
	}
}


