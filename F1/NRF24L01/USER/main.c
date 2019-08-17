#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "nrf24l01.h"
#include "spi.h"
#include "usart.h"

//CE-PB10 CSN-PB12 CLK-PB13 MOSI-PB15 MISO-PB14 IRQ-PB9


int main(void)
{
	u8 i;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	delay_init();	    	 //延时函数初始化	  
	//OLED_Init();			//初始化OLED 
	uart_init(115200);
	SPI2_Config();
	nrf24l01_init();
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //PC13调试接口不能直接用
	
	while(NRF24L01_Check()==1);
	
	for(i=0;i<10;i++)
		PAout(0)=~PAout(0);
	
	NRF24L01_RX_Mode();
	while(1);
}

