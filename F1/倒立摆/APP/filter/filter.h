#ifndef _FILTER_H
#define _FILTER_H
#include "stm32f10x.h"

#define N 5

void PID_INIT(void);
float Incremental_PID(float);
int Position_PID (int Encoder,int Target);


#endif 
