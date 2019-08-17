#include "sys.h"
#include "usart.h"	
#include "stm32f10x_dma.h"
#include "stdlib.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���
//����delay_us,delay_ms
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************/
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

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

uint8_t Uart_Rx[UART_RX_LEN]={0};

void uart_init(u32 bound){
  //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		DMA_InitTypeDef DMA_InitStructure;
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//ʹ��USART1��GPIOAʱ���Լ����ù���ʱ��
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //PA.9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_SetBits(GPIOA,GPIO_Pin_4);

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		
	/*	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure); */
    
    DMA_DeInit(DMA1_Channel5);   //DMA1ͨ��5����     
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  //�����ַ     
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Rx;  //�ڴ��ַ     
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //dma���䷽����     
    DMA_InitStructure.DMA_BufferSize = UART_RX_LEN;   //����DMA�ڴ���ʱ�������ĳ���      
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //����DMA���������ģʽ��������     
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   //����DMA���ڴ����ģʽ      
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���������ֳ�    
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;   //�ڴ������ֳ�     
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //����DMA�Ĵ���ģʽ      
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  //����DMA�����ȼ���
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //����DMA��2��memory�еı����������
    DMA_Init(DMA1_Channel5,&DMA_InitStructure);  
    DMA_Cmd(DMA1_Channel5,ENABLE);//ʹ��ͨ��5 

		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART1, &USART_InitStructure); //��ʼ������

//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
//    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE); 
		USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}

extern u8 Flag_Stop;
extern float balance_kp,balance_kd, velocity_kp ,turn_Kp,turn_Kd;
extern u8 order;
//float balance_kp=500,balance_kd=1.0, velocity_kp=75 ,turn_Kp=42,turn_Kd=0;
extern signed int angle;

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	static u8 Res[20];
	static signed char i=0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)  set_r=atoi(&SET_buf[0]);
	{
		Res[i] = USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
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
