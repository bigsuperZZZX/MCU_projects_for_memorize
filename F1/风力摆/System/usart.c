#include "usart.h"
#include "stdio.h"

/*printf重定向*/
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
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

void usart1_init(void) 
{
	GPIO_InitTypeDef     GPIO_InitStructure;  
	USART_InitTypeDef    USART_InitStructure;  
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//打开串口复用时钟
	USART_DeInit(USART1);  //复位串口1

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			   //发送脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);				

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			   //接收脚
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  

	USART_InitStructure.USART_BaudRate             = 115200;					
	USART_InitStructure.USART_WordLength           = USART_WordLength_8b;		
	USART_InitStructure.USART_StopBits             = USART_StopBits_1;		
	USART_InitStructure.USART_Parity               = USART_Parity_No;		
	USART_InitStructure.USART_HardwareFlowControl  = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode                 = USART_Mode_Rx | USART_Mode_Tx;	 //接收与发送都使能
	USART_Init(USART1, &USART_InitStructure);								

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能指定的USART1接收中断 ；

	USART_Cmd(USART1, ENABLE);							   //使能 USART1

  USART_ClearFlag(USART1, USART_FLAG_TC);                //清串口1发送标志
}


void usart3_init(void)   //MPU6050
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	USART_InitTypeDef USART_InitStructure;	 
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//打开串口复用时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
  	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				  //输出脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);    				
														  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				  //输入脚
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
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
}

/*中断优先级配置*/
void NVIC_init(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure; 					   

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);       //中断优先级分组  抢占式优先级别设置为2位；响应优先级占2位	

	//串口3中断优先级设置,MPU6050
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			   //指定中断源
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	   //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //指定响应优先级别1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	             
	NVIC_Init(&NVIC_InitStructure);							     
	
		//串口1中断优先级设置,USB
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;              //通道设置为串口1中断  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	   //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //指定响应优先级别1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	            
	NVIC_Init(&NVIC_InitStructure);							     
}

