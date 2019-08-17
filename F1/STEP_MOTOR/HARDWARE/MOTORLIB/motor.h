#ifndef _motor_h_
#define _motor_h_

#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_it.h"

#define UP 0     //צ�ӵ������ѡ��
#define DOWN 1
#define UP_NUM 0   //צ�ӵ�����´���Ӧ��ֵ����ʼֵ0Ϊ���������Ϸ�
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

//PA0��PA1-�������Ҷ��
//PA2\PA3\PA6-���1��IN1,IN2,EN
//PA8\PA11\PA12-���2��IN1,IN2,EN
//PB3\PB4\PB5-���3��IN1,IN2,EN
//PB6\PB7\PB12-���4��IN1,IN2,EN
//PB13\PB14\PB15-���5��IN1,IN2,EN
//PB8\PB9\PB10\PB11-���������IN1\IN2\IN3\IN4

typedef struct
{
	int SetState;     //�趨״̬  Set
	int CurState;     //��ǰ״̬  Current
	int SetSpeed;     //5-15000;
}StepMotor_TypeDef;

//���ܣ����������ʼ����Ȼ��TIM1��TIM2�������ﱻ�õ���
void motor_Init(void);
//���ܣ����ƴ�ת��ת��ָ���Ƕ�
//���룺-180��+180����Ӧһ��Ȧ
//���������ֵ�����Ϲ涨������1������0
char set_top_angle(signed int angle); 
//���ܣ�ָ��ĳ��צ�ӵ�����������½�
//���룺motor_num�������ţ�1��5��action��UP����DOWN
//���������д����Ϊ��
char set_paw_act(char motor_num, char action);
//���ܣ���ȡĳ������Ƿ�����ת
//���룺motor_num�������ţ�0-5��0���Ϸ����
//�����1���������У�0��ֹͣ����
char test_running(char motor_num);
void set_speed_L(u8 speed); //�����ٶȣ�0-25��ǰ��25-50���Խ����25��ת��Խ��
void set_speed_R(u8 speed); //�����ٶȣ�ͬ��
unsigned char read_key(void);  //���³���15ms�����������1���ٴΰ��³���15ms�����������0




void all_motor_stop(void);

#endif


