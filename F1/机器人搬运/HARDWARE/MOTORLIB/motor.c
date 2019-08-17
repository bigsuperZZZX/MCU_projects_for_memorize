#include "motor.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "delay.h"

//PA0、PA1-左舵机、右舵机
//PA2\PA3\PA6-电机1的IN1,IN2,EN
//PA8\PA11\PA12-电机2的IN1,IN2,EN
//PB3\PB4\PB5-电机3的IN1,IN2,EN
//PB6\PB7\PB12-电机4的IN1,IN2,EN
//PB13\PB14\PB15-电机5的IN1,IN2,EN
//PB8\PB9\PB10\PB11-顶部电机的IN1\IN2\IN3\IN4
 
StepMotor_TypeDef  StepMotor[6];   //0，转盘电机，1-5丝杆电机，顺时针排序
u8 key_clicked;  //按一次，这个变1，再按一次，这个变零
const u8 CW_top[8]= {0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08};    //正时钟旋转相序表 //反转就反过来
const u8 CW_paw[4]= {0x03,0x02,0x00,0x01};    //正时钟旋转相序表 //反转就反过来

void TIM1_UP_IRQHandler(void)   
{
	static signed char top_pulse_count=0, paw_pulse_count[6]={0}; //paw_pulse_count的第零个是不用的，为了使下标对应
	static signed char time=0;  //记录进入中断的次数
	u16 temp;
	u8 i;
	if(read_key()) key_clicked = !key_clicked;
	if(!key_clicked) {all_motor_stop(); return;}
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断源   
  {  
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源
		/***顶部电机（top motor）动作*/
		if(StepMotor[0].SetState != StepMotor[0].CurState)
		{			
			if(StepMotor[0].SetState > StepMotor[0].CurState)
			{
				top_pulse_count++;
				StepMotor[0].CurState ++;
			}
			else 
			{
				top_pulse_count--;
				StepMotor[0].CurState --;
			}
			if(top_pulse_count == 8)  top_pulse_count=0;
			if(top_pulse_count == -1)  top_pulse_count=7;
			temp = (TOP_MOTOR_PORT->ODR | 0xf<<TOP_MOTOR_OFFSET) & (~(0xf<<TOP_MOTOR_OFFSET) | (CW_top[top_pulse_count]<<TOP_MOTOR_OFFSET));
			TOP_MOTOR_PORT->ODR = temp;
		}
		/***爪子电机（pow motor）动作***/
		time++;
		if(time != 2) return;
		time = 0;  // 进三次中断，处理一次（6ms）
		for(i=1;i<=5;i++)
		{
			if(StepMotor[i].SetState != StepMotor[i].CurState)  
			{
				if(StepMotor[i].SetState > StepMotor[i].CurState)
				{
					paw_pulse_count[i]++;
					StepMotor[i].CurState ++;
				}
				else 
				{
					paw_pulse_count[i]--;
					StepMotor[i].CurState --;
				}
				if(paw_pulse_count[i] == 4)  paw_pulse_count[i]=0;
				if(paw_pulse_count[i] == -1)  paw_pulse_count[i]=3;
			}
		}
		if(CW_paw[paw_pulse_count[1]] & 0x02) GPIO_SetBits(M1I1_PORT,M1I1_PIN);
		else 																	GPIO_ResetBits(M1I1_PORT,M1I1_PIN);
		if(CW_paw[paw_pulse_count[1]] & 0x01) GPIO_SetBits(M1I2_PORT,M1I2_PIN);
		else 																	GPIO_ResetBits(M1I2_PORT,M1I2_PIN);
		if(CW_paw[paw_pulse_count[2]] & 0x02) GPIO_SetBits(M2I1_PORT,M2I1_PIN);
		else 																	GPIO_ResetBits(M2I1_PORT,M2I1_PIN);
		if(CW_paw[paw_pulse_count[2]] & 0x01) GPIO_SetBits(M2I2_PORT,M2I2_PIN);
		else 																	GPIO_ResetBits(M2I2_PORT,M2I2_PIN);
		if(CW_paw[paw_pulse_count[3]] & 0x02) GPIO_SetBits(M3I1_PORT,M3I1_PIN);
		else 																	GPIO_ResetBits(M3I1_PORT,M3I1_PIN);
		if(CW_paw[paw_pulse_count[3]] & 0x01) GPIO_SetBits(M3I2_PORT,M3I2_PIN);
		else 																	GPIO_ResetBits(M3I2_PORT,M3I2_PIN);
		if(CW_paw[paw_pulse_count[4]] & 0x02) GPIO_SetBits(M4I1_PORT,M4I1_PIN);
		else 																	GPIO_ResetBits(M4I1_PORT,M4I1_PIN);
		if(CW_paw[paw_pulse_count[4]] & 0x01) GPIO_SetBits(M4I2_PORT,M4I2_PIN);
		else 																	GPIO_ResetBits(M4I2_PORT,M4I2_PIN);
		if(CW_paw[paw_pulse_count[5]] & 0x02) GPIO_SetBits(M5I1_PORT,M5I1_PIN);
		else 																	GPIO_ResetBits(M5I1_PORT,M5I1_PIN);
		if(CW_paw[paw_pulse_count[5]] & 0x01) GPIO_SetBits(M5I2_PORT,M5I2_PIN);
		else 																	GPIO_ResetBits(M5I2_PORT,M5I2_PIN);
		
		if(StepMotor[1].SetState == StepMotor[1].CurState) GPIO_ResetBits(M1EN_PORT,M1EN_PIN);
		else 																							 GPIO_SetBits(M1EN_PORT,M1EN_PIN);
		if(StepMotor[2].SetState == StepMotor[2].CurState) GPIO_ResetBits(M2EN_PORT,M2EN_PIN);
		else 																							 GPIO_SetBits(M2EN_PORT,M2EN_PIN);
		if(StepMotor[3].SetState == StepMotor[3].CurState) GPIO_ResetBits(M3EN_PORT,M3EN_PIN);
		else 																							 GPIO_SetBits(M3EN_PORT,M3EN_PIN);
		if(StepMotor[4].SetState == StepMotor[4].CurState) GPIO_ResetBits(M4EN_PORT,M4EN_PIN);
		else 																							 GPIO_SetBits(M4EN_PORT,M4EN_PIN);
		if(StepMotor[5].SetState == StepMotor[5].CurState) GPIO_ResetBits(M5EN_PORT,M5EN_PIN);
		else 																							 GPIO_SetBits(M5EN_PORT,M5EN_PIN);
		/****************************************/
  }   
} 

char set_top_angle(signed int angle)
{
	if(angle>180 || angle<-180)
		return 1;
	else 
		StepMotor[0].SetState= (int)(11.38*angle+0.5);
	return 0;
}

char set_paw_act(char motor_num, char action)
{
	if(action == UP)
		StepMotor[motor_num].SetState= UP_NUM;
	if(action == DOWN)
		StepMotor[motor_num].SetState= DOWN_NUM;
	return 0;
}

char test_running(char motor_num)
{
	return StepMotor[motor_num].CurState == StepMotor[motor_num].SetState ? 0:1;
}

void set_speed_L(u8 speed)  //0-50;
{
	if(speed > 50) return;
	TIM_SetCompare1(TIM2,100-speed);
}

void set_speed_R(u8 speed)  //0-50;
{
	if(speed > 50) return;
	TIM_SetCompare2(TIM2,speed+50);
}

unsigned char read_key(void)
{
	static u8 flag_key=1;//按键按松开标志
	u8 key_state=GPIO_ReadInputDataBit(KEY_PORT,KEY_PIN);
	if(flag_key && key_state==0)
	{
		flag_key=0;
		return 1;	// 按键按下
	}
	else if(1==key_state)
		flag_key=1;
	return 0;//无按键按下
}

void all_motor_stop(void)
{
	GPIO_ResetBits(M1EN_PORT,M1EN_PIN);
	GPIO_ResetBits(M2EN_PORT,M2EN_PIN);
	GPIO_ResetBits(M3EN_PORT,M3EN_PIN);
	GPIO_ResetBits(M4EN_PORT,M4EN_PIN);
	GPIO_ResetBits(M5EN_PORT,M5EN_PIN);
	set_speed_L(25);
	set_speed_R(25);
	TOP_MOTOR_PORT->ODR &= ~(0xf<<TOP_MOTOR_OFFSET);
}

void motor_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;  
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_TIM1|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/*********端口启动**************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_6|GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12;	/////////// 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All^(GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2); //////////////////
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; ////////////////////////////////
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; ////////////////////////////////PC13按键
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	/*****************************************/
	/*定时器2产生50HzPWM****************************/
	TIM_TimeBaseStructure.TIM_Period = 1000-1; // 
	TIM_TimeBaseStructure.TIM_Prescaler =1440-1; //   50Hz = 72000000/1000/3600
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;                            //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

  TIM_CtrlPWMOutputs(TIM2,ENABLE);	//MOE 主输出使能	

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH1预装载使能	 
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH4预装载使能	  
	
	TIM_ARRPreloadConfig(TIM2, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM2, ENABLE);  //使能TIM1
	TIM_SetCompare1(TIM2,75);
	TIM_SetCompare2(TIM2,75);
	/******************************************************/
	/*定时器1产生500Hz基准定时器中断****************************/
	TIM_TimeBaseStructure.TIM_Period = 1000-1; //设置自动重装载寄存器周期值  
  TIM_TimeBaseStructure.TIM_Prescaler =144-1;//设置预分频值  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式  
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;//重复计数设置  
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //参数初始化 
	
  TIM_ClearFlag(TIM1, TIM_FLAG_Update);//清中断标志位  
	TIM_ITConfig(TIM1,TIM_IT_Update|TIM_IT_Trigger,ENABLE);  
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   //2位抢占，2位等待
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;    
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
  NVIC_Init(&NVIC_InitStructure);   
  
  TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设 
	/*********************************************************/
	GPIO_ResetBits(M1EN_PORT,M1EN_PIN);
	GPIO_ResetBits(M2EN_PORT,M2EN_PIN);
	GPIO_ResetBits(M3EN_PORT,M3EN_PIN);
	GPIO_ResetBits(M4EN_PORT,M4EN_PIN);
	GPIO_ResetBits(M5EN_PORT,M5EN_PIN);
}


