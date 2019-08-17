#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "touch.h" 
#include "ExtBuffer.h" 
#include "sram.h" 
#include "OV7725.h"
#include "dcmi.h" 
#include "w25qxx.h" 
#include "GUI.h" 
#include "motor.h"
#include "mofang.h"

/*u16 row[320];*/
//u16 img[240*240];
/*u16 img[240*240] __attribute__((at(0X68000000)));
u8 buffer[50]   __attribute__((at(0X6801c204)));*/
//u16 temp[100];
extern u16 img[240*240];
extern u16 *img_p;
extern u16 row[320];
extern char flag_color_complete;
extern short Restore_step[200];

void Probe_init(void) ;


/*int main2()
{
	//RestoreCube();
	int i = 0 ,j=0;
	Probe_init();
	motor_Init();
	delay_init(168);  
	
	Restore_step[0]=11;
	Restore_step[1]=11;
	Restore_step[2]=-11;
	Restore_step[3]=-11;
	Restore_step[4]=11;
	Restore_step[5]=11;
	Restore_step[6]=-11;
	Restore_step[7]=-11;
	Restore_step[8]='#';
	
	while(1)
	{
			PFout(0) = 1;
			for(j=0;j<400;j++)
			{
				PFout(1) = 1;
				delay_us2(1000);
				PFout(1) = 0;
				delay_us2(1000);
			}
	}
	
	
	
	while(1);
}*/

void Probe_init(void) // PC4
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //复用功能输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
}

//采集颜色的时候123456分别是前左后右上下
//求解所得的动作表中123456分别是前后左右上下，正数代表顺时针，1转90、2转180

int main(void)
{ 
	
	static char flag_AvoidRepeat = 0;

	
//	RestoreCube(); ///////////////////////////////////////////////////////
	
	
	
//-**************************************************************************
	Probe_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2 
	delay_init(168);  
	uart_init(115200);
	motor_pin_Init();
			
//	goto short_cut;
	
 	LCD_Init();	 			 
	TP_Init(); //需要提前初始化W25QXX_Init();   触摸屏
	
	while(OV7725_init()!=0)
	{
		LCD_ShowString(0,0,200,16,16,"OV7725_InitRetry",0);
		delay_ms(400);
	}
	LCD_ShowString(0,0,319,32,16,"OV7725_InitOK     ",0);
	delay_ms(1000);
	My_DCMI_Init();   //有一个中断
	DCMI_DMA_Init((u32)row,160,DMA_MemoryDataSize_Word); //有一个中断
	
	start:flag_AvoidRepeat = 0;
	DCMI_Start();
//-***********************************************************************
	
 	POINT_COLOR=BLACK;
	GUI_init();
	delay_ms(1500);	
	
	while(1)
	{
		if(flag_color_complete==1)
		{
			if(flag_AvoidRepeat == 0)
		  {
				DCMI_Stop_All();
				RestoreCube();
				flag_AvoidRepeat = 1;
				motor_tim_Init();
				while(key_refresh()!=6);
			}
			short_cut:
			motor_action();
//			goto short_cut;
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			flag_color_complete = 0;
			goto start;
		}
	}
}


