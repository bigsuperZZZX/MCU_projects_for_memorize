#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "lcd.h"
#include "ov7670.h"
//#include "ov7670cfg.h"
#include "timer.h"	  		 
#include "sccb.h"	
#include "exti.h"
//ALIENTEK ̽����STM32F407������ ʵ��13
//TFTLCD��ʾʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
extern u8 ov_sta;	//��exit.c���涨��
extern u8 ov_frame;	//��timer.c���涨��		
 //����LCD��ʾ
 void camera_refresh(void)
{
	u32 j;
 	u16 color;	 
	if(ov_sta==2)
	{
		LCD_Scan_Dir(U2D_L2R);		//���ϵ���,������ 
		LCD_SetCursor(0x00,0x0000);	//���ù��λ�� 
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
		OV7670_RRST=0;				//��ʼ��λ��ָ�� 
		OV7670_RCK=0;
		OV7670_RCK=1;
		OV7670_RCK=0;
		OV7670_RRST=1;				//��λ��ָ����� 
		OV7670_RCK=1;  
		for(j=0;j<76800;j++)
		{
			OV7670_RCK=0;
			color=GPIOF->IDR&0XFF;	//������
			OV7670_RCK=1; 
			color<<=8;  
			OV7670_RCK=0;
			color|=GPIOF->IDR&0XFF;	//������
			OV7670_RCK=1; 
			LCD->LCD_RAM=color;  
		}   							 
		EXTI->PR=1<<9;     			//���LINE9�ϵ��жϱ�־λ
		ov_sta=0;					//��ʼ��һ�βɼ�
 	//	ov_frame++; 
		LCD_Scan_Dir(DFT_SCAN_DIR);	//�ָ�Ĭ��ɨ�跽�� 
	} 
}	
int main(void)
{   
	Stm32_Clock_Init(336,8,2,7);//����ʱ��,168Mhz 
	delay_init(168);			//��ʱ��ʼ��  
	uart_init(84,115200);		//��ʼ�����ڲ�����Ϊ115200 
//	LED_Init();					//��ʼ��LED
 	LCD_Init();
    OV7670_Init();
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(60,70,200,16,16,"OV7670 TEST");	
	LCD_ShowString(60,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,200,16,16,"2012/9/14");  
	LCD_ShowString(60,130,200,16,16,"KEY0:Light Mode");
	LCD_ShowString(60,150,200,16,16,"KEY1:Saturation");
	LCD_ShowString(60,170,200,16,16,"KEY2:Brightness");
	LCD_ShowString(60,190,200,16,16,"KEY_UP:Contrast");
	LCD_ShowString(60,210,200,16,16,"TPAD:Effects");	 
  	LCD_ShowString(60,230,200,16,16,"OV7670 Init...");	  
	while(OV7670_Init())//��ʼ��OV7670
	{
		LCD_ShowString(60,230,200,16,16,"OV7670 Error!!");
		delay_ms(200);
	    LCD_Fill(60,230,239,246,WHITE);
		delay_ms(200);
	}
 	LCD_ShowString(60,230,200,16,16,"OV7670 Init OK");
	delay_ms(1500);	 
	OV7670_Light_Mode(0);
//	TIM3_Int_Init(10000-1,8399);			//10Khz����Ƶ��,1�����ж�		
	EXTI9_Init();						//ʹ�ܶ�ʱ������
	OV7670_Window_Set(10,174,240,320);	//���ô���	  
  	OV7670_CS=0;	
  	while(1) 
	{		 
		camera_refresh();//������ʾ
	}
}

