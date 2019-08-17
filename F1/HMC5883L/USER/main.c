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

int main(void)
{ 
	int i=0,dir_flag=0;
	
	delay_init();                 //延时初始化
	uart_init(115200);   //用到了定时器3的50ms中断
	sys_pulse();

	while(1)
	{
		//transport_sendPacketBuffer(123,"ABCDEF",7);
		//delay_ms(5000);
		if(USART1_RcvFlag == 1)
		{
			USART1_RcvFlag = 0;
			printf("%s",USART1_DataBuf);
		}	
	}
}

