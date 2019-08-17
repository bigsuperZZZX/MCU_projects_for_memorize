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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);     //��ʼ����ʱ����
	SPI1_Init();
	while(1)
	{
		key  = SPI1_ReadWriteByte(key + 1);
		delay_ms(100);
	}	
}

