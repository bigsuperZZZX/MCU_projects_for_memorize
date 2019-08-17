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
#include "stm32f10x_tim.h"

unsigned char i,j;          //计数变量

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	int rc = 0;
	int i;
	printf("AT+CIPSEND\r\n");
	while((USART1->DR & (uint16_t)0x01FF)!='>');
	for(i=0;i<buflen;i++) 
	{
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) *(buf+i);  
	}
	while((USART1->SR&0X40)==0);
	USART1->DR = 0x1a;
	return rc;
}

extern u8 USART1_RcvFlag;
extern u8 USART1_DataBuf[200];
extern u8 source_celect;
extern u8 ch7_state;
	
int main(void)
{ 
	 GPIO_InitTypeDef GPIO_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_15; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	delay_init();                 //延时初始化
	uart_init(115200);   //用到了定时器3的50ms中断
	sys_pulse();

	while(1)
	{
		if(source_celect ==1 )
		{
			PCout(13) = 0;
			PBout(0) = PBin(7);   //RS
			PBout(1) = PBin(8);
			PBout(5) = PAin(5);
			PBout(6) = PBin(10);
		}
		else
		{
			PCout(13) = 1;
			PBout(0) = PBin(11);  //遥控
			PBout(1) = PBin(12);
			PBout(5) = PBin(13);
			PBout(6) = PBin(14);
		}
		if(ch7_state == 1)
		{
			PAout(0) = 1;
		}
		else
		{
			PAout(0) = 0;
		}
	}
}

