
#include "r_cg_macrodriver.h"
#include "CUMT_IMU.h"
#include "CUMT_MPU6050.h"
#include "CUMT_HMC5883.h"
#include "CUMT_IIC.h"
#include "CUMT_delay.h"
#include "CUMT_math.h"
#include <math.h> //!!! important
#include "CUMT_control.h"

volatile float Pitch = 0, Roll = 0, Yaw = 0; //unit: degree
volatile float Alpha_rad = 0; //unit: rad  used for throttle compensation
volatile float kP_IMU = 6, kI_IMU = 0.01;

static float exInt = 0, eyInt = 0, ezInt = 0;    // integral error		
static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;    // quaternion elements
static float Hx, Hy;
static float pit_offset = 0, rol_offset = 0, yaw_offset = 0, yaw_speed = 0 ,yaw_last = 0;
float yaw_offset_init_value = 0;


void CUMT_IMU_Init(void)
{
	static uint8_t is_connected = 0;
	static uint16_t init_cnt = 500;
	
	CUMT_Delay_ms(100);
	CUMT_MPU6050_Init();
	CUMT_Delay_ms(1);
	is_connected = CUMT_MPU6050_TestConnection();
	while(!is_connected)
	{
		is_connected = CUMT_MPU6050_TestConnection();
		CUMT_MPU6050_Init();
	}
	CUMT_Delay_ms(2);
	//CUMT_HMC5883_Init();
	CUMT_Delay_ms(50);
	
	exInt = 0;
	eyInt = 0;
	ezInt = 0;
	
	q0 = 1;
	q1 = 0;
	q2 = 0;
	q3 = 0;
	CUMT_MPU6050_Get_Data6(&Acc_Gyro[0],&Acc_Gyro[1],&Acc_Gyro[2],&Acc_Gyro[3],&Acc_Gyro[4],&Acc_Gyro[5]);
	CUMT_IMU_Get_Attitude(Acc_Gyro[0],Acc_Gyro[1],Acc_Gyro[2],Acc_Gyro[3],Acc_Gyro[4],Acc_Gyro[5],Mag[0],Mag[1],Mag[2]);
	yaw_last = Yaw;
	CUMT_MPU6050_Get_Data6(&Acc_Gyro[0],&Acc_Gyro[1],&Acc_Gyro[2],&Acc_Gyro[3],&Acc_Gyro[4],&Acc_Gyro[5]);
	CUMT_IMU_Get_Attitude(Acc_Gyro[0],Acc_Gyro[1],Acc_Gyro[2],Acc_Gyro[3],Acc_Gyro[4],Acc_Gyro[5],Mag[0],Mag[1],Mag[2]);
	yaw_speed = Yaw - yaw_last;
	yaw_last = Yaw;
	while(init_cnt--)
	{
		CUMT_MPU6050_Get_Data6(&Acc_Gyro[0],&Acc_Gyro[1],&Acc_Gyro[2],&Acc_Gyro[3],&Acc_Gyro[4],&Acc_Gyro[5]);
		//CUMT_HMC5883_Get_Data3(&Mag[0], &Mag[1], &Mag[2]);
		CUMT_IMU_Get_Attitude(Acc_Gyro[0],Acc_Gyro[1],Acc_Gyro[2],Acc_Gyro[3],Acc_Gyro[4],Acc_Gyro[5],Mag[0],Mag[1],Mag[2]);
		yaw_speed = 0.95*yaw_speed + 0.05*(Yaw - yaw_last);
		yaw_last = Yaw;
		CUMT_Delay_ms(5);
	}
	yaw_offset = yaw_speed;
	Yaw_tgt = Yaw; 
	do
	{
		CUMT_MPU6050_Get_Data6(&Acc_Gyro[0],&Acc_Gyro[1],&Acc_Gyro[2],&Acc_Gyro[3],&Acc_Gyro[4],&Acc_Gyro[5]);
		//CUMT_HMC5883_Get_Data3(&Mag[0], &Mag[1], &Mag[2]);
		CUMT_IMU_Get_Attitude(Acc_Gyro[0],Acc_Gyro[1],Acc_Gyro[2],Acc_Gyro[3],Acc_Gyro[4],Acc_Gyro[5],Mag[0],Mag[1],Mag[2]);
		CUMT_Delay_ms(5);
	}
	while(Yaw > 1.5 || Yaw < -1.5);
}

void CUMT_IMU_Get_Attitude(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
	static uint16_t getted_count = 0;
	static float yaw_last = 0;
	
	float norm;
	float vx, vy, vz;
	float wx, wy, wz;
	float ex, ey, ez;
	float hx, hy, hz, bx, bz;
	float q0_temp, q1_temp ,q2_temp, q3_temp;
	float t11, t12, t13, t23, t33;
	
	float q0q0 = q0*q0;
	float q0q1 = q0*q1;
	float q0q2 = q0*q2;
	float q0q3 = q0*q3;
	float q1q1 = q1*q1;
	float q1q2 = q1*q2;
	float q1q3 = q1*q3;
	float q2q2 = q2*q2;
	float q2q3 = q2*q3;
	float q3q3 = q3*q3;
		
	//	ax *= ACC_G; //*ACC_G?
	//	ay *= ACC_G;
	//	az *= ACC_G;
	gx *= GYRO_GR;
	gy *= GYRO_GR;
	gz *= GYRO_GR;

	norm = CUMT_invSqrt(ax*ax + ay*ay + az*az);
	ax = ax*norm;
	ay = ay*norm;
	az = az*norm;
	
/*	norm = CUMT_invSqrt(mx*mx + my*my + mz*mz);          
	mx = mx*norm;
	my = my*norm;
	mz = mz*norm;
	
	hx = 2*mx*(0.5f - q2q2 - q3q3) + 2*my*(q1q2 - q0q3) + 2*mz*(q1q3 + q0q2);
	hy = 2*mx*(q1q2 + q0q3) + 2*my*(0.5f - q1q1 - q3q3) + 2*mz*(q2q3 - q0q1);
	hz = 2*mx*(q1q3 - q0q2) + 2*my*(q2q3 + q0q1) + 2*mz*(0.5f - q1q1 - q2q2);         
	bx = sqrt((hx*hx) + (hy*hy));
	bz = hz;*/

	vx = 2*(q1q3 - q0q2);															
	vy = 2*(q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3 ;
	
/*	wx = 2*bx*(0.5 - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);
	wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);
	wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5 - q1q1 - q2q2); */

	// error is sum of cross product between reference direction of fields and direction measured by sensors
	ex = (ay*vz - az*vy) /*+ (my*wz - mz*wy)*/;
	ey = (az*vx - ax*vz) /*+ (mz*wx - mx*wz)*/;
	ez = (ax*vy - ay*vx) /*+ (mx*wy - my*wx)*/;
	
	if((ex != 0.0f )&& (ey != 0.0f) && (ez != 0.0f))
	{
		exInt = exInt + ex*kI_IMU*deltaT;								  						
		eyInt = eyInt + ey*kI_IMU*deltaT;
		ezInt = ezInt + ez*kI_IMU*deltaT;
		
	//	exInt = CUMT_LIMIT(exInt, -IMU_INT_LIMIT, IMU_INT_LIMIT);
	//	eyInt = CUMT_LIMIT(eyInt, -IMU_INT_LIMIT, IMU_INT_LIMIT);
	//	ezInt = CUMT_LIMIT(ezInt, -IMU_INT_LIMIT, IMU_INT_LIMIT);
	
		// adjusted gyroscope measurements
		gx = gx + kP_IMU*ex + exInt;					   									
		gy = gy + kP_IMU*ey + eyInt;
		gz = gz + kP_IMU*ez + ezInt;	// kP = 10?
	}	
		
	// integrate quaternion rate and normalise					
	q0_temp = q0 +(-q1*gx - q2*gy - q3*gz)*halfT;
	q1_temp = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
	q2_temp = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
	q3_temp = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

	// normalise quaternion
	norm = CUMT_invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0_temp*norm;
	q1 = q1_temp*norm;
	q2 = q2_temp*norm;
	q3 = q3_temp*norm;

	t11 = q0*q0 + q1*q1 - q2*q2 - q3*q3;
	t12 = 2*(q1*q2 + q0*q3);
	t13 = 2*(q1*q3 - q0*q2);
	t23 = 2*(q2*q3 + q0*q1);
	t33 = q0*q0 + q3*q3 - q1*q1 - q2*q2;

	Roll = fast_atan2(t23,t33)*57.3 - rol_offset;
	Pitch = asin(t13)*57.3 - pit_offset;  
	Yaw = -fast_atan2(t12, t11)*57.3 - yaw_offset*1.01*getted_count - yaw_offset_init_value; // yaw
	/*if(yaw_offset>1e-5 || yaw_offset<-1e-5)
	{
		Yaw = 0.5*Yaw + 0.5*yaw_last;
		yaw_last = Yaw;
	}*/
	
	getted_count++;
}

void CUMT_IMU_Get_Yaw(float mx, float my, float mz)
{	
	Hx = mx*cos(Pitch/57.3) - mz*sin(Pitch/57.3)*cos(Roll/57.3);	// mx*cos(P_rad) - mz*sin(P_rad)
	Hy = my*cos(Roll/57.3) - mz*sin(Roll/57.3)*cos(Pitch/57.3);	// my*cos(R_rad) - mz*sin(R_rad)
	
	Yaw   = atan2(Hy,Hx)*57.3 - yaw_offset;
}
