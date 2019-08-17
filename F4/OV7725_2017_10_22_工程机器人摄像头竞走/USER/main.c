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
#include "T_motor.h"
#include "EasyTracer.h"

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

extern LineResult_def line_result;

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
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //复用功能输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //复用功能输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
}


extern const char action_list_1[][ACTION_BYTES];
extern const char action_list_2[][ACTION_BYTES];
extern const char action_list_3[][ACTION_BYTES];
extern const char action_list_4[][ACTION_BYTES];
extern const char action_list_5[][ACTION_BYTES];
extern const char action_list_6[][ACTION_BYTES];
extern const char action_list_7[][ACTION_BYTES];
extern const char action_list_8[][ACTION_BYTES];
extern uint16_t action_delay_1[100];
extern uint16_t action_delay_2[100];
extern uint16_t action_delay_3[100];
extern uint16_t action_delay_4[100];
extern uint16_t action_delay_5[100];
extern uint16_t action_delay_6[100];
extern uint16_t action_delay_7[100];
extern uint16_t action_delay_8[100];

	
	
int main(void)
{ 
	
//	static char flag_AvoidRepeat = 0;
//	uint16_t receive = 0; 
	uint16_t x,y;
//-**************************************************************************
	Probe_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2 
	delay_init(168);  
	uart_init(115200);	
	
			
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
	DCMI_Start();
//-***********************************************************************
	
 	POINT_COLOR=BLACK;
	GUI_init();
	delay_ms(1500);	
	delay_ms(1500);
	
	action_delay_read(action_delay_1, action_list_1);
	action_delay_read(action_delay_2, action_list_2);
	action_delay_read(action_delay_3, action_list_3);
	action_delay_read(action_delay_4, action_list_4);
	action_delay_read(action_delay_5, action_list_5);
	action_delay_read(action_delay_6, action_list_6);
	action_delay_read(action_delay_7, action_list_7);
	action_delay_read(action_delay_8, action_list_8);
	TIM3_Int_Init(168,50);
	
	while(TP_Read_XY2(&x ,&y)==0);  //按下屏幕，机器人开始运行
	
	action_set(action_delay_1, action_list_1);
	
	while(1)
	{
		switch(line_result.action_command)
		{
			case 3: action_set(action_delay_7, action_list_7);break;
			case 1: action_set(action_delay_5, action_list_5);break;
			case 2: action_set(action_delay_5, action_list_5);break;
			default:action_set(action_delay_7, action_list_7);break;
		}
		switch(line_result.action_command)
		{
			case 3: action_set(action_delay_8, action_list_8);break;
			case 5: action_set(action_delay_6, action_list_6);break;
			case 4: action_set(action_delay_6, action_list_6);break;
			default:action_set(action_delay_8, action_list_8);break;
		}
	}
}
// 	action_command: 1左大转，2左小转，3直走， 4右小转，5右大转
	
//	action_set(action_delay_1, action_list_1);	//直立+迈出半步
//	action_set(action_delay_2, action_list_2);	//完整直走动作
//	action_set(action_delay_3, action_list_3);	//左大转，半步
//	action_set(action_delay_4, action_list_4);	//右大转，半步
//	action_set(action_delay_5, action_list_5);	//左小转，半步
//	action_set(action_delay_6, action_list_6);	//右小转，半步
//	action_set(action_delay_7, action_list_7);	//直走，左半步
//	action_set(action_delay_8, action_list_8);	//直走，右半步
	
	


