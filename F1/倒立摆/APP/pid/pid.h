
#ifndef _PID_H
#define _PID_H
#include "stm32f10x.h"
#include "public.h"


void PID_INIT(void);
float Incremental_PID(float,float);
int Moto_Reflash(int nowpoint);
float  Posi_PID(float nowpoint,float Setpoint);
	


#endif 
