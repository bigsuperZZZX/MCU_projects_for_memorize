
#include "r_cg_macrodriver.h"
#include "CUMT_motor.h"
#include "r_cg_mtu3.h"
#include "CUMT_math.h"
#include "CUMT_RC.h"
#include "CUMT_control.h"
#include "CUMT_IMU.h"

static float k_alpha = 0;

volatile uint16_t Motor_pwm1 = 0;
volatile uint16_t Motor_pwm2 = 0;
volatile uint16_t Motor_pwm3 = 0;
volatile uint16_t Motor_pwm4 = 0; // final output

static uint16_t motor_drv1 = 0;
static uint16_t motor_drv2 = 0;
static uint16_t motor_drv3 = 0;
static uint16_t motor_drv4 = 0;

const uint16_t motor_offset1 = 0;
const uint16_t motor_offset2 = 0;
const uint16_t motor_offset3 = 0;
const uint16_t motor_offset4 = 0;

extern volatile int16_t Ctrl_drv_rol, Ctrl_drv_pit, Ctrl_drv_yaw;
extern volatile int16_t Ctrl_drv_alt; 

extern uint8_t action_cmd;

uint16_t Keep_Alt_thr = THR_BASE_VALUE;


void CUMT_Motor_Init(void)
{

}

/**************************************
motor config:

		2	^	1
			X
		3		4

1, 3 anticlockwise	
2, 4 clockwise
***************************************/
void CUMT_Motor_PWM_Manual(void)
{
	motor_drv1 = RC_thr - Ctrl_drv_rol + Ctrl_drv_pit - Ctrl_drv_yaw + motor_offset1;
	motor_drv2 = RC_thr + Ctrl_drv_rol + Ctrl_drv_pit + Ctrl_drv_yaw + motor_offset2;
	motor_drv3 = RC_thr + Ctrl_drv_rol - Ctrl_drv_pit - Ctrl_drv_yaw + motor_offset3; 
	motor_drv4 = RC_thr - Ctrl_drv_rol - Ctrl_drv_pit + Ctrl_drv_yaw + motor_offset4;
}

void CUMT_Motor_PWM_Throttle_Compensation(void)
{

}

void CUMT_Motor_PWM_KeepAltitude(void)
{
	motor_drv1 = Ctrl_drv_alt - Ctrl_drv_rol + Ctrl_drv_pit - Ctrl_drv_yaw + motor_offset1;
	motor_drv2 = Ctrl_drv_alt + Ctrl_drv_rol + Ctrl_drv_pit + Ctrl_drv_yaw + motor_offset2;
	motor_drv3 = Ctrl_drv_alt + Ctrl_drv_rol - Ctrl_drv_pit - Ctrl_drv_yaw + motor_offset3; 
	motor_drv4 = Ctrl_drv_alt - Ctrl_drv_rol - Ctrl_drv_pit + Ctrl_drv_yaw + motor_offset4;
}

void CUMT_PWM_Output_Direct(void)
{
	signed int temp;
	
//	Ctrl_drv_alt = CUMT_LIMIT(Ctrl_drv_alt, RC_THR_MIN, RC_THR_MAX);
	/*Ctrl_drv_rol = CUMT_LIMIT(Ctrl_drv_rol, 1350, 1650);
	Ctrl_drv_pit = CUMT_LIMIT(Ctrl_drv_pit, 1350, 1650);
	Ctrl_drv_yaw = CUMT_LIMIT(Ctrl_drv_yaw, 1450, 1550);*/
	
	temp = (Ctrl_drv_alt + Keep_Alt_thr);
	if(temp>1950) temp = 1950;
	if(temp<1020) temp = 1020;
		
	if(Ctrl_drv_rol > ROLL_BASE_VALUE + ROL_PIT_ALWD_OFST) Ctrl_drv_rol =  ROLL_BASE_VALUE + ROL_PIT_ALWD_OFST;
	if(Ctrl_drv_rol < ROLL_BASE_VALUE - ROL_PIT_ALWD_OFST) Ctrl_drv_rol =  ROLL_BASE_VALUE - ROL_PIT_ALWD_OFST;
		
	if(Ctrl_drv_pit > ROLL_BASE_VALUE + ROL_PIT_ALWD_OFST) Ctrl_drv_pit = ROLL_BASE_VALUE + ROL_PIT_ALWD_OFST;
	if(Ctrl_drv_pit < ROLL_BASE_VALUE - ROL_PIT_ALWD_OFST) Ctrl_drv_pit = ROLL_BASE_VALUE - ROL_PIT_ALWD_OFST;
		
	if(action_cmd == UNLOCK || action_cmd == LOCK)  //解锁和锁定阶段允许偏航值很大，否则不允许
	{
		if(Ctrl_drv_yaw>1980) Ctrl_drv_yaw = 1980;
		if(Ctrl_drv_yaw<1020) Ctrl_drv_yaw = 1020;
	}
	else
	{
		if(Ctrl_drv_yaw > YAW_BASE_VALUE + YAW_ALWD_OFST) Ctrl_drv_yaw =  YAW_BASE_VALUE + YAW_ALWD_OFST;
		if(Ctrl_drv_yaw < YAW_BASE_VALUE - YAW_ALWD_OFST) Ctrl_drv_yaw =  YAW_BASE_VALUE - YAW_ALWD_OFST;
	}
	
	PWM_OUT_CH1 = Ctrl_drv_rol*10/4;
	PWM_OUT_CH2 = Ctrl_drv_pit*10/4;	// f/16:  dt_counter = 0.4us
	PWM_OUT_CH3 = temp*10/4;
	//PWM_OUT_CH3 = 2000*10/4;
	PWM_OUT_CH4 = Ctrl_drv_yaw*10/4;
	
}

void CUMT_Motor_PWM_Output(void)
{
	motor_drv1 = CUMT_LIMIT(motor_drv1, RC_THR_MIN, RC_THR_MAX);
	motor_drv2 = CUMT_LIMIT(motor_drv2, RC_THR_MIN, RC_THR_MAX);
	motor_drv3 = CUMT_LIMIT(motor_drv3, RC_THR_MIN, RC_THR_MAX);
	motor_drv4 = CUMT_LIMIT(motor_drv4, RC_THR_MIN, RC_THR_MAX);

	if(is_Armed)
	{
		if(RC_thr>RC_THR_MIN+50)
		{
			Motor_pwm1 = motor_drv1;
			Motor_pwm2 = motor_drv2;
			Motor_pwm3 = motor_drv3;
			Motor_pwm4 = motor_drv4;
		}
		else
		{
			Motor_pwm1 = RC_thr + 50;
			Motor_pwm2 = RC_thr + 50;
			Motor_pwm3 = RC_thr + 50;
			Motor_pwm4 = RC_thr + 50;
		}
	}
	else
	{
		Motor_pwm1 = RC_THR_MIN;
		Motor_pwm2 = RC_THR_MIN;
		Motor_pwm3 = RC_THR_MIN;
		Motor_pwm4 = RC_THR_MIN;
	}
	
	PWM_OUT_CH1 = Motor_pwm1*10/4;	// f/16:  dt_counter = 0.4us
	PWM_OUT_CH2 = Motor_pwm2*10/4;
	PWM_OUT_CH3 = Motor_pwm3*10/4;
	PWM_OUT_CH4 = Motor_pwm4*10/4;
}
