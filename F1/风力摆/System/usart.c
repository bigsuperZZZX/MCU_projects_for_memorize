#include "usart.h"
#include "stdio.h"

/*printf�ض���*/
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 

void usart1_init(void) 
{
	GPIO_InitTypeDef     GPIO_InitStructure;  
	USART_InitTypeDef    USART_InitStructure;  
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//�򿪴��ڸ���ʱ��
	USART_DeInit(USART1);  //��λ����1

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			   //���ͽ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);				

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			   //���ս�
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  

	USART_InitStructure.USART_BaudRate             = 115200;					
	USART_InitStructure.USART_WordLength           = USART_WordLength_8b;		
	USART_InitStructure.USART_StopBits             = USART_StopBits_1;		
	USART_InitStructure.USART_Parity               = USART_Parity_No;		
	USART_InitStructure.USART_HardwareFlowControl  = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode                 = USART_Mode_Rx | USART_Mode_Tx;	 //�����뷢�Ͷ�ʹ��
	USART_Init(USART1, &USART_InitStructure);								

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // ʹ��ָ����USART1�����ж� ��

	USART_Cmd(USART1, ENABLE);							   //ʹ�� USART1

  USART_ClearFlag(USART1, USART_FLAG_TC);                //�崮��1���ͱ�־
}


void usart3_init(void)   //MPU6050
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	USART_InitTypeDef USART_InitStructure;	 
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//�򿪴��ڸ���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
  	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				  //�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);    				
														  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				  //�����
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);                 
	  
	//����USART3����
	USART_InitStructure.USART_BaudRate = 115200;	                  
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	   
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	     
	USART_InitStructure.USART_Parity = USART_Parity_No ;           
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;									//����ʹ��
	USART_Init(USART3, &USART_InitStructure);                      
	
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // ʹ��ָ����USART3�����ж�
	
	USART_Cmd(USART3, ENABLE);                             // USART3ʹ��
}

/*�ж����ȼ�����*/
void NVIC_init(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure; 					   

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);       //�ж����ȼ�����  ��ռʽ���ȼ�������Ϊ2λ����Ӧ���ȼ�ռ2λ	

	//����3�ж����ȼ�����,MPU6050
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			   //ָ���ж�Դ
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	   //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //ָ����Ӧ���ȼ���1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	             
	NVIC_Init(&NVIC_InitStructure);							     
	
		//����1�ж����ȼ�����,USB
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;              //ͨ������Ϊ����1�ж�  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	   //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //ָ����Ӧ���ȼ���1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	            
	NVIC_Init(&NVIC_InitStructure);							     
}

