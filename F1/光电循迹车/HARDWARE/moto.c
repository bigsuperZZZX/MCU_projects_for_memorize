#include "moto.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include <math.h>
#include "oled.h"

short pwm_r=0,pwm_l=0;
extern long pulse_r,pulse_l;

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
	TIM_TimeBaseStructure.TIM_Period = 1000;//�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 719;//����������ΪTIMxʱ��Ƶ��Ԥ��Ƶֵ��100Khz����Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);	
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


void TIM4_IRQHandler()	 //��ʱ��4�жϺ���
{
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	
	moto_control();
	
	if(pwm_r>0)
	{
		TIM_SetCompare1(TIM2,pwm_r);
		TIM_SetCompare2(TIM2,0);
	}
	else
	{
		TIM_SetCompare1(TIM2,0);
		TIM_SetCompare2(TIM2,-pwm_r);
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

/*void moto_control(void)     //��ɫ��Ϊ��߽�
{
	u16 State=0x1c;
	static u16 last_State=0;
	State&=GPIO_ReadInputData(GPIOG);
	
	switch(State)
	{
		case 0x1c:pwm_r=1000;pwm_l=1000;break;						   //00011100		
		case 0x18:pwm_r=1000;pwm_l=0;		break;
		case 0x14:pwm_r=1000;pwm_l=1000;break;
		case 0x0c:pwm_r=0;	 pwm_l=1000;break;
		case 0x10:pwm_r=1000;pwm_l=-1000;break;
		case 0x04:pwm_r=-1000;pwm_l=1000;break;
		case 0x08:pwm_r=1000;pwm_l=1000;break;
		case 0x00:pwm_r=0;	pwm_l=-1000;break;
		default: break;
	}
	
	if((State!=last_State)&&(State==0x14))
	{
		pulse_r=0;
		pulse_l=0;
	}
	
	if(State==0x14)
	{
		pwm_r-=(pulse_r-pulse_l)*5;
		pwm_l+=(pulse_r-pulse_l)*5;
	}
	
	if(pwm_r>2500)
		pwm_r=2500;
	if(pwm_r<-2500)
		pwm_r=-2500;
	if(pwm_l>2500)
		pwm_l=2500;
	if(pwm_l<-2500)
		pwm_l=-2500;
	
	last_State=State;
}*/

void moto_control(void)     //Ѱ�ڰױ߽�
{
	u16 State=0x0c;
	static u16 last_State=0;
	State&=GPIO_ReadInputData(GPIOG);
	
	
		if(State==0x0c)
		{
			pwm_r=0;
			pwm_l=1500;
		}
		else if(State==0x08)
		{
			pwm_r=1500;
			pwm_l=1500;
		}
		else if(State==0x00)
		{
			pwm_r=1500;
			pwm_l=0;
		}
	
	if((State!=last_State)&&(State==0x08))
	{
		pulse_r=0;
		pulse_l=0;
	}
	
	if(State==0x08)
	{
		pwm_r-=(pulse_r-pulse_l)*5;
		pwm_l+=(pulse_r-pulse_l)*5;
	}
	
	if(pwm_r>2999)
		pwm_r=2999;
	if(pwm_r<-2999)
		pwm_r=-2999;
	if(pwm_l>2999)
		pwm_l=2999;
	if(pwm_l<-2999)
		pwm_l=-2999;
	
	last_State=State;
}

void photoelectric_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
}




