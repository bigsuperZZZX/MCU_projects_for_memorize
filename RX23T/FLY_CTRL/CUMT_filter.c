#include "CUMT_filter.h"
#include <math.h>
#include "CUMT_math.h"


float CUMT_LPF_1st(float X_new, float *X_last, float lpf) //Weighted Average Filter
{
	float X_out;
	X_out = lpf*X_new + (1 - lpf)*(*X_last);
	*X_last = X_out;
	return X_out;
}

int16_t CUMT_Skip_Spike(int16_t data_new, int16_t data_last, int16_t tolerability)
{
	uint16_t diff = 0;
	diff = CUMT_ABS(data_new - data_last);
	if(diff>tolerability)
	{
		return data_last;
	}
	else
	{
		return data_new;
	}
}

float CUMT_Kalman_Filter(float X_new, float *X_last, float Q, float R, float *P, float *Kg)
{
	float X_out;
	*P = *P + Q;
	*Kg = *P/(*P + R);
	X_out = *X_last + (*Kg)*(X_new - *X_last);
	*P = (1 - *Kg)*(*P);
	*X_last = X_out;
	return X_out;
}

void CUMT_LPF_2nd_Factor_Cal(float dt, float Fcut, float* lpf_data)
{
	float a = 1/(2*M_PI*Fcut*dt);
	lpf_data[0] = 1/(a*a + 3*a + 1);
	lpf_data[1] = (2*a*a + 3*a)/(a*a + 3*a + 1);
	lpf_data[2] = (a*a)/(a*a + 3*a + 1);
}

float CUMT_LPF_2nd(float b0, float a1, float a2, float lastout, float preout, float newData)
{
	float lpf_2nd_data;
	
	lpf_2nd_data = newData * b0 + lastout * a1 - preout * a2;
	preout=lastout;
	lastout = lpf_2nd_data;
	return lpf_2nd_data;
}

float test_xxxx[5],test_z1,test_z2,test_yyyyy[5],testyyyy;

// Fs = 200Hz
//Fcut = 30Hz

float CUMT_IIR_Butterworth_LPF(float data_in, float *x, float *y, float *a, float*b, uint8_t order)
{
    float z1 = 0;
	float z2 = 0;
	uint8_t na = order + 1;
	uint8_t nb = order + 1;
    int i;
    float data_out;

    for(i=na-1;i>0;i--) 
	{
		x[i] = x[i-1]; 
	}
    x[0] = data_in; 
/*	
	for(i=0;i<5;i++)
	{
		test_xxxx[i] = x[i];
	}
*/
    for(i=0;i<nb;i++)
	{
		z1 += b[i]*x[i];
	}
//	test_z1 = z1;
	
    for(i=nb-1;i>0;i--)
    {
		y[i] = y[i-1];
	}

    for(i=1;i<na;i++)
	{
		z2 += a[i]*y[i];
	}
    y[0] = z1 - z2;
    data_out = y[0];

    return data_out;
}
