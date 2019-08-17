#include "mpu6050.h"
#include "stm32f10x.h"

float cur_x,cur_x_v;

void usart3_init(void)   //MPU6050
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	USART_InitTypeDef USART_InitStructure;	 
	NVIC_InitTypeDef    NVIC_InitStructure; 	
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//打开串口复用时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
														  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				  //输入脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);                 
	  
	//配置USART3参数
	USART_InitStructure.USART_BaudRate = 115200;	                  
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	   
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	     
	USART_InitStructure.USART_Parity = USART_Parity_No ;           
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;									//接收使能
	USART_Init(USART3, &USART_InitStructure);                      
	
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 使能指定的USART3接收中断				
	
	USART_Cmd(USART3, ENABLE);                             // USART3使能   

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);       //中断优先级分组  抢占式优先级别设置为2位；响应优先级占2位	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			   //指定中断源
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	   //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //指定响应优先级别1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	             
	NVIC_Init(&NVIC_InitStructure);
}


void USART3_IRQHandler(void)
{
	static unsigned char counter=0;
	static unsigned char BUFF[4];
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断有效,若接收数据寄存器满
  {
    BUFF[counter] = USART_ReceiveData(USART3);   //接收数据
		if(BUFF[0] == 0x55)
			counter++;
		else
			counter=0;
		if(counter==4)
		{
			counter=0;
			if(BUFF[1]==0x52)
				cur_x_v = -((short)(BUFF[3]<<8| BUFF[2]))/32768.0*2000-2.95;
			if(BUFF[1]==0x53)
				cur_x = ((short)(BUFF[3]<<8| BUFF[2]))/32768.0*180/*-offset_y*/;
		}
	}
}

