
#include "r_cg_macrodriver.h"
#include "CUMT_control.h"
#include "CUMT_PID.h"
#include "CUMT_RC.h"
#include "CUMT_motor.h"
#include "CUMT_sonar.h"
#include "CUMT_MPU6050.h"
#include "CUMT_math.h"
#include "CUMT_IMU.h"
#include "CUMT_Delay.h"
#include "CUMT_UART.h"
#include <math.h>

volatile uint8_t Fly_mode = MODE_MANUAL;
volatile uint8_t is_Armed = 0;

volatile float Yaw_tgt = 0;
volatile float Pit_tgt = 0;
volatile float Rol_tgt = 0;
volatile float Alt_tgt = 0;

volatile int16_t Ctrl_drv_rol = 1500;
volatile int16_t Ctrl_drv_pit = 1500;
volatile int16_t Ctrl_drv_yaw = 1500;
volatile int16_t Ctrl_drv_alt = 0;

const int16_t alt_drv_max = 1800;
const int16_t alt_drv_min = 1200; // need test on the cardan shaft

//static float Pit_tgt = 0;
//static float Rol_tgt = 0;
//static float yaw_tgt = 0;
static float p_tgt = 0;
static float q_tgt = 0;
static float r_tgt = 0;
static float w_tgt = 0;
//static float alt_tgt = 0;
//static float pos_x_tgt = 0;
//static float pos_y_tgt = 0;

static float p = 0, q = 0, r = 0;  //rotation rate by x, y, z axises	unit: deg/s
static float w = 0; // vertical velocity, up+, down-	unit: m/s

static float pit_err = 0;
static float rol_err = 0;
static float yaw_err = 0;
static float alt_err = 0;
static float last_alt_err = 0;
static float sum_alt_err = 0;
static float p_err = 0;
static float q_err = 0;
static float r_err = 0;
static float w_err = 0;
static float p_err_last = 0;
static float q_err_last = 0;
static float r_err_last = 0;
static float w_err_last = 0;
static float sum_rol_err = 0;
static float last_rol_err = 0;
static float sum_pit_err = 0;
static float last_pit_err = 0;
static float sum_yaw_err = 0;
static float last_yaw_err = 0;

uint8_t auto_rol_pit_switch = 1;

//static float alt_err = 0;
//static float alt_err_last = 0;

/* position control should be added in the flight guider board
static float pos_x_err = 0;
static float pos_y_err = 0;
static float pos_x_err_last = 0;
static float pos_y_err_last = 0;
*/
static float p_inte = 0;
static float q_inte = 0;
static float r_inte = 0;
static float w_inte = 0;
static float p_deri = 0;
static float q_deri = 0;
static float r_deri = 0;
static float w_deri = 0;
static float p_deri_last = 0;	// for filtering
static float q_deri_last = 0;
static float r_deri_last = 0;
static float w_deri_last = 0;
//static float alt_inte = 0;

const float dt_200Hz = 0.005;  // unit: s
const float dt_100Hz = 0.01; 
const float dt_67Hz = 0.015;  
const float dt_40Hz = 0.025; 
const float dt_29Hz = 0.035; 
const float dt_18Hz = 0.055; 

const float imax = 100;  // limit integration

const float angle_gain = 0.025;
const float r_gain = 0.1;

const float anlge_limit = 8;
const float rate_limit = 1000;	// ? need test here!

const float err_anlge_limit = 10; // unit: deg
const float err_rate_limit = 1000;	// unit: deg/s
const float err_alt_limit = 100;	// uint: mm

extern uint16_t Keep_Alt_thr;

//static uint8_t rol_mid = 60;
//static uint8_t pit_mid = 60;
//static signed char yaw_mid = 0;
uint8_t roll_mid = 60, pitch_mid = 60;
float yaw_mid = 0.0;

extern volatile uint8_t rol_position;  //中点值
extern volatile uint8_t pit_position;  //中点值
extern volatile signed int yaw_position;  //中点值

extern volatile uint8_t uart5_rx_cplt; 

extern volatile uint8_t uart1_rx_data[50];
extern volatile uint8_t uart5_rx_data[20];

uint8_t action_cmd = LOCK;
/*#define TAKE0FF 0
#define LANDING 1
#define TRACK   2
#define UNLOCK  3
#define LOCK    4
#define SET_TARGET 5
#define FREE 6
*/
uint8_t action_state = LOCK_CPLT;
/*#define TAKE0FF_CPLT 0
#define LANDING_CPLT 1
#define TRACK_CPLT   2
#define UNLOCK_CPLT 3
#define LOCK_CPLT    4
#define TARGET_SETTED 5 */
uint8_t delay_state = DELAY_CPLT;
/*#define DELAY_CPLT      0
#define DELAY_UNCPLT    1*/

uint8_t mode_select = NONE;
/*
#define NONE       0
#define MISSION_1  1
#define MISSION_2  2
#define MISSION_3  3
#define MISSION_4  4
#define MISSION_5  5
#define MISSION_6  6
#define MISSION_7  7
#define MISSION_8  8
*/

/*
#define COLOR 0xB0
#define BLACK 0xB1
*/

uint8_t fist_call_flag = 1;

void CUMT_Ctrl_Check_Arm(void)
{
	static uint8_t arm_cnt = 0;
	if(RC_thr>(RC_THR_MIN-10)&&RC_thr<(RC_THR_MIN+10)&&RC_yaw>(RC_YAW_MIN-10)&&RC_yaw<(RC_YAW_MIN+10))
	{
		arm_cnt++;
		if(arm_cnt>10) //hold for 3s to disarm
		{
			arm_cnt = 0;
			is_Armed = 0; 
		}
	}
	else if(RC_thr>(RC_THR_MIN-10)&&RC_thr<(RC_THR_MIN+10)&&RC_yaw>(RC_YAW_MAX-10)&&RC_yaw<(RC_YAW_MAX+10))
	{
		arm_cnt++;
		if(arm_cnt>30)	//hold for 3s to arm
		{
			arm_cnt = 0;
			is_Armed = 1;	
		}
	}
	else
		arm_cnt = 0;
}


void CUMT_Ctrl_Check_Mode(void)
{
	if(RC_aux1<1400)
	{
		Fly_mode = MODE_MANUAL; // keep attitude
	}
	else if(RC_aux1>1600)
	{
		Fly_mode = MODE_KEEP_POS; // keep position
	}
	else 
	{
		Fly_mode = MODE_KEEP_ALT; //keep height
	}
}

void CUMT_Ctrl_Get_RC_Command(void)
{
	Pit_tgt = ((int16_t)RC_pit - 1500)*angle_gain;
	Rol_tgt = ((int16_t)RC_rol - 1500)*angle_gain;
	
	if(RC_thr>RC_THR_MIN+50)
	{
		Yaw_tgt += (((int16_t)RC_yaw - 1500)/10)*r_gain; // 20Hz , 50ms
	}
	
	if(Yaw_tgt>180)
	{
		Yaw_tgt -= 360;
	}
	else if(Yaw_tgt<-180)
	{
		Yaw_tgt += 360;
	}

	Pit_tgt = CUMT_LIMIT(Pit_tgt, -anlge_limit, anlge_limit);
	Rol_tgt = CUMT_LIMIT(Rol_tgt, -anlge_limit, anlge_limit);
}

float CUMT_Ctrl_Get_YawError(float target, float real)
{
	static float error = 0;
	error = target - real;
	if(error<(-180.0))
	{
		error += 360.0;
	}
	else if(error>180.0)
	{
		error -= 360.0;
	}
	return error;
}

void CUMT_Ctrl_Attitude_Outer_Loop(void)
{
	pit_err = Pit_tgt - Pitch;
	rol_err = Rol_tgt - Roll;
	yaw_err = CUMT_Ctrl_Get_YawError(Yaw_tgt, Yaw);
	
	pit_err = CUMT_LIMIT(pit_err, -err_anlge_limit, err_anlge_limit);
	rol_err = CUMT_LIMIT(rol_err, -err_anlge_limit, err_anlge_limit);
	yaw_err = CUMT_LIMIT(yaw_err, -err_anlge_limit, err_anlge_limit);

	q_tgt = kP_pit*pit_err;
	p_tgt = kP_rol*rol_err;
	r_tgt = kP_yaw*yaw_err;
	
	q_tgt = CUMT_LIMIT(q_tgt, -rate_limit, rate_limit);
	p_tgt = CUMT_LIMIT(p_tgt, -rate_limit, rate_limit);
	r_tgt = CUMT_LIMIT(r_tgt, -rate_limit, rate_limit);
}

void CUMT_Ctrl_Attitude_Inner_Loop(void)
{
	p = Acc_Gyro[3]*GYRO_GA;
	q = -Acc_Gyro[4]*GYRO_GA;
	r = Acc_Gyro[5]*GYRO_GA;
	
	p_err = p_tgt - p;
	q_err = q_tgt - q;
	r_err = r_tgt - r;
	
	p_err = CUMT_LIMIT(p_err, -err_rate_limit, err_rate_limit);
	q_err = CUMT_LIMIT(q_err, -err_rate_limit, err_rate_limit);
	r_err = CUMT_LIMIT(r_err, -err_rate_limit, err_rate_limit);
	
	//Ctrl_drv_rol = CUMT_PID_Get_PID(kP_p, kI_p, kD_p, p_err, p_err_last, p_inte, imax, p_deri, p_deri_last, dt_100Hz);
	//Ctrl_drv_pit = CUMT_PID_Get_PID(kP_q, kI_q, kD_q, q_err, q_err_last, q_inte, imax, q_deri, q_deri_last, dt_100Hz);
	Ctrl_drv_yaw = CUMT_PID_Get_PID(kP_r, kI_r, kD_r, r_err, r_err_last, r_inte, imax, r_deri, r_deri_last, dt_100Hz);

	/*if(Fly_mode == MODE_MANUAL)
	{
		CUMT_Motor_PWM_Manual();
	//	CUMT_Motor_PWM_Throttle_Compensation();
	}
	else
	{
		//CUMT_Motor_PWM_KeepAltitude();
		CUMT_Motor_PWM_Manual();
	}*/
	CUMT_PWM_Output_Direct();
	//CUMT_Motor_PWM_Output();
}

void CUMT_Ctrl_Keep_Altitude(void)
{
	int temp;
	w = ((float)Sonar_height - (float)Sonar_height_last)/(dt_29Hz*1000.0);	// unit: m/s
	Sonar_height_last = Sonar_height;
	alt_err = Alt_tgt - Sonar_height;
	alt_err = CUMT_LIMIT(alt_err, -err_alt_limit, err_alt_limit);
	w_tgt = kP_alt*alt_err;
	w_err = w_tgt - w;
	temp = CUMT_PID_Get_PID(kP_w, kI_w, kD_w, w_err, w_err_last, w_inte, imax, w_deri, w_deri_last, dt_29Hz);
	
	Ctrl_drv_alt += temp;
	Ctrl_drv_alt = CUMT_LIMIT(Ctrl_drv_alt, alt_drv_min, alt_drv_max);
}

void HIGH_Control(void)
{
	static float temp_P = 0, temp_I = 0, temp_D = 0;
	
//	Alt_tgt = 800;
	alt_err = (float)Alt_tgt - (float)Sonar_height;
	temp_P = kP_alt * alt_err;
	temp_I = kI_alt * sum_alt_err;
	temp_D = kD_alt * (alt_err - last_alt_err);
		if(temp_I>200)
	        	temp_I = 300;
	    	if(temp_I<-200)
	        	temp_I = -300;
    	Ctrl_drv_alt = temp_P + temp_I + temp_D;
		if(Ctrl_drv_alt>450)
			Ctrl_drv_alt = 450;
		if(Ctrl_drv_alt<-450)
		        Ctrl_drv_alt = -450;
    	sum_alt_err += alt_err;
		if(sum_alt_err>600)
		        sum_alt_err = 600;
		if(sum_alt_err<-600)
		        sum_alt_err = -600;
    	last_alt_err = alt_err;
}

void ROLL_Control(uint8_t rol_mid)
{
	static float temp_P = 0, temp_I = 0, temp_D = 0;
	
//	rol_mid = 60;
		if(rol_mid>105) rol_mid = 105;
		if(rol_mid<15) rol_mid = 15;
	rol_err = (float)rol_mid - (float)rol_position;
	temp_P = kP_rol * rol_err;
	temp_I = kI_rol * sum_rol_err;
	temp_D = kD_rol * (rol_err - last_rol_err);
		if(temp_I>200)
	        	temp_I = 300;
	    	if(temp_I<-200)
	        	temp_I = -300;
    	Ctrl_drv_rol = ROLL_BASE_VALUE+temp_P + temp_I + temp_D;
    	sum_rol_err += rol_err;
		if(sum_rol_err>1000)
		        sum_rol_err = 1000;
		if(sum_rol_err<-1000)
		        sum_rol_err = -1000;
    	last_rol_err = rol_err;
		
}


void PITCH_Control(uint8_t pit_mid)
{
	static float temp_P = 0, temp_I = 0, temp_D = 0;
	
//	pit_mid = 60;
		if(pit_mid>105) pit_mid = 105;
		if(pit_mid<15) pit_mid = 15;
	pit_err = (float)pit_mid - (float)pit_position;
	temp_P = kP_pit * pit_err;
	temp_I = kI_pit * sum_pit_err;
	temp_D = kD_pit * (pit_err - last_pit_err);
		if(temp_I>200) temp_I = 300;
	    	if(temp_I<-200) temp_I = -300;
    	Ctrl_drv_pit = PITCH_BASE_VALUE+temp_P + temp_I + temp_D;
    	sum_pit_err += pit_err;
		if(sum_pit_err>1000) sum_pit_err = 1000;
		if(sum_pit_err<-1000) sum_pit_err = -1000;
    	last_pit_err = pit_err;	
}


void YAW_Control(float yaw_mid)
{
	static float temp_P = 0, temp_I = 0, temp_D = 0;
	
//	yaw_mid = 0;
	yaw_position = Yaw;
	yaw_err = (float)yaw_mid - (float)yaw_position;
	temp_P = kP_yaw * yaw_err;
	temp_I = kI_yaw * sum_yaw_err;
	temp_D = kD_yaw * (yaw_err - last_yaw_err);
		if(temp_I>60) temp_I = 60;
	    	if(temp_I<-60) temp_I = -60;
    	Ctrl_drv_yaw = YAW_BASE_VALUE+temp_P + temp_I + temp_D;
    	sum_yaw_err += yaw_err;
		if(sum_yaw_err>200) sum_yaw_err = 200;
		if(sum_yaw_err<-200) sum_yaw_err = -200;
    	last_yaw_err = yaw_err;	
}

void unlock_action(void)
{
	static uint16_t action_count = 0;  //????40?
	
	if(action_cmd != UNLOCK) return;
	action_count++;
	Alt_tgt = 0;
	if(action_count < 130)
	{
		Keep_Alt_thr = 1;
		Ctrl_drv_yaw = 1980;
	}
	else
	{
		action_state = UNLOCK_CPLT;
		action_count = 0;
	}
}

void lock_action(void)
{
	static uint16_t action_count = 0;  //????40?
	
	action_count++;
	Alt_tgt = 0;
	if(action_count < 300)
	{
		Keep_Alt_thr = 1;
		Ctrl_drv_yaw = 1020;
	}
	else
	{
		action_state = LOCK_CPLT;
		action_count = 0;
		mode_select = NONE;
		
	}
}

void take_off_action(uint16_t hover_alt)
{
	Alt_tgt = hover_alt;
	Keep_Alt_thr = THR_BASE_VALUE;
	Ctrl_drv_yaw = YAW_BASE_VALUE;
	if(Sonar_height > 500)
	{
		auto_rol_pit_switch = 1;
	}
	else
	{
		auto_rol_pit_switch = 0;
	}
	if(Sonar_height > Alt_tgt )
	{
		action_state = TAKEOFF_CPLT;	
	}
}

void circle_delay(uint16_t n_10ms)  //n_10ms > 1  ,???????? ,n_10ms == 1,??????
{
	static uint16_t action_count = 0;
	static uint16_t set_n_10ms = 0;
	if(n_10ms > 0)
	{
		set_n_10ms = n_10ms;
		delay_state = DELAY_UNCPLT ; 
	}
	else
	{
		action_count++;
		if(action_count == set_n_10ms)
		{
			delay_state = DELAY_CPLT ; 
			action_count = 0;
			set_n_10ms = 1;  //????action_count++
		}
	}
}

void landing_action(void)
{
	static uint16_t action_count = 0;
	
	kP_rol = (19.0*(float)Sonar_height + 3000.0)/10000.0;
	if(kP_rol < 0.7 ) kP_rol = 0.7;
	if(kP_rol > 2.5 ) kP_rol = 2.5;
	kP_pit = kP_rol;
	
	if(action_count % 20 == 0)
	{
		Alt_tgt -= 50;
		Keep_Alt_thr -= 6;
		if(Alt_tgt < 60) Alt_tgt = 60;
		if(Keep_Alt_thr < 1400) Keep_Alt_thr = 1400;
	}
	if(Sonar_height < 300)
	{
		auto_rol_pit_switch = 0;
		Keep_Alt_thr -= 5;
		kP_rol = 0.7;
		kP_pit = 0.7;
	}
	else
	{
		auto_rol_pit_switch = 1;
		roll_mid = 60;
		pitch_mid = 60;
	}
	if(Sonar_height < 180)
	{
		Keep_Alt_thr = 1000;
		action_state = LANDING_CPLT;
	}
	
	action_count++;
}

void landing_backward_action(void)
{
	static uint16_t action_count = 0;
	static uint8_t action_stage = 0;
	static uint16_t condition_count = 0;
	static uint8_t first_call = 1;
	
	action_count++;
	
	if(action_count % 20 == 0 && action_stage == 0)
	{
		Alt_tgt -= 70;
		Keep_Alt_thr -= 10;
		if(Alt_tgt < 500) Alt_tgt = 500;
		if(Keep_Alt_thr < 1500) Keep_Alt_thr = 1500;
	}
	if(Sonar_height < 500 && action_stage == 0)
	{
		action_stage = 1;
	}
	if(action_stage == 1)
	{
		roll_mid = 60;
		pitch_mid = 90;
		if(pitch_mid - pit_position > -10 && pitch_mid - pit_position < 10)
		{
			action_stage = 2;
		}
	}
	if(action_stage == 2)
	{
		if(Sonar_height > 200)
		{
			Keep_Alt_thr -= 4;
			auto_rol_pit_switch = 0;
		}
		else
		{
			Keep_Alt_thr = 1000;
			action_state = LANDING_CPLT;
			auto_rol_pit_switch = 1;
			roll_mid = 60;
			pitch_mid = 60;
		}
	}
}

void set_target(uint8_t target)
{
	static uint16_t action_count = 0;
	static uint8_t track_tag = 0xff;
	
	
	track_tag = target;
	R_SCI5_Serial_Send(&track_tag, 1);
	
	if(target == COLOR && uart5_rx_data[0] == 0xA0)
	{
		action_state = TARGET_SETTED;
	}
	if(target == BLACK && uart5_rx_data[0] == 0xA1)
	{
		action_state = TARGET_SETTED;
	}
}


void emergency_landing(void)
{
	static uint16_t action_count = 0;
	
	auto_rol_pit_switch = 0;
	
	if(action_state == EMERGENCY_LANDING_CPLT)
	{
		action_cmd = LOCK;
		lock_action();
	}
	else
	{
		if(action_count % 20 == 0)
		{
			Alt_tgt -= 75;
			Keep_Alt_thr -= 20;
			if(Alt_tgt < 60) Alt_tgt = 60;
			if(Keep_Alt_thr < 1400) Keep_Alt_thr = 1400;
		}
		if(Sonar_height < 300)
		{
			Keep_Alt_thr -= 30;
		}
		if(Sonar_height < 180)
		{
			Keep_Alt_thr = 1;
			action_state = EMERGENCY_LANDING_CPLT;
		}
	}
	
	action_count++;
}

void mission_1_action()
{
	static uint16_t action_count = 0;
	static uint8_t action_stage = 0;
	action_count++;
	
	if(action_cmd == UNLOCK)
	{
		unlock_action();
		if(action_state == UNLOCK_CPLT)
		{
			action_stage = 1;
		}
	}
	if(action_stage == 1)
	{
		set_target(BLACK);
		if(action_state == TARGET_SETTED)
		{
			action_cmd = TAKEOFF;
			action_stage = 2;
		}
	}
	if(action_cmd == TAKEOFF)
	{
		take_off_action(1200);
		if(action_state == TAKEOFF_CPLT)
		{
			action_cmd = LANDING;
		}
		else
		{
			action_count = 0;
		}
				
	}
	if(action_count < 280) return ;  // t = 160/40 = 4s
	if(action_cmd == LANDING)
	{
		landing_action();
		if(action_state == LANDING_CPLT)
			action_cmd = LOCK;	
	}
	if(action_cmd == LOCK)
	{
		lock_action();
	}
}

void mission_2_action()
{
	static uint16_t action_count = 0;
	float distance , x_dist , y_dist , z_dist;  //unit: mm
	uint8_t beep_cmd;
	
	action_count++;
	
	x_dist = fabs(rol_position - 60) / 50 * Sonar_height;
	y_dist = fabs(pit_position - 60) / 50 * Sonar_height;
	z_dist = Sonar_height;
	distance = sqrt(x_dist * x_dist + y_dist * y_dist + z_dist * z_dist);
	if(distance < 1500 && distance > 500)
	{
		beep_cmd = 0x11;
		PORT1.PODR.BIT.B0 = 1;
		PORT1.PODR.BIT.B1 = 1;
	}
	else
	{
		beep_cmd = 0x22;
		PORT1.PODR.BIT.B0 = 0;
		PORT1.PODR.BIT.B1 = 0;
	}
	R_SCI1_Serial_Send(&beep_cmd, 1);
}

void mission_3_action()
{
	static uint16_t action_count = 0 , condition_count = 0;
	static uint8_t action_stage = 0;
	action_count++;
	
	if(action_cmd == UNLOCK)
	{
		unlock_action();
		if(action_state == UNLOCK_CPLT)
		{
			action_cmd = SET_TARGET;
			action_stage = 1;
		}
	}
	if(action_stage == 1)
	{
		set_target(BLACK);
		if(action_state == TARGET_SETTED)
		{
			action_cmd = TAKEOFF;
			action_stage = 2;
		}
	}
	if(action_stage == 2)
	{
		take_off_action(1200);
		if(action_state == TAKEOFF_CPLT)
		{
			action_cmd = TRACK;
			action_stage = 3;
		}		
	}	
	if(action_stage == 3)
	{
		condition_count++;
		if(condition_count > 120)
		{
			action_stage = 4;
		}
	}
	if(action_stage == 4)
	{
		roll_mid = 60;
		pitch_mid = 16;
		if(pitch_mid - pit_position > -8 && pitch_mid - pit_position < 8)
		{
			action_state = TRACK_CPLT;
			action_cmd = SET_TARGET ;
			action_stage = 5;
			condition_count = 0;
		}
	}
	if(action_stage == 5)
	{
		set_target(COLOR);
		if(action_state == TARGET_SETTED)
		{
			action_stage = 6;
			roll_mid = 60;
			pitch_mid = 60;
			condition_count = 0;
			Alt_tgt = 800;
		}
	}
	if(action_stage == 6)
	{
		condition_count++;
		if(condition_count > 400)
		{
			action_stage = 7;
		}
	}		
	if(action_stage == 7)
	{
		landing_backward_action();
		if(action_state == LANDING_CPLT)
		{
			action_stage = 8;
			action_cmd = LOCK ;
		}	
	}
	if(action_stage == 8)
	{
		lock_action();
		if(action_state == LOCK_CPLT)
		{
			action_stage = 9;
		}
	}
	R_SCI1_Serial_Send(&action_stage, 1);
}

void mission_4_5_action()
{
	static uint16_t action_count = 0 , condition_count = 0;
	static uint8_t action_stage = 0;
	action_count++;
	
	if(action_cmd == UNLOCK)
	{
		unlock_action();
		if(action_state == UNLOCK_CPLT)
		{
			action_cmd = SET_TARGET;
			action_stage = 1;
		}
	}
	if(action_stage == 1)
	{
		set_target(BLACK);
		if(action_state == TARGET_SETTED)
		{
			action_cmd = TAKEOFF;
			action_stage = 2;
		}
	}
	if(action_stage == 2)
	{
		take_off_action(1200);
		if(action_state == TAKEOFF_CPLT)
		{
			action_cmd = TRACK;
			action_stage = 3;
		}		
	}	
	if(action_stage == 3)
	{
		condition_count++;
		if(condition_count > 120)
		{
			action_stage = 4;
		}
	}
	if(action_stage == 4)
	{
		roll_mid = 60;
		pitch_mid = 16;
		if(pitch_mid - pit_position > -8 && pitch_mid - pit_position < 8)
		{
			action_state = TRACK_CPLT;
			action_cmd = SET_TARGET ;
			action_stage = 5;
			condition_count = 0;
		}
	}
	if(action_stage == 5)
	{
		set_target(COLOR);
		if(action_state == TARGET_SETTED)
		{
			action_stage = 6;
			roll_mid = 60;
			pitch_mid = 60;
			condition_count = 0;
			Alt_tgt = 800;
		}
	}
	if(action_stage == 6)
	{
		if(uart1_rx_data[0] == 0xCF)
		{
			action_stage = 7;
		}
	}
	if(action_stage == 7)
	{
		landing_backward_action();
		if(action_state == LANDING_CPLT)
		{
			action_stage = 8;
			action_cmd = LOCK ;
		}	
	}
	if(action_stage == 8)
	{
		lock_action();
		if(action_state == LOCK_CPLT)
		{
			action_stage = 9;
		}
	}
	R_SCI1_Serial_Send(&uart1_rx_data[0], 1);
//	R_SCI5_Serial_Send(&uart1_rx_data[0], 1);
}

/*#define TAKE0FF 0
#define LANDING 1
#define TRACK   2
#define UNLOCK  3
#define LOCK    4*/

void mission_action(uint8_t mission_select)
{
	static uint16_t action_count = 0;
	if(mission_select == NONE) return;
	if(uart1_rx_data[0] == 0xCD)
	{
		emergency_landing();
		return;
	}
	if(action_cmd == UNLOCK)
		action_count ++ ;
	if(action_count < 240) return;
	switch(mission_select)
	{
		case MISSION_1 : mission_1_action();break;
		case MISSION_2 : mission_2_action();break;
		case MISSION_3 : mission_3_action();break;
		case MISSION_4 : mission_4_5_action();break;
		case MISSION_5 : mission_4_5_action();break;
		default : break;
	}
}