#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "touch.h" 
#include "ExtBuffer.h" 
#include "sram.h" 
#include "OV7725.h"
#include "dcmi.h" 
#include "w25qxx.h" 

u16 img2[240][240];


int main(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//PA4/6   复用功能输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //复用功能输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化

/******************************************************************************/	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	FSMC_SRAM_Init();  
	delay_init(168);  
	uart_init(256000);		
	
	LED_Init();				
 	LCD_Init();	   
	KEY_Init(); 
	W25QXX_Init();			 
	TP_Init(); //需要提前初始化W25QXX_Init();   触摸屏
	
/*	while(OV7725_init()!=0)
	{
		LCD_ShowString(0,0,200,16,16,"OV7725_InitRetry");
		delay_ms(400);
	}
	LCD_ShowString(0,0,319,32,16,"OV7725_InitOK");
	delay_ms(1000);
	My_DCMI_Init();   //有一个中断
	DCMI_DMA_Init((u32)row,160,DMA_MemoryDataSize_Word); //有一个中断
	DCMI_Start();*/
/************************************************************************/	
	
 	POINT_COLOR=RED;
	delay_ms(1500);	
	
	while(1)
	{
		;
	}
}


