
/*****��������¼***********************************
		��λ�Ƕ�ȡ�����ڴ���mpu6050����������100/s
		�������ȫ������PID���ƣ����������޸ģ�Ϊ��ȡ�ø��õĶ�λЧ����
ȡ���˱��������ȡ����֮������ǰ������ÿ��0.5��1�����ȶ��ķ�������PWMֵ��
���������Ϊ��������������������pwmֵ+�������+΢���������ȡ���˱꽺�õĶ�λЧ����
		���ߣ���Բ��д���ֵȹ���ȫ�����ڶ�����ƣ�
		������Ҫ����Ҫ��ȫ�ֱ���������ǰx���꣬��ǰy���꣬�趨x���꣬�趨y���꣬�趨��ǰģʽ
		Ϊ���Է��㣬������������ȫ��ͨ�������������ɣ�δ������ʾ���밴��
		����δ�������������Լ������б����ڵ�һ�ζ����ʱ���ں������ע�⡣ͬʱ����������кܴ���Ż��Ľ��ռ䣬
�����жϺ���.c�ļ��е�һЩflag(sign)֮ǰ�����ˣ����Ǻ����ƺ�ûʲô���ˡ�
		��һ��дһ�������Ĺ��̣����Ժܶຯ����λ�÷�������˺ܴ�����⣬��֪���÷ŵ��ĸ�c�ļ����
		���ھ�����ƣ��������ȵ�6050ģ����ʱԤ�ȣ���ֵ�ȶ�������Ȼ������  j  ���й��㡣�������ģʽ���Ž����ݣ�
ϵͳ�㿪ʼ��������ģʽ���������ݸ�ʽ���£�
һ.���㣺|| p12045 || ���У�pΪpoint��12Ϊ�������°뾶ֵ��045Ϊ�������½Ƕ�ֵ��
��.���ߣ�|| l15045 || ���У�lΪline��15��045����ͬ�ϣ�
��.��Բ��|| c15 || ���У�cΪcircle��15Ϊ�뾶��  ����Բ����������Ϊ����뾶��1.15�������о��˵Ļ�Ҫ��΢Сһ�㣩
��.д���֣�|| w5 || ���У�wΪwrite��5ΪҪд�����֣������������ֻ��д��0��7�������룻
��.pidϵ�����ߵ��ڣ�|| +10520 || �� || -10520 || ���У�+��-��ʾ��ǰֵ���ϻ��ȥ�������ݣ�
	�������ݷ�Ϊ�����֣�10����kp����λΪ1��5����ki����λΪ0.01��20����kd����λΪ1
��.����pwm�������ƶ�������ԣ�|| s1560 || ���У�sΪset��1560��������pwmֵ1560�����3000��
		���Ƶ�����ܹ���ſ�������Ϊ��ÿ��ȡһ��6050��̬���ݣ�0.01s����������һ����̬��pid������ƣ�ÿ��100�Σ���
���еĿ��ƶ����жϺ�������ɣ�������·�ǣ�ÿ����0.01sͨ��switch������Ӧģʽ��Ȼ�����һ�ζ���pid���㲢�����
pwmֱֵ�Ӵ��ݸ����ط��ȣ��ڼ�ÿ��һ�����루ʵ�ֵķ�ʽ��ͨ����һ��������¼������뵱ǰ�жϴ�����ÿ��һ�ζ��Ǻ�
��ȷ��0.01s�����Լ��ʱ������ɽ��������ȷ������ͨ��һ����ChangePoint�������ı�set_x��set_y����Ϊ��ȫ�ֱ�������
�Ըı����趨ֵ���Ͽ�����pid�����в���Ч����
		֮ǰ�ڽ���ģ�͵�ʱ�����˺ܺõ�һ�㣬��������ʮ���η��ã���ôx�����y������Ա��������ƣ����еĶ�άƽ��
�˶�����һ����ת��Ϊһά�ˣ�����ֻ��������pid��ͬʱ���������İ�������ÿ������ֻҪ���������ͺ��ˣ�֮���Բ���
��ȫ������Ҫ��������һ������ֻḴ�Ӻܶ࣬��Ȼ֮���Բ���oled��Ļ�ͼ��̣���ҪҲ����Ϊ�ٽ���ĩʱ��������ԾͲ�Ū
�ˡ�		
		�����ʱ��һ��Ѿ���ȫ��������һ���ַ���ת���ֵĺ�����û�й�ϵ�����ò�
******************2016��6��16��***********************/




#include "stm32f10x.h"
#include "pwm.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include "mpu6050.h"
#include "moto.h"
#include "control.h"

//#define H (50)  �ڵ�� �궨����control.h��

/**********************///mpu6050���ݶ�ȡ���ж�
extern float cur_x,cur_y,cur_z;
/**********************///���Է��Ϳ����ж�
extern int set_a,set_r;
extern u8 set_mode;
/*����*/
extern float set_x,set_y;
extern u8 set_quadrant,counter;

extern struct
{
	float SumError;
	float LastError;
	float Proportion;
	float Integral;
	float Derivative;	
}PID_X,PID_Y;

/*�趨ֵ�Ķ����� set_a , set_r , set_x , set_y , set_quadrant ,set_mode*/
/*��ǰֵ�Ķ����� cur_x , cur_y */



int main(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	delay_init();            //��ʼ����ʱ����	
	pwm_init();			        //TIM2
	usart1_init();         //��ʼ�� ����USART1		(����ͨ��)
  usart3_init();        //��ʼ�� ����USART2 �����ڽ���MPU6050���ݣ�
  NVIC_init();         //�ж����ȼ���ʼ��
	pid_config();
	
	/****PWM����**************/
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);     
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
/******************************************/
	  
  while (1)
  {
		if(set_mode=='p'||set_mode=='w')
			printf("X:%.2f||Y:%.2f||L:%d,%d||SX%.2f,SY%.2f||KP%.0fKI%.2fKD%.0f\r\n",cur_x,cur_y,set_r,set_a,set_x,set_y,PID_X.Proportion,PID_X.Integral,PID_X.Derivative);
		else if(set_mode=='c')
			printf("X:%.2f||Y:%.2f||R=%d||%.2f||%.2f\r\n",cur_x,cur_y,set_r,PID_X.Proportion,PID_X.Derivative);
		else
			printf("X:%.2f||Y:%.2f||KP%.0fKI%.2fKD%.0f\r\n",cur_x,cur_y,PID_X.Proportion,PID_X.Integral,PID_X.Derivative);
		
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0)
		{
			delay_ms(10);
			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0)
			{
				PID_Y.Integral+=0.02;
				PID_X.Integral+=0.02;
				while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0);
			}
		}
		delay_ms(300);
	}
	
}
