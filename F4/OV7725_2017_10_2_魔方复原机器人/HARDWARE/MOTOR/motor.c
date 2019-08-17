#include "motor.h"
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

void TIM3_IRQHandler(void)   
{
	static signed char paw_pulse_count[7]={0}; //paw_pulse_count的第零个是不用的，为了使下标对应
	u8 i;
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)//检查指定的TIM中断发生与否:TIM 中断源   
  {  
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源
		
		
		PCout(4) = ~PCout(4);
		
		
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

extern short Restore_step[200];

void delay_us2(u16 t)
{
	int i;
	while(t--)
	{
		for(i=41;i>1;i--);
	}
}

//PE0-5 ENABLE
//PF01 23 45 67 89 1011 前一个数是方向，后一个是脉冲

void motor_enable(char moto_posi)  //1-6
{
	PEout(0) = 1;
	PEout(1) = 1;
	PEout(2) = 1;
	PEout(3) = 1;
	PCout(0) = 1;
	PCout(1) = 1;
	switch(moto_posi)
	{
		case 1: PEout(0) = 0; break;
		case 2: PEout(1) = 0; break;
		case 3: PEout(2) = 0; break;
		case 4: PEout(3) = 0; break;
		case 5: PCout(0) = 0; break;
		case 6: PCout(1) = 0; break;
		default:break;
	}
}

short my_abs(short x)
{
	if(x>=0)
		return x;
	return -x;
}

signed int offset_cal(short Restore_step_now, short Restore_step_next, int offset_absval)
{
	static char count = 0;
	short now,next;
	if(count < 16)
	{
		count++;
		now = my_abs(Restore_step_now)/10;
		next = my_abs(Restore_step_next)/10;
		if(now == next) 
			return 0;
		if((now == 1 && next == 2) || (now == 2 && next == 1) || 
			 (now == 3 && next == 4) || (now == 4 && next == 3) ||
			 (now == 5 && next == 6) || (now == 6 && next == 5))    //前后两面是相对的
			return 0;
		if((Restore_step_next > 0 && Restore_step_now > 0) || (Restore_step_next < 0 && Restore_step_now < 0))
			return offset_absval;
		if((Restore_step_next < 0 && Restore_step_now > 0) || (Restore_step_next > 0 && Restore_step_now < 0))
			return -offset_absval;
	}
	else
	{
		return 0;
	}
	return 0;
}

void motor_action(void)
{
	#define offset_absval 10 
	#define steps_90deg 400
	#define delay_count 120
	signed int steps_offset = 0;
	int i=0,j=0;
	 while(Restore_step[i] != '#')
	{
		steps_offset = offset_cal(Restore_step[i],Restore_step[i+1],offset_absval);
		if(Restore_step[i] == 11 || Restore_step[i] == -13)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(0) = 0;
			for(j=0;j<steps_90deg + steps_offset;j++)
			{
				PFout(1) = 1;
				delay_us2(delay_count);
				PFout(1) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == -11 || Restore_step[i] == 13)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(0) = 1;
			for(j=0;j<steps_90deg + steps_offset;j++)
			{
				PFout(1) = 1;
				delay_us2(delay_count);
				PFout(1) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == 21 || Restore_step[i] == -23)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(2) = 0;
			for(j=0;j<steps_90deg + steps_offset;j++)
			{
				PFout(3) = 1;
				delay_us2(delay_count);
				PFout(3) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == -21 || Restore_step[i] == 23)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(2) = 1;
			for(j=0;j<steps_90deg + steps_offset;j++)
			{
				PFout(3) = 1;
				delay_us2(delay_count);
				PFout(3) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == 31 || Restore_step[i] == -33)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(4) = 0;
			for(j=0;j<steps_90deg + steps_offset;j++)
			{
				PFout(5) = 1;
				delay_us2(delay_count);
				PFout(5) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == -31 || Restore_step[i] == 33)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(4) = 1;
			for(j=0;j<steps_90deg + steps_offset;j++)
			{
				PFout(5) = 1;
				delay_us2(delay_count);
				PFout(5) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == 41 || Restore_step[i] == -43)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(6) = 0;
			for(j=0;j<steps_90deg + steps_offset;j++)
			{
				PFout(7) = 1;
				delay_us2(delay_count);
				PFout(7) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == -41 || Restore_step[i] == 43)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(6) = 1;
			for(j=0;j<steps_90deg + steps_offset;j++)
			{
				PFout(7) = 1;
				delay_us2(delay_count);
				PFout(7) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == 51 || Restore_step[i] == -53)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(8) = 0;
			for(j=0;j<steps_90deg + steps_offset;j++)
			{
				PFout(9) = 1;
				delay_us2(delay_count);
				PFout(9) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == -51 || Restore_step[i] == 53)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(8) = 1;
			for(j=0;j<steps_90deg + steps_offset;j++)
			{
				PFout(9) = 1;
				delay_us2(delay_count);
				PFout(9) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == 61 || Restore_step[i] == -63)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(10) = 0;
			for(j=0;j<steps_90deg + steps_offset;j++)
			{
				PFout(11) = 1;
				delay_us2(delay_count);
				PFout(11) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == -61 || Restore_step[i] == 63)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(10) = 1;
			for(j=0;j<steps_90deg + steps_offset;j++)
			{
				PFout(11) = 1;
				delay_us2(delay_count);
				PFout(11) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == 12)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(0) = 0;
			for(j=0;j<(steps_90deg*2 + steps_offset);j++)
			{
				PFout(1) = 1;
				delay_us2(delay_count);
				PFout(1) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == -12)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(0) = 1;
			for(j=0;j<(steps_90deg*2 + steps_offset);j++)
			{
				PFout(1) = 1;
				delay_us2(delay_count);
				PFout(1) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == 22)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(2) = 0;
			for(j=0;j<(steps_90deg*2 + steps_offset);j++)
			{
				PFout(3) = 1;
				delay_us2(delay_count);
				PFout(3) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == -22)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(2) = 1;
			for(j=0;j<(steps_90deg*2 + steps_offset);j++)
			{
				PFout(3) = 1;
				delay_us2(delay_count);
				PFout(3) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == 32)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(4) = 0;
			for(j=0;j<(steps_90deg*2 + steps_offset);j++)
			{
				PFout(5) = 1;
				delay_us2(delay_count);
				PFout(5) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == -32)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(4) = 1;
			for(j=0;j<(steps_90deg*2 + steps_offset);j++)
			{
				PFout(5) = 1;
				delay_us2(delay_count);
				PFout(5) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == 42)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(6) = 0;
			for(j=0;j<(steps_90deg*2 + steps_offset);j++)
			{
				PFout(7) = 1;
				delay_us2(delay_count);
				PFout(7) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == -42)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(6) = 1;
			for(j=0;j<(steps_90deg*2 + steps_offset);j++)
			{
				PFout(7) = 1;
				delay_us2(delay_count);
				PFout(7) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == 52)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(8) = 0;
			for(j=0;j<(steps_90deg*2 + steps_offset);j++)
			{
				PFout(9) = 1;
				delay_us2(delay_count);
				PFout(9) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == -52)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(8) = 1;
			for(j=0;j<(steps_90deg*2 + steps_offset);j++)
			{
				PFout(9) = 1;
				delay_us2(delay_count);
				PFout(9) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == 62)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(10) = 0;
			for(j=0;j<(steps_90deg*2 + steps_offset);j++)
			{
				PFout(11) = 1;
				delay_us2(delay_count);
				PFout(11) = 0;
				delay_us2(delay_count);
			}
		}
		if(Restore_step[i] == -62)
		{
			motor_enable(my_abs(Restore_step[i])/10);
			PFout(10) = 1;
			for(j=0;j<(steps_90deg*2 + steps_offset);j++)
			{
				PFout(11) = 1;
				delay_us2(delay_count);
				PFout(11) = 0;
				delay_us2(delay_count);
			}
		}
		delay_ms(150);
		i++;
	}
	
	motor_enable(0);
}

void motor_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	/*********端口启动**************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|
																GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
 	GPIO_Init(GPIOF, &GPIO_InitStructure);
	/******************************************************/
	/*定时器3产生10000Hz基准定时器中断****************************/
	TIM_TimeBaseInitStructure.TIM_Period = 99; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=83;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=DISABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
}

void motor_pin_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF, ENABLE);
	/*********端口启动**************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|
																GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
 	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	/******************************************************/
	
	motor_enable(0);
	
	PFout(0) = 0;
	PFout(1) = 0;
	PFout(2) = 0;
	PFout(3) = 0;
	PFout(4) = 0;
	PFout(5) = 0;
	PFout(7) = 0;
	PFout(8) = 0;
	PFout(9) = 0;
	PFout(10) = 0;
	PFout(11) = 0;
	
	delay_ms(1);
}

void motor_tim_Init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	/*定时器3产生10000Hz基准定时器中断****************************/
	TIM_TimeBaseInitStructure.TIM_Period = 99; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=83;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=DISABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
}

