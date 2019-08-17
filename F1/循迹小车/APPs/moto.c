#include "stm32f10x.h"
#include "moto.h"
#include "stm32f10x_tim.h"
#include "math.h"
#include "LDC1314.h"
#include "delay.h"
#include "math.h"

#define dist_1 900   //轮子一圈16cm
#define dist_2 200
#define dist_3 200
#define dist_4 700
#define dist_5 200
#define dist_6 900


u16 PWM_M_L=0,PWM_M_R=0,PWM_C=440;
u8 state_change_flag=0;
u8 stop_flag=1;
u32 time=0;  //0.05秒
extern u16 coil_0,coil_1,coil_2,coil_3;
u8 corner=0;  //过了几个弯
u8 corner_in=0;   //是否正在过弯
u16 dist_count_last=0;
extern u32 counter;

void PWM_init(void)  //pwm输出PC6
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 3000;
	TIM_TimeBaseStructure.TIM_Prescaler = 29;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2; //模式2，输入数字代表占空比高
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OC2Init(TIM5, & TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM5, & TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM5, ENABLE);
	
	TIM_SetCompare2(TIM5,0);
	TIM_SetCompare4(TIM5,0);
	/***************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 3000;  //PWM_C  180--630  中点440
	TIM_TimeBaseStructure.TIM_Prescaler = 239;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2; //模式2，输入数字代表占空比高
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OC1Init(TIM2, & TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM2, ENABLE);
	
	TIM_SetCompare1(TIM2,440);
}

void time_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	 //声明一个结构体变量，用来初始化GPIO
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源
	TIM_TimeBaseInitStructure.TIM_Period = 1000;//设置自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 3599;//设置用来作为TIMx时钟频率预分频值，100Khz计数频率
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);	
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


void TIM4_IRQHandler()	 //定时器3中断函数
{
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	time++;
	TIM_SetCompare2(TIM5,PWM_M_L);
	TIM_SetCompare4(TIM5,PWM_M_R);
	TIM_SetCompare1(TIM2,PWM_C);
}

void moto_control(void)  //PWM_C  180--630  中点440
{
		if(abs(coil_1-LDC_Read_Reg(0X02))>1)
		{
			PWM_C=540;     ////
			if(PWM_C>630)
				PWM_C=630;
			if(PWM_C<180)
				PWM_C=180;
			PWM_M_L=300;   ////
			if(PWM_M_L>1000)
				PWM_M_L=1000;
			if(PWM_M_L<400)
				PWM_M_L=400;
			PWM_M_R=700;   ////
			if(PWM_M_R>1000)
				PWM_M_R=1000;
			if(PWM_M_R<400)
				PWM_M_R=400;
			delay_ms(8);
		}
		else if(abs(coil_0-LDC_Read_Reg(0X00))>1)
		{
			PWM_C=340;    ////
			if(PWM_C>630)
				PWM_C=630;
			if(PWM_C<180)
				PWM_C=180;
			PWM_M_L=700;   ////
			if(PWM_M_L>1000)
				PWM_M_L=1000;
			if(PWM_M_L<400)
				PWM_M_L=400;
			PWM_M_R=300;   ////
			if(PWM_M_R>1000)
				PWM_M_R=1000;
			if(PWM_M_R<400)
				PWM_M_R=400;
			delay_ms(8);
		}
		else if(abs(coil_3-LDC_Read_Reg(0X06))<3)
		{
			PWM_C=440;
			PWM_M_L=PWM_M_R=700;
		}
}

