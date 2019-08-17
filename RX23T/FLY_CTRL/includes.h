
#ifndef _INCLUDES_H
#define _INCLUDES_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
/*
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
#include "r_cg_it.h"
#include "r_cg_userdefine.h"
*/
/* Start user code for include. Do not edit comment generated here */
// user head file
#include "CUMT_delay.h"
#include <math.h>
#include "CUMT_math.h"
#include "CUMT_filter.h"
#include "CUMT_led.h"
#include "CUMT_sonar.h"
#include "CUMT_RC.h"
#include "CUMT_motor.h"
#include "CUMT_UART.h"
#include "CUMT_IIC.h"
#include "CUMT_MPU6050.h"
#include "CUMT_HMC5883.h"
#include "CUMT_BMP180.h"
#include "CUMT_IMU.h"
#include "CUMT_PID.h"
#include "CUMT_control.h"
#include "CUMT_schedule.h"

extern volatile uint32_t Systime_cnt_5ms;
extern volatile uint8_t  Systime_cnt_flag;

/* End user code. Do not edit comment generated here */
#endif
