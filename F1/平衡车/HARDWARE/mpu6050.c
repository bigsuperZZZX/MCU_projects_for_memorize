#include "mpu6050.h"
#include "stm32f10x.h"

float cur_x,cur_x_v;

void usart3_init(void)   //MPU6050
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	USART_InitTypeDef USART_InitStructure;	 
	NVIC_InitTypeDef    NVIC_InitStructure; 	
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//�򿪴��ڸ���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
														  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				  //�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
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

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);       //�ж����ȼ�����  ��ռʽ���ȼ�������Ϊ2λ����Ӧ���ȼ�ռ2λ	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			   //ָ���ж�Դ
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	   //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //ָ����Ӧ���ȼ���1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	             
	NVIC_Init(&NVIC_InitStructure);
}


void USART3_IRQHandler(void)
{
	static unsigned char counter=0;
	static unsigned char BUFF[4];
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж���Ч,���������ݼĴ�����
  {
    BUFF[counter] = USART_ReceiveData(USART3);   //��������
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

