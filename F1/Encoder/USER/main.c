#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "stm32f10x.h"
#include "TIM.h"
#include "exti.h"
#include "motor_control.h"

float speed_1,speed_2;    //ÿ�����ת��Ȧ��
extern long pulse_1,pulse_2;
extern char clockwise_1,clockwise_2;
extern struct pid
{ 
	short SetPulse;//�����趨ֵ
	short ActualPulse;//����ʵ��ֵ
	short err; //����ƫ��ֵ 
	short err_last;//������һ��ƫ��ֵ 
	float Kp,Ki,Kd;//������������֡�΢��ϵ��
	short PWM;//����PWMֵ������ִ�����ı�����
	short integral;//�������ֵ 
}moto_angle_1,moto_angle_2;


 int main(void)
 {
	// short set_angle=180;
	//  char i=1;
	 PID_init();
		delay_init();	    	 //��ʱ������ʼ��	  
		OLED_Init();			//��ʼ��OLED 
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

