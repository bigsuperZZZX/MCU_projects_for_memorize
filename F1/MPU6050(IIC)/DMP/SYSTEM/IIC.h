#ifndef __IIC_H
#define __IIC_H
#include "stdio.h"	
#include "sys.h" 


#define I2C_SCL_H GPIO_SetBits(GPIO_I2C,I2C_SCL)
#define I2C_SCL_L GPIO_ResetBits(GPIO_I2C,I2C_SCL)
#define I2C_SDA_H GPIO_SetBits(GPIO_I2C,I2C_SDA)
#define I2C_SDA_L GPIO_ResetBits(GPIO_I2C,I2C_SDA)

#define GPIO_I2C GPIOB    //����IIC�Ľӿ�
#define	GPIO_I2C_Clock	RCC_APB2Periph_GPIOB
#define I2C_SCL GPIO_Pin_6
#define I2C_SDA GPIO_Pin_7



void I2C_SDA_OUT(void);
void I2C_SDA_IN(void);
void I2C_Init(void);  //��ʼ��IIC
void I2C_Start(void) ;  //������ʼ�ź� 
void I2C_Stop(void) ;  //2. �����ź�
void I2C_Ack(void); //��������Ӧ���ź� ACK
void I2C_NAck(void);  //����������Ӧ���ź� NACK
//�ȴ��ӻ�Ӧ���ź�
//����ֵ�� 1 ����Ӧ��ʧ��
// 0 ����Ӧ��ɹ�
u8 I2C_Wait_Ack(void);
void I2C_Send_Byte(u8 txd);  //I2C ����һ���ֽ� 
u8 I2C_Read_Byte(u8 ack);  //I2C ��ȡһ���ֽ�

#endif


