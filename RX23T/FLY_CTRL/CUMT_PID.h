#ifndef _CUMT_PID_H
#define _CUMT_PID_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for function. Do not edit comment generated here */
extern volatile float kP_p, kP_q, kP_r, kP_w;
extern volatile float kI_p, kI_q, kI_r, kI_w;
extern volatile float kD_p, kD_q, kD_r, kD_w;
extern volatile float kP_pit, kP_rol, kP_yaw;
extern volatile float kI_pit, kI_rol, kI_yaw;
extern volatile float kD_pit, kD_rol, kD_yaw;
extern volatile float kP_alt, kI_alt, kD_alt;
extern volatile float kP_pos, kI_pos, kD_pos;

int16_t CUMT_PID_Get_PD(float kp,float kd,float err,float err_last,float deri,float deri_last,float dt);
int16_t CUMT_PID_Get_PID(float kp,float ki,float kd,float err,float err_last,float inte,float imax,float deri,float deri_last,float dt);

void CUMT_PID_Set_Pitch(float kp_pit, float kp_q, float ki_q, float kd_q);
void CUMT_PID_Set_Roll(float kp_rol, float kp_p, float ki_p, float kd_p);
void CUMT_PID_Set_Yaw(float kp_yaw, float kp_r, float ki_r, float kd_r);
void CUMT_PID_Set_Altitude(float kp_alt, float kp_w, float ki_w, float kd_w);
void CUMT_PID_Set_Position(float kp_pos, float ki_pos, float kd_pos);

/* End user code. Do not edit comment generated here */
#endif