#ifndef _CUMT_MOTOR_H
#define _CUMT_MOTOR_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for function. Do not edit comment generated here */
//#define MULTIPLE 4
#define PWM_OUT_CH1	MTU0.TGRD
#define PWM_OUT_CH2	MTU0.TGRC
#define PWM_OUT_CH3	MTU0.TGRB
#define PWM_OUT_CH4	MTU1.TGRB

extern volatile uint16_t Motor_pwm1, Motor_pwm2, Motor_pwm3, Motor_pwm4;

void CUMT_Motor_Init(void);
void CUMT_Motor_PWM_Manual(void);
void CUMT_Motor_PWM_Throttle_Compensation(void);
void CUMT_Motor_PWM_KeepAltitude(void);
void CUMT_Motor_PWM_Output(void);
void CUMT_PWM_Output_Direct(void);
/* End user code. Do not edit comment generated here */
#endif
