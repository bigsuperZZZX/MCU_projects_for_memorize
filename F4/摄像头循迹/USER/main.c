#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "lcd.h"
#include "ov7670.h"		 
#include "sccb.h"	
#include "exti.h"
#include "stm32f4xx.h"
#include "pwm.h"

extern short cen_x,cen_y;

void init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM14时钟使能    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);              //初始化PF9
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;           //GPIOF9
	GPIO_Init(GPIOB,&GPIO_InitStructure);              //初始化PF9
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	
}

int main(void)
{
	init();
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	pwm_init();
	delay_ms(2000);
	
	TIM_SetCompare1(TIM3,1000);  //使能TIM14在CCR1上的预装载寄存器
	TIM_SetCompare2(TIM3,0);  //使能TIM14在CCR1上的预装载寄存器
	TIM_SetCompare3(TIM3,1000);  //使能TIM14在CCR1上的预装载寄存器
	TIM_SetCompare4(TIM3,0);  //使能TIM14在CCR1上的预装载寄存器
	uart_init(84,115200);		//初始化串口波特率为115200 
 	LCD_Init();
  OV7670_Init();
	POINT_COLOR=RED;//设置字体为红色 
	while(OV7670_Init())//初始化OV7670
	{
		LCD_ShowString(60,250,200,16,16,"OV7670 Error!!");
		delay_ms(200);
	  LCD_Fill(60,230,239,246,WHITE);
	}
 	LCD_ShowString(60,250,200,16,16,"OV7670 Init OK");
	EXTI9_Init();						//使能定时器捕获  
	
	
  while(1) 
	{		 
		camera_convert_display();//更新显示
		
	}
}

