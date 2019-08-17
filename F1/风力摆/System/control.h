#ifndef __control_H
#define __control_H 
#include "stm32f10x.h"

#define H (48)
#define conver 57.29578
#define RAD_10 0.174533

u16 base(float a);

void FixPoint(void);
void circle_PointMove(float a);
void line_PointMove(void);
void number_PointMove(void);

typedef struct {
	float x[7];
	float y[7];
}Number_Struct;

#endif

