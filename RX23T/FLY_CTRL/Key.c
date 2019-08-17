#include "r_cg_macrodriver.h"
#include "Key.h"
#include "CUMT_PID.h"
#include "CUMT_RC.h"
#include "CUMT_motor.h"
#include "CUMT_control.h"
#include "CUMT_MPU6050.h"
#include "CUMT_math.h"
#include "CUMT_IMU.h"
#include "CUMT_Delay.h"
#include "CUMT_UART.h"
#include <math.h>

uint8_t key_pressed = 0;
extern uint8_t mode_select;
extern float yaw_offset_init_value;
extern uint8_t action_cmd;

uint8_t key_scan(void)
{
	static uint8_t last_key_pressed = 0;
	uint8_t key_pressed = 0;
	if(PORT7.PIDR.BIT.B0 == 0)
	{
		key_pressed = 1;
	}
	else if(PORT7.PIDR.BIT.B1 == 0)
	{
		key_pressed = 2;
	}
	else if(PORT7.PIDR.BIT.B2 == 0)
	{
		key_pressed = 3;
	}
	else if(PORT7.PIDR.BIT.B3 == 0)
	{
		key_pressed = 4;
	}
	else if(PORT7.PIDR.BIT.B4 == 0)
	{
		key_pressed = 5;
	}
	else if(PORT7.PIDR.BIT.B5 == 0)
	{
		key_pressed = 6;
	}
	else
	{
		key_pressed = 0;
	}
	
	if(last_key_pressed == key_pressed)
	{
		return key_pressed;
	}
	
	last_key_pressed = key_pressed;
	return 0;
}


void decode_key(uint8_t key)
{
	uint8_t send_char = 0xb3;
	if(key == 1)
	{
		mode_select = MISSION_1;
		action_cmd = LOCK;
	}
	if(key == 2)
	{
		mode_select = MISSION_2;
		action_cmd = LOCK;
	}
	if(key == 3)
	{
		mode_select = MISSION_3;
		action_cmd = LOCK;
	}
	if(key == 4)
	{
		mode_select = MISSION_4;
		action_cmd = LOCK;
	}
	if(key == 5)
	{
		action_cmd = UNLOCK;
		yaw_offset_init_value = Yaw;
	}
	if(key == 6)
	{
		R_SCI5_Serial_Send(&send_char,1);
	}
}











