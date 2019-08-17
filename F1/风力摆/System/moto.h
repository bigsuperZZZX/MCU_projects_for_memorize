#ifndef __moto_H
#define __moto_H 
#include "stm32f10x.h"

typedef struct
{
	float CurPos;
	float PrevPos;
	u16 PWM;
}MotoTypeDef;

typedef struct
{
	float SumError;
	float LastError;
	float Proportion;
	float Integral;
	float Derivative;	
}PidTypeDef;

void moto_init(void);

void pid_config(void);

void pid_cal(void);

u16 cal_x(void);
u16 cal_y(void);

#endif
