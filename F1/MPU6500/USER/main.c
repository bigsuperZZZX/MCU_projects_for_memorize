#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "DataScope_DP.h"
#include "usart.h"
#include "stm32f10x_tim.h"
#include "mpu6500_spi.h"
#include "stm32f10x_spi.h"
#include "mpu6500.h"
#include "ahrs.h"

	
extern S_INT16_XYZ MPU6500_Acc;
extern S_INT16_XYZ MPU6500_Gyro;
extern S_INT16_XYZ Acc_Avg;
extern u16 sys_count;

int main(void)
{
	delay_init();                 //延时初始化
	uart_init(256000);
	while(MPU6500_Init() == 0 );  //6500初始化
	sys_it();
	
	
	while(1)
	{
		MPU6500_CS(0); 
		if(sys_count % 5 == 0)
		{
			Prepare_Data();
			IMU_Update();	
		}
		MPU6500_CS(1); 
		if(sys_count % 102 == 0)
		{
			printf("%.2f %.2f %.2f\n", Angle.X , Angle.Y , Angle.Z/*,  MPU6500_Acc.X,MPU6500_Acc.Y,MPU6500_Acc.Z ,MPU6500_Gyro.X,MPU6500_Gyro.Y,MPU6500_Gyro.Z*/);
		}	
		delay_us(600);
	}
}

