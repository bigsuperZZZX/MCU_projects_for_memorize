#ifndef __IIC_H
#define __IIC_H
#include "stdio.h"	
#include "sys.h" 


#define I2C_SCL_H GPIO_SetBits(GPIO_I2C,I2C_SCL)
#define I2C_SCL_L GPIO_ResetBits(GPIO_I2C,I2C_SCL)
#define I2C_SDA_H GPIO_SetBits(GPIO_I2C,I2C_SDA)
#define I2C_SDA_L GPIO_ResetBits(GPIO_I2C,I2C_SDA)

#define GPIO_I2C GPIOB    //定义IIC的接口
#define	GPIO_I2C_Clock	RCC_APB2Periph_GPIOB
#define I2C_SCL GPIO_Pin_6
#define I2C_SDA GPIO_Pin_7



void I2C_SDA_OUT(void);
void I2C_SDA_IN(void);
void I2C_Init(void);  //初始化IIC
void I2C_Start(void) ;  //产生起始信号 
void I2C_Stop(void) ;  //2. 结束信号
void I2C_Ack(void); //主机产生应答信号 ACK
void I2C_NAck(void);  //主机不产生应答信号 NACK
//等待从机应答信号
//返回值： 1 接收应答失败
// 0 接收应答成功
u8 I2C_Wait_Ack(void);
void I2C_Send_Byte(u8 txd);  //I2C 发送一个字节 
u8 I2C_Read_Byte(u8 ack);  //I2C 读取一个字节

#endif


