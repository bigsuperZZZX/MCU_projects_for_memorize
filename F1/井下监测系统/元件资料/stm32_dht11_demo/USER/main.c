#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "dht11.h"


/*系统初始化*/
void Sys_Init(void)
{
	delay_init();
	uart_init(9600);
	Led_Init();
	DHT11_Init();
}

int main(void)
{	
	Sys_Init();
	
	delay_ms(500);
	delay_ms(500);
	
	while(1)
	{			
		if(DHT11_ReadData())
		{
			printf("read temp :%d.%d hump: %d.%d \n", \
				DHT11_GetTem()/256, DHT11_GetTem()%256,	\
				DHT11_GetHum()/256, DHT11_GetHum()%256);
			
			Led_Set(LED_RED, LED_OFF);
			Led_Set(LED_GREEN, LED_ON);
		}
		else
		{
			printf("read DHT11 failure.\n");
			Led_Set(LED_RED, LED_ON);
			Led_Set(LED_GREEN, LED_OFF);
		}
		delay_ms(500);
		delay_ms(500);
		Led_Set(LED_RED, LED_OFF);
		Led_Set(LED_GREEN, LED_OFF);
		delay_ms(500);
		delay_ms(500);
	}
}

