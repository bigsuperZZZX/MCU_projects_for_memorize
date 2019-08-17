#ifndef _I2C_H_
#define _I2C_H_
#include "system.h"

//=========================
//从设备地址 +0 为写入；+1为读取 
//值和芯片有关，请注意根据实际情况更改
//MPU6050的SlaveAddress为 0xd0
//HMC5883的SlaveAddress为0x3C
//=========================
#define SlaveAddress 	0x3c

//=========================
//I2C引脚定义
//=========================
#define SCL_0		GPIOC -> BRR  		 	= 1<< 0			//PC0
#define SCL_1		GPIOC -> BSRR 	 	= 1<< 0
#define SDA_0		GPIOC -> BRR			= 1<< 1			//PC1
#define SDA_1		GPIOC -> BSRR			= 1<< 1

#define	SDA_STATUS	(GPIOC -> IDR & (1 << 1))
//=========================
//函数声明
//=========================
void I2C_init(void);
void I2C_Write_Reg(unsigned char REG_Address,unsigned char REG_data);
unsigned char I2C_Read_Reg(unsigned char REG_Address);

void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendACK(unsigned char ack);
unsigned char I2C_RecvACK(void);
void I2C_Write(unsigned char dat);
unsigned char I2C_Read(void);



#endif
