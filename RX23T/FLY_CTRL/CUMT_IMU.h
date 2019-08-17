#ifndef _CUMT_IMU_H
#define _CUMT_IMU_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for function. Do not edit comment generated here */

extern volatile float Pitch, Roll, Yaw;
extern volatile float Alpha_rad;
//extern volatile int16_t Acc_Gyro[6];
//extern volatile int16_t Mag[3];
extern volatile float kP_IMU, kI_IMU;
//extern float g_a_m[9]; //gyro-acc-Mag
#define KP 2.0f                        // proportional gain governs rate of convergence to accelerometer/magnetometer
#define KI 0.01f                      // integral gain governs rate of convergence of gyroscope biases
#define halfT 0.005f                   // half the sample period
#define deltaT 0.01f                   // sample period
#define IMU_INT_LIMIT 0.0349		//2*pi/180

void CUMT_IMU_Init(void);
void CUMT_IMU_Get_Attitude(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);
void CUMT_IMU_Get_Yaw(float mx, float my, float mz);

/* End user code. Do not edit comment generated here */
#endif