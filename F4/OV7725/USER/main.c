#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "OV7725.h"
#include "dcmi.h" 
#include "imgprcs.h"

extern u8 row[640];

int main(void)
{ 
	delay_init(168);		//延时初始化 
	uart_init(256000);	//串口初始化波特率为115200
	uart2_init(256000);

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOA时钟
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); 
			
	PCout(4)=1;
	
	devition_init();
	
	while(OV7725_init()!=0)
	{
		printf("OV7725_InitRetry\r\n");
		delay_ms(400);
	}
	printf("OV7725_InitOk\r\n");
	delay_ms(1800);
	
	My_DCMI_Init();
	DCMI_DMA_Init((u32)row,160,DMA_MemoryDataSize_Word);
	DCMI_Start();
	while(1)
	{

	}
}

