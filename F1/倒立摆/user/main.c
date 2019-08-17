#include "key.h"
#include "pid.h"
#include "public.h"
#include "adc.h"
#include "usart.h"
#include "pwm.h"
#include "time.h"
#include "systick.h"
#include "oled.h"
#include "led.h"
#include "encoder.h"
#include "pid.h"

#define led PAout(8)
#define Moto1 PAout(3)
#define Moto2 PAout(4)


static float ad,angle;
float PWM;
signed int PWM1;
signed int Encoder_Left; 


int main()
{	
    u8 i;

	float ad=0;
	PID_INIT();
	pwm_init();					//TIM2 通道3 pa2,通道4 pa3
    NVIC_Configuration(); 
    usart_init(115200);
	time_init();				//tim3
	LED_Init();
	Encoder_Init_TIM4();		//pb6，pb7
	adc_init();	 				//ADC初始化 pa1
		Moto1=1;
		Moto2=0;
	
	led=0;
	while(1)
	{		
		//ad采样
        ad=0;
		for(i=0;i<2;i++)
		{
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//转换结束标志位
			ad=ad+ADC_GetConversionValue(ADC1);//返回最近一次ADCx规则组的转换结果		
		}
        ad=ad*45/1024;
		angle=ad-179;
		
     }	   
}


void TIM3_IRQHandler()	 //定时器3中断函数
{
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		
	Encoder_Left= (short)TIM4->CNT;
	TIM4->CNT=0;

	if(angle> 70||angle<-70)
	{
		PWM1= 0;
	}	
	else 
	{
		PWM1= Posi_PID(angle,3 );
//		PWM1= Moto_Reflash(PWM1 )+100;
	}
	
	printf("%f\r\n",angle);
	/*	Moto1=1;
		Moto2=0;*/
	if(PWM1 >0)
	{
		Moto1=1;
		Moto2=0;
	}
	else
	{
		Moto1= 0;
		Moto2= 1;
		PWM1 = -PWM1;
	}
	TIM_SetCompare3(TIM2,899-PWM1);
	//printf("\nad=	%0.2f \t encoder=%d\tpwm =%d\t \n",angle,Encoder_Left,PWM1);
}




