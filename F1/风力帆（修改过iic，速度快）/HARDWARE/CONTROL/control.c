#include "control.h"	
#include "oled.h"
#include "MPU6050.h"
#include "stm32f10x_tim.h"
#include "TTP229.h"

#define SET_PITCH_INIT 45
#define BEEP_ON()  GPIO_SetBits(GPIOA,GPIO_Pin_6)
#define BEEP_OFF() GPIO_ResetBits(GPIOA,GPIO_Pin_6)


extern float Pitch,Roll;
float Set_Pitch=SET_PITCH_INIT;
int Pwm=0;
//float Proportion=15, Integral=0.4, Derivative=550;
float Proportion=50, Integral=0.4, Derivative=550;
extern u8 start_flag;
float SumError=0, LastError=0;

int myabs(int a)
{
	if(a<0) return -a;
	return a;
}
/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
         5ms��ʱ�ж���MPU6050��INT���Ŵ���
         �ϸ�֤���������ݴ����ʱ��ͬ��				 
**************************************************************************/
int EXTI9_5_IRQHandler(void) 
{
	static u8 count=0 , Setting_buff = SET_PITCH_INIT , beep_count=0, beep_flag=0;
//		GPIO_SetBits(GPIOA,GPIO_Pin_1);
	 if(PBin(5)==0)		
	 {   
		 EXTI->PR=1<<5;                                                      //���LINE5�ϵ��жϱ�־λ   
	   Read_DMP();		 //===������̬	
		 if(start_flag != 0)
		 {
//			 if(Set_Pitch < 35) Integral=0.3;
//			 else Integral=0.4;
//			 Integral = 0.26 + (Set_Pitch>20 ? (Set_Pitch - 20)/180 : 0);
			 Pwm = PID_PosLocCalc(Pitch);
				if(Pwm>2500)
					Pwm =2500;
				if(Pwm<100)
					Pwm =100;
			TIM_SetCompare1(TIM2,Pwm);
				
				OLED_ShowNum(0,6,Pwm,4,16);
				
			if(myabs(Set_Pitch - Pitch) < 0.2 && beep_flag == 0) beep_count++;     //������
			if(beep_count == 30 && beep_flag == 0)
		  {
				BEEP_ON(); 
				beep_flag = 1;
			}
		 }
		 else
		 {
			 SumError=0;
			 LastError=0;
			 Pwm=0;
			 TIM_SetCompare1(TIM2,0);
		 }
		 
/*		 switch(count%6)   //��ʾ����
		 {
			 case 0:OLED_ShowFloat(72,0,Pitch,6,16);break;
			 case 1:if((u8)Set_Pitch == Setting_buff) OLED_ShowNum(80,3,(u8)Set_Pitch,2,16);break;
			 case 2:OLED_ShowFloat(0,2,Proportion,6,16);break;
			 case 3:OLED_ShowFloat(64,2,Integral*10,6,16);break;
			 case 4:OLED_ShowFloat(0,4,Derivative,7,16);break;
			 case 5:OLED_ShowNum(0,6,Pwm,6,16);break;
		 }*/
		  /*switch(count%6)   //��ʾ����
		 {
			 case 0:OLED_ShowFloat(72,0,Pitch,6,16);break;
			 case 1:if((u8)Set_Pitch == Setting_buff) OLED_ShowNum(80,3,(u8)Set_Pitch,2,16);break;
			 case 2:OLED_ShowFloat(0,8,Proportion,6,16);break;
			 case 3:OLED_ShowFloat(64,8,Integral*10,6,16);break;
			 case 4:OLED_ShowFloat(0,8,Derivative,7,16);break;
			 case 5:OLED_ShowNum(0,8,Pwm,6,16);break;
		 }*/
		 switch(count%6)   //��ʾ����
		 {
			 case 0:OLED_ShowFloat(72,0,Pitch,6,16);break;
			 case 1:if((u8)Set_Pitch == Setting_buff) OLED_ShowNum(80,3,(u8)Set_Pitch,2,16);break;
			 default: break;
		 }
		 count++;
	 } 
			
	 if(count % 4 == 0)   //�������
	 {		 
		 static u8 Pressed, Prs_chg_flag=0, Setting_mode=0;
		 Prs_chg_flag = get_key2(&Pressed);  //�õ���ǰ�İ���ֵ
//		 OLED_ShowNum(64,6,Setting_mode,2,16);
		 
		 if(Setting_mode == 1 && Prs_chg_flag==1 && Pressed<11)   //����һ�γ��򽻻��Ⱥ�λ�ã�����һ���жϽ�������OLED��ʾ����
		 {
			 static signed char Setting_times = 0; //��¼���ǵ�һ�λ��ǵڶ��ν�����һ�γ���
			 if(Setting_times == 0)
			 {
				 count = 0;
				 Setting_buff = Setting_buff%10 + Pressed%10*10;
				 OLED_ShowChar(80,3,Pressed%10+48);
			 }  
			 if(Setting_times == 1)
			 {
				 count = 0;
				 Setting_buff = Setting_buff%100/10*10 + Pressed%10;
				 OLED_ShowChar(88,3,Pressed%10+48);
				 Setting_times = -1;
				 Setting_mode = 0;      //ֱ�ӽ�������ģʽ
				 
			 }
			 Setting_times++;
		 }
		 
		 if(Prs_chg_flag == 1)
			 switch(Pressed)
			 {
				 case 13: start_flag = !start_flag; beep_count = 0; 
									BEEP_OFF(); beep_flag = 0; break;
				 case 14: Setting_mode = 1; Setting_buff = Set_Pitch;
									OLED_ShowChar(102,3,'?'); BEEP_OFF(); break;
				 case 15: Setting_mode = 0; Set_Pitch = Setting_buff; 
									OLED_ShowChar(102,3,' '); 
									beep_flag = 0; beep_count = 0; break;
				 case 12: Set_Pitch++; Setting_buff = Set_Pitch; break;
				 case 16: Set_Pitch--; Setting_buff = Set_Pitch; break;
				 default: break;
			 }
	 }
		  
//	GPIO_ResetBits(GPIOA,GPIO_Pin_1);	 
	 return 0;	  
}

int32_t PID_PosLocCalc(float NextPoint)
{
   register float  iError,dError;

	iError = Set_Pitch - NextPoint;        // ƫ��
	SumError += iError;				    // ����
	if(SumError > 5000.0)					//�����޷�2300
		SumError = 5000.0;
	else if(SumError < -5000.0)
		SumError = -5000.0;	
	dError = iError - LastError; 			// ��ǰ΢��
	LastError = iError;
	
	return(int32_t)(  Proportion * iError           	// ������
          		    + Integral   * SumError 		// ������
          		    + Derivative * dError);
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	static char Res[20];
	static char i=0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		EXTI->IMR&=~(1<<5);  // �����ж�
		TIM_SetCompare1(TIM2,0);
		Res[i] = USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		if(Res[i]=='s')
		{
			Res[i]='\0';
			if(Res[0]=='p') Proportion=atoi(&Res[1]);
			if(Res[0]=='i') Integral=atof(&Res[1]);
			if(Res[0]=='d') Derivative=atoi(&Res[1]);  
			if(Res[0]=='g') Set_Pitch=(float)atoi(&Res[1]); 
			i=-1;
			EXTI->IMR|=1<<5;               // ���ж�
		}
		i++;
  } 
}

void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);           //
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
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
	
	TIM_Cmd(TIM2, ENABLE);
	
	TIM_SetCompare1(TIM2,100);   //��Ӧ������� in1
}

void Key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); 
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);    //���ƶ�
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
}


