#ifndef _mpu6050_H
#define _mpu6050_H

typedef struct
{
	float Angle_x;
	float Angle_y;
	float Angle_z;
	float Accel_x;
	float Accel_y;
	float Accel_z;
	float Gyro_x;
	float Gyro_y;
	float Gyro_z;
	float Tmp;
}MPU6050_TypeDef;	


#endif
