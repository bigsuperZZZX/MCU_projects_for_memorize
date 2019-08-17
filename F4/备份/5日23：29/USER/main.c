#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "OV7670.h"
#include "dcmi.h" 
#include "imgprcs.h"

extern u8 row[640];

int main(void)
{ 
	delay_init(168);		//��ʱ��ʼ�� 
	uart_init(256000);	//���ڳ�ʼ��������Ϊ115200

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOAʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOC,&GPIO_InitStructure); 
			
	PCout(4)=1;
	
	while(OV7670_init()!=0)
	{
		printf("OV7670_InitRetry\r\n");
		delay_ms(400);
	}
	printf("OV7670_InitOk\r\n");
	delay_ms(1800);
	
	My_DCMI_Init();
	DCMI_DMA_Init((u32)row,160,DMA_MemoryDataSize_Word);
	DCMI_Start();
	while(1)
	{
//		data_process2();    //����������ɫɫ�ʷ�Χhe���������ɫ��Ի����λ�ú����
	}
}

