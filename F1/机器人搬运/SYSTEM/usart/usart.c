#include "sys.h"
#include "usart.h"	
#include "stm32f10x_dma.h"
#include "stdlib.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//使用SysTick的普通计数模式对延迟进行管理
//包括delay_us,delay_ms
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************/
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif

uint8_t Uart_Rx[UART_RX_LEN]={0};

void uart_init(u32 bound){
  //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		DMA_InitTypeDef DMA_InitStructure;
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//使能USART1，GPIOA时钟以及复用功能时钟
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //PA.9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_SetBits(GPIOA,GPIO_Pin_4);

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		
	/*	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure); */
    
    DMA_DeInit(DMA1_Channel5);   //DMA1通道5配置     
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  //外设地址     
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Rx;  //内存地址     
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //dma传输方向单向     
    DMA_InitStructure.DMA_BufferSize = UART_RX_LEN;   //设置DMA在传输时缓冲区的长度      
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //设置DMA的外设递增模式，不递增     
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   //设置DMA的内存递增模式      
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //外设数据字长    
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;   //内存数据字长     
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //设置DMA的传输模式      
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  //设置DMA的优先级别
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //设置DMA的2个memory中的变量互相访问
    DMA_Init(DMA1_Channel5,&DMA_InitStructure);  
    DMA_Cmd(DMA1_Channel5,ENABLE);//使能通道5 

		USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART1, &USART_InitStructure); //初始化串口

//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
//    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE); 
		USART_Cmd(USART1, ENABLE);                    //使能串口 

}

extern u8 Flag_Stop;
extern float balance_kp,balance_kd, velocity_kp ,turn_Kp,turn_Kd;
extern u8 order;
//float balance_kp=500,balance_kd=1.0, velocity_kp=75 ,turn_Kp=42,turn_Kd=0;
extern signed int angle;

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	static u8 Res[20];
	static signed char i=0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)  set_r=atoi(&SET_buf[0]);
	{
		Res[i] = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		if(Res[i]=='s')
		{
			Res[i]='\0';
			if(Res[0]=='a') angle=(atoi(&Res[1]));
			i=-1;
		}
		i++;
  } 
	
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {}
//		order = (uint16_t)(USART1->DR & (uint16_t)0x01FF);
		
}
