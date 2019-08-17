#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "lcd.h"
#include "ov7670.h"		 
#include "sccb.h"	
#include "exti.h"

/************����******************
OV7670��FIFO
OV7670�����Դ��룬�޸ľ�����ԭ�ӿ�������룬
����LCD������ԭ��̽���ߣ���Ҫʹ�����������壬�������Ų���
̽���߿������·ͼ��
��Ϊ�ҵ�����ͷ����ԭ�ӵģ����Բ���ֱ��ʽ��
ov7670���Ų���ov7670.h�е����ţ�sccb��scl��sda�����Ų���
sccb.h 
����ov7670cfg.h�п�������RGB565����YUV422������Ѷ�Ӧ�����
ע�͵����ɡ�
�������ݶ�ȡ��OV7670.c�е�camera_convert_display()�����У�
�Ű��ߺ����ˣ����һ���öŰ��ߵĻ����ź��ߺͰ���������Զ�룬
��ȡ�ɹ���Ҫ�����ߡ������ʾ"OV7670 Error!!"�Ļ�ͨ�����ߵ�����
�����ߺ��ٰ���λ�������߱Ƚ϶��ִ������ء�
**********************************/

int main(void)
{   
	Stm32_Clock_Init(336,8,2,7);//����ʱ��,168Mhz 
	delay_init(168);			//��ʱ��ʼ��  
	uart_init(84,115200);		//��ʼ�����ڲ�����Ϊ115200 
 	LCD_Init();
  OV7670_Init();
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	while(OV7670_Init())//��ʼ��OV7670
	{
		LCD_ShowString(60,250,200,16,16,"OV7670 Error!!");
		delay_ms(200);
	    LCD_Fill(60,230,239,246,WHITE);
		delay_ms(200);
	}
 	LCD_ShowString(60,250,200,16,16,"OV7670 Init OK");
	EXTI9_Init();						//ʹ�ܶ�ʱ������  
  while(1) 
	{		 
		camera_convert_display();//������ʾ
	}
}

