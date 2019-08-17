#include "stm32f10x.h"
#include "sys.h"
#include "control.h"	
#include "filter.h"	
#include "oled.h"	
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
u8 Way_Angle=1;                             //获取角度的算法，1：四元数  2：卡尔曼  3：互补滤波 
u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right; //蓝牙遥控相关的变量
u8 Flag_Stop=1,Flag_Show=0;                 //停止标志位和 显示标志位 默认停止 显示打开
int Encoder_Left,Encoder_Right;             //左右编码器的脉冲计数
int Moto1,Moto2;                            //电机PWM变量 应是Motor的 向Moto致敬	
int Temperature;                            //显示温度
int Voltage;                                //电池电压采样相关的变量
float Angle_Balance,Gyro_Balance,Gyro_Turn; //平衡倾角 平衡陀螺仪 转向陀螺仪
float Show_Data_Mb;                         //全局显示变量，用于显示需要查看的数据
u32 Distance;                               //超声波测距
u8 delay_50,delay_flag,Bi_zhang=0;         //默认情况下，不开启避障功能，长按用户按键2s以上可以进入避障模式
float Acceleration_Z;                      //Z轴加速度计  
extern long pulse_r,pulse_l;
extern char dir_r,dir_l;   //0是前，1是后  右电机绿线中短线，下降沿进入，白线为低则为前，为高则为后 // 左 白 下 绿 低 前 高 后
extern int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
extern u8 Flag_Target;
extern signed char count;
extern signed int l_pulse,r_pulse;
extern long pulse_r,pulse_l;
extern char dir_r,dir_l;   //0是前，1是后  右电机绿线中短线，下降沿进入，白线为低则为前，为高则为后 // 左 白 下 绿 低 前 高 后
extern	int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
extern float Pitch,Roll,zhongzhi; 
signed char count2=0;

void symbol_key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

int main(void)
  { 
		Flag_Stop =0 ;
		delay_init();	    	            //=====延时函数初始化	
		uart_init(128000);	            //=====串口初始化为
		delay_ms(1800);
    MiniBalance_Motor_Init();   //=====初始化PWM 10KHZ，用于驱动电机 如需初始化电调接口 
    encoder_ExtiInit();
    IIC_Init();                     //=====IIC初始化
		delay_ms(200);    
		MPU6050_initialize();           //=====MPU6050初始化	
    DMP_Init();                     //=====初始化DMP 
    OLED_Init();                    //=====OLED初始化	   
	  MiniBalance_EXTI_Init();        //=====MPU6050 5ms定时中断初始化
		symbol_key_init();
    while(1)
	   {
			  if(count%6 == 5)
				{
					count = 0;
					GPIO_SetBits(GPIOB,GPIO_Pin_2);
//					zhongzhi += (float)(pulse_r+pulse_l)/50;
//					if(zhongzhi > 2)  zhongzhi=2;
//					if(zhongzhi < -2)  zhongzhi=-2;
					Balance_Pwm =balance(Angle_Balance,Gyro_Balance);                   //===平衡PID控制	
	  		  Velocity_Pwm=velocity(pulse_l,pulse_r);                  //===速度环PID控制	 记住，速度反馈是正反馈，就是小车快的时候要慢下来就需要再跑快一点
	//  	  Turn_Pwm    =turn(Encoder_Left,Encoder_Right,Gyro_Turn);            //===转向环PID控制
					Moto1=Balance_Pwm+Velocity_Pwm-Turn_Pwm;                            //===计算左轮电机最终PWM
					Moto2=Balance_Pwm+Velocity_Pwm+Turn_Pwm;                            //===计算右轮电机最终PWM
					Xianfu_Pwm();                                                       //===PWM限幅
	//				if(Pick_Up(Acceleration_Z,Angle_Balance,Encoder_Left,Encoder_Right))//===检查是否小车被那起
	//				if(Put_Down(Angle_Balance,Encoder_Left,Encoder_Right))              //===检查是否小车被放下
					Turn_Off(Angle_Balance,Voltage);                              //===如果不存在异常
					Set_Pwm(Moto1,Moto2);					//===赋值给PWM寄存器
					
					count2++;
					if(count2==4)
					{
						count2=0;
						pulse_r = pulse_l =0;	
					}
						
					if(count%20 == 0)
					{
						OLED_ShowFloat(0,0,Pitch,6,16);
						OLED_ShowFloat(0,2,zhongzhi,6,16);
						OLED_ShowNum(80,0,pulse_r,6,16);
						OLED_ShowNum(80,2,pulse_l,6,16);
//						OLED_ShowNum(80,4,dir_r,6,16);
//						OLED_ShowNum(80,6,dir_l,6,16);
					}
					GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				}
	   } 
}

