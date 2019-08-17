
#include "r_cg_macrodriver.h"
#include "CUMT_PID.h"
#include "CUMT_math.h"
#include <math.h>

// Examples for _filter:
// f_cut = 10 Hz -> _filter = 15.9155e-3
// f_cut = 15 Hz -> _filter = 10.6103e-3
// f_cut = 20 Hz -> _filter =  7.9577e-3
// f_cut = 25 Hz -> _filter =  6.3662e-3
// f_cut = 30 Hz -> _filter =  5.3052e-3
const float filter = 7.9577e-3; //  1/(2*PI*f_cut)


volatile float kP_p = 0, kP_q = 0, kP_r = 0, kP_w = 0;
volatile float kI_p = 0, kI_q = 0, kI_r = 0, kI_w = 0;
volatile float kD_p = 0, kD_q = 0, kD_r = 0, kD_w = 0;

volatile float kP_pit = 0, kP_rol = 0, kP_yaw = 0;
volatile float kI_pit = 0, kI_rol = 0, kI_yaw = 0;
volatile float kD_pit = 0, kD_rol = 0, kD_yaw = 0;
volatile float kP_alt = 0, kI_alt = 0, kD_alt = 0;
volatile float kP_pos = 0, kI_pos = 0, kD_pos = 0;


float CUMT_PID_Get_P(float kp, float err)
{
	return kp*err;
}

float CUMT_PID_Get_I(float ki, float err, float inte, float imax, float dt)
{
	if(ki != 0)
	{
		inte += err*ki*dt;
		inte = CUMT_LIMIT(inte, -imax, imax);
		return inte;
	}
	return 0;
}

float CUMT_PID_Get_D(float kd, float err, float err_last, float deri, float deri_last, float dt)
{
	if(kd != 0)
	{
		deri = (err - err_last)/dt;
		deri = deri_last + (dt/(filter + dt))*(deri - deri_last);
		err_last = err;
		deri_last = deri;
		return kd*deri;
	}
	return 0;
}

int16_t CUMT_PID_Get_PD(float kp,float kd,float err,float err_last,float deri,float deri_last,float dt)
{
	float temp = 0;
	temp += CUMT_PID_Get_P(kp, err);
	temp += CUMT_PID_Get_D(kd, err, err_last, deri, deri_last, dt);
	return (int16_t) temp;
}

int16_t CUMT_PID_Get_PID(float kp,float ki,float kd,float err,float err_last,float inte,float imax,float deri,float deri_last,float dt)
{
	float temp = 0;
	temp += CUMT_PID_Get_P(kp, err);
	temp += CUMT_PID_Get_I(ki, err, inte, imax, dt);
	temp += CUMT_PID_Get_D(kd, err, err_last, deri, deri_last, dt);
	return (int16_t) temp;
}

void CUMT_PID_Set_Pitch(float kp_pit, float kp_q, float ki_q, float kd_q)
{
	kP_pit = kp_pit;
	kP_q = kp_q;
	kI_q = ki_q;
	kD_q = kd_q;
}

void CUMT_PID_Set_Roll(float kp_rol, float kp_p, float ki_p, float kd_p)
{
	kP_rol = kp_rol;
	kP_p = kp_p;
	kI_p = ki_p;
	kD_p = kd_p;
}

void CUMT_PID_Set_Yaw(float kp_yaw, float kp_r, float ki_r, float kd_r)
{
	kP_yaw = kp_yaw;
	kP_r = kp_r;
	kI_r = ki_r;
	kD_r = kd_r;
}

void CUMT_PID_Set_Altitude(float kp_alt, float kp_w, float ki_w, float kd_w)
{
	kP_alt = kp_alt;
	kP_w = kp_w;
	kI_w = ki_w;
	kD_w = kd_w;
}

void CUMT_PID_Set_Position(float kp_pos, float ki_pos, float kd_pos)
{
	kP_pos = kp_pos;
	kI_pos = ki_pos;
	kD_pos = kd_pos;
}