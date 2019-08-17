#include "exti.h"
#include "delay.h" 
#include "led.h" 
#include "key.h"
#include "beep.h"
#include "ov7670.h"
#include "lcd.h"

u8 ov_sta;
 //�ⲿ�ж�5~9�������
void EXTI9_5_IRQHandler(void)
{	 		
	if(EXTI->PR&(1<<9))//��9�ߵ��ж�
	{     
		if(ov_sta<2)
		{
			if(ov_sta==0)
			{
				OV7670_WRST=0;	 	//��λдָ��	
				delay_us(1);
				OV7670_WRST=1;	
				OV7670_WREN=1;		//����д��FIFO
			}else OV7670_WREN=0;	//��ֹд��FIFO 	 
			ov_sta++;
		}	 
	}
	EXTI->PR=1<<9;     //���LINE8�ϵ��жϱ�־λ
} 
//�ⲿ�ж�9��ʼ��
void EXTI9_Init(void)
{												  
	Ex_NVIC_Config(GPIO_F,9,RTIR); 			//������ش���			  
	MY_NVIC_Init(0,0,EXTI9_5_IRQn,2);		//��ռ0,�����ȼ�0����2	   
}











