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
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM14ʱ��ʹ��    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);              //��ʼ��PF9
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;           //GPIOF9
	GPIO_Init(GPIOB,&GPIO_InitStructure);              //��ʼ��PF9
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	
}

int main(void)
{
	init();
	Stm32_Clock_Init(336,8,2,7);//����ʱ��,168Mhz 
	delay_init(168);			//��ʱ��ʼ��  
	pwm_init();
	delay_ms(2000);
	
	TIM_SetCompare1(TIM3,1000);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_SetCompare2(TIM3,0);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_SetCompare3(TIM3,1000);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_SetCompare4(TIM3,0);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���
	uart_init(84,115200);		//��ʼ�����ڲ�����Ϊ115200 
 	LCD_Init();
  OV7670_Init();
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	while(OV7670_Init())//��ʼ��OV7670
	{
		LCD_ShowString(60,250,200,16,16,"OV7670 Error!!");
		delay_ms(200);
	  LCD_Fill(60,230,239,246,WHITE);
	}
 	LCD_ShowString(60,250,200,16,16,"OV7670 Init OK");
	EXTI9_Init();						//ʹ�ܶ�ʱ������  
	
	
  while(1) 
	{		 
		camera_convert_display();//������ʾ
		
	}
}

