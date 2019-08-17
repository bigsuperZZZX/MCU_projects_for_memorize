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
//ALIENTEK 探索者STM32F407开发板 实验13
//TFTLCD显示实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
extern u8 ov_sta;	//在exit.c里面定义
extern u8 ov_frame;	//在timer.c里面定义		
 //更新LCD显示
 void camera_refresh(void)
{
	u32 j;
 	u16 color;	 
	if(ov_sta==2)
	{
		LCD_Scan_Dir(U2D_L2R);		//从上到下,从左到右 
		LCD_SetCursor(0x00,0x0000);	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM	
		OV7670_RRST=0;				//开始复位读指针 
		OV7670_RCK=0;
		OV7670_RCK=1;
		OV7670_RCK=0;
		OV7670_RRST=1;				//复位读指针结束 
		OV7670_RCK=1;  
		for(j=0;j<76800;j++)
		{
			OV7670_RCK=0;
			color=GPIOF->IDR&0XFF;	//读数据
			OV7670_RCK=1; 
			color<<=8;  
			OV7670_RCK=0;
			color|=GPIOF->IDR&0XFF;	//读数据
			OV7670_RCK=1; 
			LCD->LCD_RAM=color;  
		}   							 
		EXTI->PR=1<<9;     			//清除LINE9上的中断标志位
		ov_sta=0;					//开始下一次采集
 	//	ov_frame++; 
		LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 
	} 
}	
int main(void)
{   
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200 
//	LED_Init();					//初始化LED
 	LCD_Init();
    OV7670_Init();
	POINT_COLOR=RED;//设置字体为红色 
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
	while(OV7670_Init())//初始化OV7670
	{
		LCD_ShowString(60,230,200,16,16,"OV7670 Error!!");
		delay_ms(200);
	    LCD_Fill(60,230,239,246,WHITE);
		delay_ms(200);
	}
 	LCD_ShowString(60,230,200,16,16,"OV7670 Init OK");
	delay_ms(1500);	 
	OV7670_Light_Mode(0);
//	TIM3_Int_Init(10000-1,8399);			//10Khz计数频率,1秒钟中断		
	EXTI9_Init();						//使能定时器捕获
	OV7670_Window_Set(10,174,240,320);	//设置窗口	  
  	OV7670_CS=0;	
  	while(1) 
	{		 
		camera_refresh();//更新显示
	}
}

