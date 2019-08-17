#ifndef _motor_h_
#define _motor_h_

#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_it.h"

#define UP 0     //爪子电机方向选择
#define DOWN 1
#define UP_NUM 0   //爪子电机上下处对应的值，初始值0为滑块在最上方
#define DOWN_NUM 256
#define TOP_MOTOR_PORT GPIOB
#define TOP_MOTOR_OFFSET 8

#define M1I1_PORT GPIOA
#define M1I1_PIN  GPIO_Pin_2
#define M1I2_PORT GPIOA
#define M1I2_PIN  GPIO_Pin_3
#define M1EN_PORT GPIOA
#define M1EN_PIN  GPIO_Pin_6

#define M2I1_PORT GPIOA
#define M2I1_PIN  GPIO_Pin_8
#define M2I2_PORT GPIOA
#define M2I2_PIN  GPIO_Pin_11
#define M2EN_PORT GPIOA
#define M2EN_PIN  GPIO_Pin_12

#define M3I1_PORT GPIOB
#define M3I1_PIN  GPIO_Pin_3
#define M3I2_PORT GPIOB
#define M3I2_PIN  GPIO_Pin_4
#define M3EN_PORT GPIOB
#define M3EN_PIN  GPIO_Pin_5

#define M4I1_PORT GPIOB
#define M4I1_PIN  GPIO_Pin_6
#define M4I2_PORT GPIOB
#define M4I2_PIN  GPIO_Pin_7
#define M4EN_PORT GPIOB
#define M4EN_PIN  GPIO_Pin_12

#define M5I1_PORT GPIOB
#define M5I1_PIN  GPIO_Pin_13
#define M5I2_PORT GPIOB
#define M5I2_PIN  GPIO_Pin_14
#define M5EN_PORT GPIOB
#define M5EN_PIN  GPIO_Pin_15

#define KEY_PORT GPIOC
#define KEY_PIN GPIO_Pin_13

//PA0、PA1-左舵机、右舵机
//PA2\PA3\PA6-电机1的IN1,IN2,EN
//PA8\PA11\PA12-电机2的IN1,IN2,EN
//PB3\PB4\PB5-电机3的IN1,IN2,EN
//PB6\PB7\PB12-电机4的IN1,IN2,EN
//PB13\PB14\PB15-电机5的IN1,IN2,EN
//PB8\PB9\PB10\PB11-顶部电机的IN1\IN2\IN3\IN4

typedef struct
{
	int SetState;     //设定状态  Set
	int CurState;     //当前状态  Current
	int SetSpeed;     //5-15000;
}StepMotor_TypeDef;

//功能：电机驱动初始化，然后TIM1和TIM2都在这里被用掉了
void motor_Init(void);
//功能：控制大转盘转到指定角度
//输入：-180到+180，对应一整圈
//输出：输入值不符合规定，返回1，否则0
char set_top_angle(signed int angle); 
//功能：指定某个爪子电机上升或是下降
//输入：motor_num：电机编号，1—5；action：UP或者DOWN
//输出：懒得写，恒为零
char set_paw_act(char motor_num, char action);
//功能：获取某个电机是否在运转
//输入：motor_num：电机编号，0-5，0是上方电机
//输出：1：正在运行；0：停止运行
char test_running(char motor_num);
void set_speed_L(u8 speed); //左轮速度，0-25向前，25-50向后，越靠近25，转速越慢
void set_speed_R(u8 speed); //右轮速度，同上
unsigned char read_key(void);  //按下超过15ms，将持续输出1，再次按下超过15ms，将持续输出0




void all_motor_stop(void);

#endif


