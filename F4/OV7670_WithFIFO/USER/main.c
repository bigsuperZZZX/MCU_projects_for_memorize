#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "lcd.h"
#include "ov7670.h"		 
#include "sccb.h"	
#include "exti.h"

/************备忘******************
OV7670带FIFO
OV7670最精简调试代码，修改精简自原子开发板代码，
其中LCD适用于原子探索者，如要使用其他开发板，具体引脚参照
探索者开发板电路图。
因为我的摄像头不是原子的，所以不用直插式。
ov7670引脚参照ov7670.h中的引脚，sccb（scl，sda）引脚参照
sccb.h 
参照ov7670cfg.h中可以设置RGB565或者YUV422输出，把对应代码块
注释掉即可。
具体数据读取在OV7670.c中的camera_convert_display()函数中，
杜邦线害死人，如果一定用杜邦线的话把信号线和八条数据线远离，
读取成功后不要再碰线。如果显示"OV7670 Error!!"的话通常是线的问题
检查接线后再按复位看看。线比较多又串扰严重。
**********************************/

int main(void)
{   
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200 
 	LCD_Init();
  OV7670_Init();
	POINT_COLOR=RED;//设置字体为红色 
	while(OV7670_Init())//初始化OV7670
	{
		LCD_ShowString(60,250,200,16,16,"OV7670 Error!!");
		delay_ms(200);
	    LCD_Fill(60,230,239,246,WHITE);
		delay_ms(200);
	}
 	LCD_ShowString(60,250,200,16,16,"OV7670 Init OK");
	EXTI9_Init();						//使能定时器捕获  
  while(1) 
	{		 
		camera_convert_display();//更新显示
	}
}

