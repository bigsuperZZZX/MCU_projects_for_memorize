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
#include "ds18b20.h"	
#include "BMP180.h"
#include "pulse.h"
#include "dht11.h"

unsigned char i,j;          //计数变量
u16 temperature;
int press;      //实际气压,单位:Pa
extern uint16_t ADC_ConvertedValue;
extern char count,refresh_flag;
u16 ADC_Value[60];
extern DHT11_TypeDef DHT11;	
char count2;

int main(void)
{ 
	char i;
	delay_init();                 //延时初始化
	
	uart_init(57600);

	while(1)
	{
			USART_SendData(USART1,0x55);
			delay_us(500);
			USART_SendData(USART1,0x56);
			delay_us(500);
			USART_SendData(USART1,20+i);
			delay_us(500);
			USART_SendData(USART1,0x57);
			delay_us(500);
			USART_SendData(USART1,30+i);
			delay_us(500);
			USART_SendData(USART1,0x58);
			delay_us(500);
			USART_SendData(USART1,12345>>8);
			delay_us(500);
			USART_SendData(USART1,12345&0xff);
			delay_us(500);
			USART_SendData(USART1,0x59);
			delay_us(500);
			USART_SendData(USART1,50+i*5);
		
		i++;
		if(i==10) i=0;
		delay_ms(995);
	}
}

