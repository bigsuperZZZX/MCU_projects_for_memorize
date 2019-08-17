#include "pwm.h"

/*******************************************************************************
* 函 数 名         : pwm_init
* 函数功能		   : IO端口及TIM3初始化函数	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void pwm_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;   //声明一个结构体变量，用来初始化GPIO

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//声明一个结构体变量，用来初始化定时器

	TIM_OCInitTypeDef TIM_OCInitStructure;//根据TIM_OCInitStruct中指定的参数初始化外设TIMx

	/* 开启时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出GPIO_Mode_Out_PP
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//TIM3定时器初始化
	TIM_TimeBaseInitStructure.TIM_Period = 899;	   //设置自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler =20;//设置用来作为TIMx时钟频率预分频值，100Khz计数频率
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
	TIM_TimeBaseInit(TIM2, & TIM_TimeBaseInitStructure);

	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM2,ENABLE);//改变指定管脚的映射	//pb5

	//PWM初始化	  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM输出使能
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;

	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
    
   /* TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM输出使能
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;

	TIM_OC4Init(TIM2,&TIM_OCInitStructure);*/
	//注意此处初始化时TIM_OC2Init而不是TIM_OCInit，否则会出错。因为固件库的版本不一样。

	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);//使能或者失能TIMx在CCR2上的预装载寄存器
	//TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);//使能或者失能TIMx在CCR2上的预装载寄存器
    
	TIM_Cmd(TIM2,ENABLE);//使能或者失能TIMx外设
	
	TIM_SetCompare3(TIM2,890);
}

