#include "mpu6050.h"
#include "stm32f10x.h"
#include "string.h"

MPU6050_TypeDef	MPU6050_InitStructure;

void USART3_IRQHandler(void)  //暂定默认使用串口3
{
	static short Buff[11];
	static char counter=0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断有效,若接收数据寄存器满
     {
			 Buff[counter] = USART_ReceiveData(USART3);   //接收数据
			 counter++;
		 }
		 if(counter==10&&Buff[0]==0x55)
     {
			 switch(Buff[1])
         {
             case 0x51: //标识这个包是加速度包
                MPU6050_InitStructure.Accel_x = ((short)(Buff[3]<<8 | Buff[2]))/32768.0*16;      //X轴加速度
                MPU6050_InitStructure.Accel_y = ((short)(Buff[5]<<8 | Buff[4]))/32768.0*16;      //Y轴加速度
                MPU6050_InitStructure.Accel_z = ((short)(Buff[7]<<8 | Buff[6]))/32768.0*16;      //Z轴加速度
                MPU6050_InitStructure.Tmp = ((short)(Buff[9]<<8| Buff[8]))/340.0+36.25;  			  //温度
                break;
             case 0x52: //标识这个包是角速度包
                MPU6050_InitStructure.Gyro_x = ((short)(Buff[3]<<8| Buff[2]))/32768.0*2000;      //X轴角速度
                MPU6050_InitStructure.Gyro_y = ((short)(Buff[5]<<8| Buff[4]))/32768.0*2000;      //Y轴角速度
                MPU6050_InitStructure.Gyro_z = ((short)(Buff[7]<<8| Buff[6]))/32768.0*2000;      //Z轴角速度
                MPU6050_InitStructure.Tmp = ((short)(Buff[9]<<8| Buff[8]))/340.0+36.25;  			  //温度
                break;
             case 0x53: //标识这个包是角度包
                MPU6050_InitStructure.Angle_x = ((short)(Buff[3]<<8| Buff[2]))/32768.0*180;     //X轴滚转角（x 轴）
                MPU6050_InitStructure.Angle_y = ((short)(Buff[5]<<8| Buff[4]))/32768.0*180;     //Y轴俯仰角（y 轴）
                MPU6050_InitStructure.Angle_z = ((short)(Buff[7]<<8| Buff[6]))/32768.0*180;     //Z轴偏航角（z 轴）
                MPU6050_InitStructure.Tmp = ((short)(Buff[9]<<8| Buff[8]))/340.0+36.25;  			  //温度
                break;
             default:  break;
          }
				 counter=0;
		 }
}
