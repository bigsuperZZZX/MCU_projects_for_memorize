#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "stm32f10x.h"
#include "TIM.h"
#include "exti.h"
#include "motor_control.h"

float speed_1,speed_2;    //每秒大轴转过圈数
extern long pulse_1,pulse_2;
extern char clockwise_1,clockwise_2;
extern struct pid
{ 
	short SetPulse;//定义设定值
	short ActualPulse;//定义实际值
	short err; //定义偏差值 
	short err_last;//定义上一个偏差值 
	float Kp,Ki,Kd;//定义比例、积分、微分系数
	short PWM;//定义PWM值（控制执行器的变量）
	short integral;//定义积分值 
}moto_angle_1,moto_angle_2;


 int main(void)
 {
	// short set_angle=180;
	//  char i=1;
	 PID_init();
		delay_init();	    	 //延时函数初始化	  
		OLED_Init();			//初始化OLED 
		TIM3_Init();
		exti_init(); 
	  pwm_init();
	  moto_angle_1.SetPulse=390;
	  moto_angle_2.SetPulse=130;
	  
		while(1)
		{OLED_ShowNum(0,0,pulse_2,10,16);
		OLED_ShowNum(0,2,moto_angle_2.PWM,10,16);
			delay_ms(1000);
			//delay_ms(1000);
		/*	switch(i)
			{
				case 1:OLED_ShowNum(0,0,clockwise_1,10,16);break;
				case 2:OLED_ShowNum(0,2,clockwise_2,10,16);i=0;break;
			}
			i++;
			delay_ms(5000);*/
		}
}

