#ifndef _CUMT_FILTER_H
#define _CUMT_FILTER_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for function. Do not edit comment generated here */

#define M_PI  (float)3.1415926535

float CUMT_LPF_1st(float X_new, float *X_last, float lpf); //Low Pass Filter
int16_t CUMT_Skip_Spike(int16_t data_new, int16_t data_last, int16_t tolerability);
float CUMT_Kalman_Filter(float X_new, float *X_last, float Q, float R, float *P, float *Kg);
float CUMT_LPF_2nd(float b0, float a1, float a2, float lastout, float preout, float newData);
float CUMT_IIR_Butterworth_LPF(float data_in, float *x, float *y, float *a, float *b, uint8_t order);

/* End user code. Do not edit comment generated here */
#endif