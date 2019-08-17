#include "stm32f10x.h"
#include "motor_control.h"
#include "stm32f10x_tim.h"

//轮子一圈，脉冲260个

extern long pulse_1,pulse_2;

struct pid
{ 
	short SetPulse;//定义设定值
	short ActualPulse;//定义实际值
	short err; //定义偏差值 
	short err_last;//定义上一个偏差值 
	float Kp,Ki,Kd;//定义比例、积分、微分系数
	short PWM;//定义PWM值（控制执行器的变量）
	short integral;//定义积分值 
}moto_angle_1,moto_angle_2;

void moto_1_pid(void)
{
	moto_angle_1.ActualPulse = (short)pulse_1;
	moto_angle_1.err=moto_angle_1.SetPulse-moto_angle_1.ActualPulse; 
	if(moto_angle_1.err>-40&&moto_angle_1.err<40)
		moto_angle_1.integral+=moto_angle_1.err;
	moto_angle_1.PWM = (short)(moto_angle_1.Kp * moto_angle_1.err + moto_angle_1.Ki * moto_angle_1.integral + moto_angle_1.Kd * (moto_angle_1.err-moto_angle_1.err_last));
	if(moto_angle_1.PWM>3000) moto_angle_1.PWM=2999;
	if(moto_angle_1.PWM<-3000) moto_angle_1.PWM=-2999;
	moto_angle_1.err_last=moto_angle_1.err;       
}

void moto_2_pid(void)
{
	moto_angle_2.ActualPulse = (short)pulse_2;
	moto_angle_2.err=moto_angle_2.SetPulse-moto_angle_2.ActualPulse; 
	if(moto_angle_2.err>-40&&moto_angle_2.err<40)
		moto_angle_2.integral+=moto_angle_2.err;
	moto_angle_2.PWM = (short)(moto_angle_2.Kp * moto_angle_2.err + moto_angle_2.Ki * moto_angle_2.integral + moto_angle_2.Kd * (moto_angle_2.err-moto_angle_2.err_last));
	if(moto_angle_2.PWM>3000) moto_angle_2.PWM=2999;
	if(moto_angle_2.PWM<-3000) moto_angle_2.PWM=-2999;
	moto_angle_2.err_last=moto_angle_2.err;       
}

void PID_init()
{      
	moto_angle_1.SetPulse=0;     
	moto_angle_1.ActualPulse=0;    
	moto_angle_1.err=0;     
	moto_angle_1.err_last=0;     
	moto_angle_1.PWM=0;     
	moto_angle_1.integral=0;     
	moto_angle_1.Kp=38;     
	moto_angle_1.Ki=20;     
	moto_angle_1.Kd=38;         //35,20,40
	moto_angle_2.SetPulse=0;     
	moto_angle_2.ActualPulse=0;    
	moto_angle_2.err=0;     
	moto_angle_2.err_last=0;     
	moto_angle_2.PWM=0;     
	moto_angle_2.integral=0;     
	moto_angle_2.Kp=10;     
	moto_angle_2.Ki=5;     
	moto_angle_2.Kd=0;         //35,20,40
}

void pwm_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);           //
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;  //TIM4对应B的6789
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 3000;
	TIM_TimeBaseStructure.TIM_Prescaler = 3;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
//	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2; //模式2，输入数字代表占空比高
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OC1Init(TIM4, & TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM4, & TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM4, & TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM4, & TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM4, ENABLE);
	
	TIM_SetCompare1(TIM4,0);
	TIM_SetCompare2(TIM4,0);
	TIM_SetCompare3(TIM4,0);
	TIM_SetCompare4(TIM4,0);
}
