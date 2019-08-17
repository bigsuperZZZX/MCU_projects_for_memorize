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
	GPIO_InitTypeDef GPIO_InitStructure;
//  u16 ADC_Value2[20];	
	char pulse_dist=0;
    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	delay_init();                 //延时初始化
	uart_init(57600);
//	OLED_Init();
	ADC1_Init();
	DHT11_Init();
	IIC_PortInit(); 
  Read_CalibrationData();
  TIM_Init();
	
	while(1)
	{
		if(refresh_flag == 0) continue;
		refresh_flag = 0;                      
    press=Convert_UncompensatedToTrue(Get_BMP180UT(),Get_BMP180UP());
	//  OLED_ShowNum(0,2,press,7,16);
		
		ADC_Value[count-1] = (u16)ADC_ConvertedValue;
		if(count == 20 || count == 40 || count == 60)
		{
			DHT11_ReadData();
	//		OLED_ShowNum(0,0,DHT11.Tem_H,4,16);
	//		OLED_ShowNum(50,0,DHT11.Hum_H,4,16);
			
//			printf("a%db%dc%dd%ds\r\n",DHT11.Tem_H,DHT11.Hum_H,press,pulse_dist);
			
			USART_SendData(USART1,0x55);
			delay_us(500);
			USART_SendData(USART1,0x56);
			delay_us(500);
			USART_SendData(USART1,DHT11.Tem_H);
			delay_us(500);
			USART_SendData(USART1,0x57);
			delay_us(500);
			USART_SendData(USART1,DHT11.Hum_H);
			delay_us(500);
			USART_SendData(USART1,0x58);
			delay_us(500);
			USART_SendData(USART1,press>>8);
			delay_us(500);
			USART_SendData(USART1,press&0xff);
			delay_us(500);
			USART_SendData(USART1,0x59);
			delay_us(500);
			USART_SendData(USART1,pulse_dist);
			
		}
		if(count == 22)
		{
			pulse_dist = get_pulse(ADC_Value);
//			OLED_ShowNum(0,6,pulse_dist,3,16);
		}
		
	}
}

