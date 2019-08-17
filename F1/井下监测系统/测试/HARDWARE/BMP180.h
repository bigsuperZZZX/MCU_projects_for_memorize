#ifndef __BMP180_H
#define __BMP180_H	
#include "stm32f10x.h"

void IIC_PortInit(void);
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
unsigned char IIC_ReceiveACK(void);
void IIC_SendACK(unsigned char ack);
unsigned char IIC_SendByte(unsigned char dat);
unsigned char IIC_ReadByte(void);
u8 BMP180_ReadOneByte(u8 ReadAddr);
short BMP180_ReadTwoByte(u8 ReadAddr);
void Write_OneByteToBMP180(u8 RegAdd, u8 Data);
void Read_CalibrationData(void);
long Get_BMP180UT(void);
long Get_BMP180UP(void);
long Convert_UncompensatedToTrue(long UT,long UP);




#endif


