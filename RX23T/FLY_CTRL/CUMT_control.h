#ifndef _CUMT_CONTROL_H
#define _CUMT_CONTROL_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for function. Do not edit comment generated here */
#define MODE_MANUAL 0x01
#define MODE_KEEP_ALT 0x02
#define MODE_KEEP_POS 0x03

#define TAKEOFF 0
#define LANDING 1
#define TRACK   2
#define UNLOCK  3
#define LOCK    4
#define SET_TARGET 5
#define FREE 6
#define EMERGENCY_LANDING 7

#define TAKEOFF_CPLT 0
#define LANDING_CPLT 1
#define TRACK_CPLT   2
#define UNLOCK_CPLT  3
#define LOCK_CPLT    4
#define TARGET_SETTED 5
#define EMERGENCY_LANDING_CPLT 6

#define DELAY_CPLT      0
#define DELAY_UNCPLT    1

#define NONE       0
#define MISSION_1  1
#define MISSION_2  2
#define MISSION_3  3
#define MISSION_4  4
#define MISSION_5  5
#define MISSION_6  6
#define MISSION_7  7
#define MISSION_8  8


#define COLOR 0xB0
#define BLACK 0xB1


#define ROLL_BASE_VALUE  1560
#define PITCH_BASE_VALUE 1535
#define THR_BASE_VALUE   1750
#define YAW_BASE_VALUE   1500
#define ROL_PIT_ALWD_OFST   180
#define THR_ALWD_OFST       0
#define YAW_ALWD_OFST       170


extern volatile uint8_t Fly_mode;
extern volatile uint8_t is_Armed;

extern volatile float Yaw_tgt;
extern volatile float Pit_tgt;
extern volatile float Rol_tgt;
extern volatile float Alt_tgt;

void CUMT_Ctrl_Check_Arm(void);
void CUMT_Ctrl_Check_Mode(void);
void CUMT_Ctrl_Get_RC_Command(void);
void CUMT_Ctrl_Attitude_Outer_Loop(void);
void CUMT_Ctrl_Attitude_Inner_Loop(void);
void CUMT_Ctrl_Keep_Altitude(void);
void HIGH_Control(void);
void PITCH_Control(void);
void YAW_Control(void);
void mission_action(uint8_t mission_select);
void landing_backward_action(void);
/* Copyright (C) CUMT. All rights reserved.*/
/* End user code. Do not edit comment generated here */
#endif