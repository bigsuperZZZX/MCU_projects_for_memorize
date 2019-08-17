#include "moto.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include <math.h>
#include "usart.h"	
#include "delay.h"


short pwm_r=0,pwm_l=0;
float set_x=-1.20;
extern long pulse_r,pulse_l;
extern u16 sensor;
extern u16 dist1_mm,dist2_mm;
extern u8 flag_stark;

struct
{
	float SumError;
	float LastError;
	float Proportion;
	float Integral;
	float Derivative;	
}pid_r,pid_l;

void moto_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);           //
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 3000;
	TIM_TimeBaseStructure.TIM_Prescaler = 2;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2; //ģʽ2���������ִ���ռ�ձȸ�
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OC1Init(TIM2, & TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM2, & TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM2, & TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM2, & TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM2, ENABLE);
	
	TIM_SetCompare1(TIM2,0);   //��Ӧ������� in1��4
	TIM_SetCompare2(TIM2,0);
	TIM_SetCompare3(TIM2,0);
	TIM_SetCompare4(TIM2,0);	
	
	/******************************/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//���TIMx���жϴ�����λ:TIM �ж�Դ
	TIM_TimeBaseStructure.TIM_Period = 30000;//�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 719;//����������ΪTIMxʱ��Ƶ��Ԥ��Ƶֵ��100Khz����Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);	
	TIM_Cmd(TIM4,ENABLE); //ʹ�ܻ���ʧ��TIMx����
	/* �����жϲ����������ж� */
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE );	//ʹ�ܻ���ʧ��ָ����TIM�ж�
	
	/* ����NVIC���� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //��TIM3_IRQn��ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;	//��ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;  //��Ӧ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ��
	NVIC_Init(&NVIC_InitStructure);	
}

long absl(long a)
{
	if(a<0) return -a;
	return a;
}

void TIM4_IRQHandler()	 //��ʱ��4�жϺ���
{
	static long last_pulse_r=0,last_pulse_l=0;
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE );
	
	if((absl(pulse_r-last_pulse_r) < 3) )
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE );
	
	/*if(pwm_r>0)
	{
		TIM_SetCompare1(TIM2,pwm_r);
		TIM_SetCompare2(TIM2,0);
	//	TIM_SetCompare3(TIM2,pwm_l+(pulse_r-pulse_l)*5);
	//	TIM_SetCompare4(TIM2,0);
	}
	else
	{
		TIM_SetCompare1(TIM2,0);
		TIM_SetCompare2(TIM2,-pwm_r);
	//	TIM_SetCompare3(TIM2,0);
	//	TIM_SetCompare4(TIM2,-pwm_l);
	}
	if(pwm_l>0)
	{
		TIM_SetCompare3(TIM2,pwm_l);
		TIM_SetCompare4(TIM2,0);
	}
	else
	{
		TIM_SetCompare3(TIM2,0);
		TIM_SetCompare4(TIM2,-pwm_l);
	}*/
}

void moto_control(void)   //�ڴ���2���жϺ����е���
{
	u16 dist = (dist1_mm+dist2_mm)/2;
	if(dist>200)
	{
		short pwm_temp=1500-(dist-200)*10+(dist2_mm-dist1_mm)*15;
		if(pwm_temp>1600)
			pwm_temp=1600;
		if(pwm_temp<1000)
			pwm_temp=1000;
		pwm_r=pwm_temp;
		pwm_l=1500;
		TIM_SetCompare1(TIM2,pwm_r);
		TIM_SetCompare3(TIM2,pwm_l);
	}
	if(dist<200)
	{
		short pwm_temp=1500-(200-dist)*10+(dist1_mm-dist2_mm)*15;
		if(pwm_temp>1500)
			pwm_temp=1500;
		if(pwm_temp<1200)
			pwm_temp=1200;
		pwm_r=1500;
		pwm_l=pwm_temp;
		TIM_SetCompare1(TIM2,pwm_r);
		TIM_SetCompare3(TIM2,pwm_l);
	}
		dist1_mm=0;
		dist2_mm=0;
}

void moto_ControlInit(void)
{
	pid_r.Derivative=0;
	pid_r.Integral=0;
	pid_r.LastError=0;
	pid_r.Proportion=0;
	pid_r.SumError=0;
	
	pid_l.Derivative=0;
	pid_l.Integral=0;
	pid_l.LastError=0;
	pid_l.Proportion=0;
	pid_l.SumError=0;
}

void turn_left(void)
{
	pulse_r=0;
	pulse_l=0;
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE );	
	TIM_SetCompare1(TIM2,pwm_r);
	TIM_SetCompare3(TIM2,(u16)(pwm_l/2*offset));
	while(pulse_r-pulse_l<300);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE );	
}

void turn_right(void)
{
	pulse_r=0;
	pulse_l=0;
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE );	
	TIM_SetCompare1(TIM2,pwm_r/2);
	TIM_SetCompare3(TIM2,(u16)(pwm_l*offset));
	while(pulse_l-pulse_r<200);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE );	
}

void gostreight(void)
{
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE );	
	TIM_SetCompare1(TIM2,2000);
	TIM_SetCompare3(TIM2,(u16)(2000*offset));
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE );	
}

void stuck(u8 stuck)
{
	if(stuck==1)  //��ת��ס��
	{
		TIM_SetCompare1(TIM2,0);
		TIM_SetCompare2(TIM2,2000);
		TIM_SetCompare3(TIM2,0);
		TIM_SetCompare4(TIM2,2000);
		delay_ms(1000);
		TIM_SetCompare2(TIM2,0);
		delay_ms(400);
		stuck=0;
	}
	if(stuck==2)  //��ת��ס��
	{
		TIM_SetCompare1(TIM2,0);
		TIM_SetCompare2(TIM2,2000);
		TIM_SetCompare3(TIM2,0);
		TIM_SetCompare4(TIM2,2000);
		delay_ms(1000);
		TIM_SetCompare4(TIM2,0);
		delay_ms(400);
		stuck=0;
	}
}




