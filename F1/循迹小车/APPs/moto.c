#include "stm32f10x.h"
#include "moto.h"
#include "stm32f10x_tim.h"
#include "math.h"
#include "LDC1314.h"
#include "delay.h"
#include "math.h"

#define dist_1 900   //����һȦ16cm
#define dist_2 200
#define dist_3 200
#define dist_4 700
#define dist_5 200
#define dist_6 900


u16 PWM_M_L=0,PWM_M_R=0,PWM_C=440;
u8 state_change_flag=0;
u8 stop_flag=1;
u32 time=0;  //0.05��
extern u16 coil_0,coil_1,coil_2,coil_3;
u8 corner=0;  //���˼�����
u8 corner_in=0;   //�Ƿ����ڹ���
u16 dist_count_last=0;
extern u32 counter;

void PWM_init(void)  //pwm���PC6
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
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2; //ģʽ2���������ִ���ռ�ձȸ�
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
	
	TIM_TimeBaseStructure.TIM_Period = 3000;  //PWM_C  180--630  �е�440
	TIM_TimeBaseStructure.TIM_Prescaler = 239;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2; //ģʽ2���������ִ���ռ�ձȸ�
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OC1Init(TIM2, & TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM2, ENABLE);
	
	TIM_SetCompare1(TIM2,440);
}

void time_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	 //����һ���ṹ�������������ʼ��GPIO
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//���TIMx���жϴ�����λ:TIM �ж�Դ
	TIM_TimeBaseInitStructure.TIM_Period = 1000;//�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 3599;//����������ΪTIMxʱ��Ƶ��Ԥ��Ƶֵ��100Khz����Ƶ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);	
	TIM_Cmd(TIM4,ENABLE); //ʹ�ܻ���ʧ��TIMx����
	/* �����жϲ����������ж� */
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE );	//ʹ�ܻ���ʧ��ָ����TIM�ж�
	
	/* ����NVIC���� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //��TIM3_IRQn��ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;	//��ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;  //��Ӧ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ��
	NVIC_Init(&NVIC_InitStructure);	
}


void TIM4_IRQHandler()	 //��ʱ��3�жϺ���
{
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	time++;
	TIM_SetCompare2(TIM5,PWM_M_L);
	TIM_SetCompare4(TIM5,PWM_M_R);
	TIM_SetCompare1(TIM2,PWM_C);
}

void moto_control(void)  //PWM_C  180--630  �е�440
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

