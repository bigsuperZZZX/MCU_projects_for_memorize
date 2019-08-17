#include "moto.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include <math.h>


short pwm_r=0,pwm_l=0;
float set_x=-1.20;
extern float cur_x,cur_x_v;
extern short pwm_r,pwm_l;
extern long pulse_r,pulse_l;

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
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2; //模式2，输入数字代表占空比高
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
	
	TIM_SetCompare1(TIM2,0);   //对应电机驱动 in1—4
	TIM_SetCompare2(TIM2,0);
	TIM_SetCompare3(TIM2,0);
	TIM_SetCompare4(TIM2,0);	
	
	/******************************/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源
	TIM_TimeBaseStructure.TIM_Period = 1000;//设置自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = 719;//设置用来作为TIMx时钟频率预分频值，100Khz计数频率
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);	
	TIM_Cmd(TIM4,ENABLE); //使能或者失能TIMx外设
	/* 设置中断参数，并打开中断 */
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE );	//使能或者失能指定的TIM中断
	
	/* 设置NVIC参数 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //打开TIM3_IRQn的全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;	//抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;  //响应优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能
	NVIC_Init(&NVIC_InitStructure);	
}


void TIM4_IRQHandler()	 //定时器4中断函数
{
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	
	moto_control();
	
	if(pwm_r>0)
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
	}
}

void moto_control(void)
{
	static u8 i=0;
	
	float error=0;
	float f=0;
	error=cur_x-set_x;
	if(error>0) f=1;
	if(error<0) f=-1;
	
	//pid_r.SumError+=1/(fabs(error)+1)*f;
	pid_r.SumError+=error;
	if(pid_r.SumError>100)  pid_r.SumError=100;
	if(pid_r.SumError<-100)  pid_r.SumError=-100;
	
	if(cur_x<20&&cur_x>-20)
	{
		pwm_r=(short)(pid_r.Proportion*sqrt(fabsf(error)/10)*f + pid_r.Integral*pid_r.SumError + pid_r.Derivative*cur_x_v);
		pwm_l=pwm_r;
		pwm_r-=(pulse_r-pulse_l)*5;
		pwm_l+=(pulse_r-pulse_l)*5;
	}
	else
	{
		pwm_r=0;
		pwm_l=0;
	}
	if(pwm_r>2500)
		pwm_r=2500;
	if(pwm_r<-2500)
		pwm_r=-2500;
	if(pwm_l>2500)
		pwm_l=2500;
	if(pwm_l<-2500)
		pwm_l=-2500;
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




