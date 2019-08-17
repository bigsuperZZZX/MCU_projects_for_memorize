#ifndef _CUMT_MATH_H
#define _CUMT_MATH_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include <math.h>
/* Start user code for function. Do not edit comment generated here */
float fast_atan2(float y, float x) ;
float CUMT_invSqrt(float x);
float CUMT_ABS(float number);
int16_t CUMT_MAX(int16_t x1, int16_t x2);
int16_t CUMT_MIN(int16_t x1, int16_t x2);
float CUMT_LIMIT(float x_in, float min, float max);


/* End user code. Do not edit comment generated here */
#endif