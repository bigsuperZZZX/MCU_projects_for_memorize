#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "spi.h"
#include "w25qxx.h"
#include "key.h"  


int main(void)
{ 
	u8 key = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);     //初始化延时函数
	SPI1_Init();
	while(1)
	{
		key  = SPI1_ReadWriteByte(key + 1);
		delay_ms(100);
	}	
}

