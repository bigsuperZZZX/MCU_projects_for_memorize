#include "delay.h"
#include "stm32f10x.h"



int main(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��A�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //��ʼ��GPIOD3,6
	 
	delay_init();
  
	while(1)
	{
		GPIO_Write(GPIOA, 0xffff);
		delay_ms(100);
		GPIO_Write(GPIOA, 0x0000);
		delay_ms(100);
	}
}

