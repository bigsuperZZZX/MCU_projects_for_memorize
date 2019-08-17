#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "usart.h"	
#include "encoder.h"
#include "encoder.h"
#include "moto.h"

u16 dist1_mm,dist2_mm;
u16 dist1_play,dist2_play;
u16 sensor;
extern long pulse_r,pulse_l;
extern short pwm_r,pwm_l;
u8 flag_stark=0;   //右边卡1,左边卡2
u8 flag_line=0;    //0表示还没到黑线，1表示到了黑线了

int main(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	delay_init();                 //延时初始化
	OLED_Init();
	uart1_init(9600);
	uart2_init(9600);
	encoder_ExtiInit();
	moto_init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOG, &GPIO_InitStructure);	 	
	
	delay_ms(3000);
	
//	TIM_SetCompare1(TIM2,1000);  //右边
//	TIM_SetCompare3(TIM2,2000);  //左边
	
	while(1)
	{
		sensor = GPIO_ReadInputData(GPIOG);  //光电管没接收到障碍输出高
		if((GPIO_ReadInputData(GPIOG)&0x0002)==0)
		{
			delay_us(500);
			if((GPIO_ReadInputData(GPIOG)&0x0002)==0)
			{
				flag_line=1;
				TIM_SetCompare1(TIM2,0);
				TIM_SetCompare2(TIM2,0);
				TIM_SetCompare3(TIM2,1500);
				TIM_SetCompare4(TIM2,0);
				while((GPIO_ReadInputData(GPIOG)&0x0008) != 0);
			}
			
		}
		if(flag_line==0)
		{
			if(((sensor&0x0038)==0x0038) || ((sensor&0x0038)==0x0010))
			{
				TIM_SetCompare1(TIM2,2000);
				TIM_SetCompare3(TIM2,2100);
				TIM_SetCompare4(TIM2,0);
				OLED_ShowString(0,0,"straight");
			}
			else if(((sensor&(0x0001<<4))== 0) && ((sensor & (0x0001<<5))!=0)) 
			{
				TIM_SetCompare1(TIM2,1600);
				TIM_SetCompare3(TIM2,0);
				TIM_SetCompare4(TIM2,1000);
				OLED_ShowString(0,0,"left");
			}
			else
			{
				if((sensor & (0x0001<<3)) == 0)
				{
					TIM_SetCompare1(TIM2,2000);
					TIM_SetCompare3(TIM2,1400+100);
					TIM_SetCompare4(TIM2,0);
				OLED_ShowString(0,0,"straight");
				}
				if((sensor&(0x0001<<5)) == 0)
				{
					TIM_SetCompare1(TIM2,1400);
					TIM_SetCompare3(TIM2,2000+200);
					TIM_SetCompare4(TIM2,0);
					OLED_ShowString(0,0,"straight");
				}
			}
			delay_ms(1);
		}
		else
		{
			if(((sensor&0x0038)==0x0038) || ((sensor&0x0038)==0x0010))
			{
				TIM_SetCompare1(TIM2,2000);
				TIM_SetCompare2(TIM2,0);
				TIM_SetCompare3(TIM2,2100);
				TIM_SetCompare4(TIM2,0);
				OLED_ShowString(0,0,"straight");
			}
			else if(((sensor&(0x0001<<4))== 0) && ((sensor & (0x0001<<3))!=0)) 
			{
				TIM_SetCompare1(TIM2,0);
				TIM_SetCompare2(TIM2,600);
				TIM_SetCompare3(TIM2,1700);
				TIM_SetCompare4(TIM2,0);
				OLED_ShowString(0,0,"right");
			}
			else
			{
				if((sensor & (0x0001<<3)) == 0)
				{
					TIM_SetCompare1(TIM2,2000);
					TIM_SetCompare2(TIM2,0);
					TIM_SetCompare3(TIM2,1400+100);
					TIM_SetCompare4(TIM2,0);
					OLED_ShowString(0,0,"straight");
				}
				if((sensor&(0x0001<<5)) == 0)
				{
					TIM_SetCompare1(TIM2,1400);
					TIM_SetCompare2(TIM2,0);
					TIM_SetCompare3(TIM2,2000+200);
					TIM_SetCompare4(TIM2,0);
					OLED_ShowString(0,0,"straight");
				}
			}
		}
	}
	
	/*while(1)
	{
		sensor = GPIO_ReadInputData(GPIOG);  //光电管没接收到障碍输出高
		if((sensor&0x0038)==0x0038)
		{
			TIM_SetCompare1(TIM2,1500);
			TIM_SetCompare3(TIM2,1600);
				TIM_SetCompare4(TIM2,0);
		}
		else if(((sensor&(0x0001<<4))== 0) && ((sensor & (0x0001<<5))!=0)) 
		{
			TIM_SetCompare1(TIM2,1800);
			TIM_SetCompare3(TIM2,0);
			TIM_SetCompare4(TIM2,1400);
		}
		else
		{
			if((sensor & (0x0001<<3)) == 0)
			{
				TIM_SetCompare1(TIM2,1500);
				TIM_SetCompare3(TIM2,1000+100);
				TIM_SetCompare4(TIM2,0);
			}
			if((sensor&(0x0001<<5)) == 0)
			{
				TIM_SetCompare1(TIM2,1400);
				TIM_SetCompare3(TIM2,2000+200);
				TIM_SetCompare4(TIM2,0);
			}
		}
		delay_ms(2);
	}*/
	
	/*gostreight();
	
	pwm_r=2000,pwm_l=2000;
	
	while(1)
	{
		turn_left();
		gostreight();
		delay_ms(1000);
		turn_right();
		gostreight();
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
	}		*/
/*	
	while(1)
	{
		sensor = GPIO_ReadInputData(GPIOG);
		if(sensor & 0x0001)
			OLED_ShowChar(110,0,'1');
		else
			OLED_ShowChar(110,0,'0');
		if(sensor & 0x0002)
			OLED_ShowChar(100,0,'1');
		else
			OLED_ShowChar(100,0,'0');
		if(sensor & 0x0004)
			OLED_ShowChar(90,0,'1');
		else
			OLED_ShowChar(90,0,'0');
		if(sensor & 0x0008)
			OLED_ShowChar(100,2,'1');
		else
			OLED_ShowChar(100,2,'0');
		
		
		OLED_ShowNum(0,4,pulse_l,4,16);
		OLED_ShowNum(64,4,pulse_r,4,16);
		
		OLED_ShowNum(0,6,pwm_l,4,16);
		OLED_ShowNum(64,6,pwm_r,4,16);
		
		if(dist1_play!=0)
			OLED_ShowNum(0,0,dist1_play,4,16);
		if(dist2_play!=0)
			OLED_ShowNum(0,2,dist2_play,4,16);
		dist1_play=0;
		dist2_play=0;
		
		delay_ms(10);
	}*/
}

