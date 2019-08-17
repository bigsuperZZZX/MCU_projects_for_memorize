#include "mpu6050.h"
#include "stm32f10x.h"
#include "string.h"

MPU6050_TypeDef	MPU6050_InitStructure;

void USART3_IRQHandler(void)  //�ݶ�Ĭ��ʹ�ô���3
{
	static short Buff[11];
	static char counter=0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж���Ч,���������ݼĴ�����
     {
			 Buff[counter] = USART_ReceiveData(USART3);   //��������
			 counter++;
		 }
		 if(counter==10&&Buff[0]==0x55)
     {
			 switch(Buff[1])
         {
             case 0x51: //��ʶ������Ǽ��ٶȰ�
                MPU6050_InitStructure.Accel_x = ((short)(Buff[3]<<8 | Buff[2]))/32768.0*16;      //X����ٶ�
                MPU6050_InitStructure.Accel_y = ((short)(Buff[5]<<8 | Buff[4]))/32768.0*16;      //Y����ٶ�
                MPU6050_InitStructure.Accel_z = ((short)(Buff[7]<<8 | Buff[6]))/32768.0*16;      //Z����ٶ�
                MPU6050_InitStructure.Tmp = ((short)(Buff[9]<<8| Buff[8]))/340.0+36.25;  			  //�¶�
                break;
             case 0x52: //��ʶ������ǽ��ٶȰ�
                MPU6050_InitStructure.Gyro_x = ((short)(Buff[3]<<8| Buff[2]))/32768.0*2000;      //X����ٶ�
                MPU6050_InitStructure.Gyro_y = ((short)(Buff[5]<<8| Buff[4]))/32768.0*2000;      //Y����ٶ�
                MPU6050_InitStructure.Gyro_z = ((short)(Buff[7]<<8| Buff[6]))/32768.0*2000;      //Z����ٶ�
                MPU6050_InitStructure.Tmp = ((short)(Buff[9]<<8| Buff[8]))/340.0+36.25;  			  //�¶�
                break;
             case 0x53: //��ʶ������ǽǶȰ�
                MPU6050_InitStructure.Angle_x = ((short)(Buff[3]<<8| Buff[2]))/32768.0*180;     //X���ת�ǣ�x �ᣩ
                MPU6050_InitStructure.Angle_y = ((short)(Buff[5]<<8| Buff[4]))/32768.0*180;     //Y�ḩ���ǣ�y �ᣩ
                MPU6050_InitStructure.Angle_z = ((short)(Buff[7]<<8| Buff[6]))/32768.0*180;     //Z��ƫ���ǣ�z �ᣩ
                MPU6050_InitStructure.Tmp = ((short)(Buff[9]<<8| Buff[8]))/340.0+36.25;  			  //�¶�
                break;
             default:  break;
          }
				 counter=0;
		 }
}
