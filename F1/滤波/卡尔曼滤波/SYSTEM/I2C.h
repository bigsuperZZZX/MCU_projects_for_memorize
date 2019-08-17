#ifndef I2C_H
#define I2C_H

#include "sys.h"

//IO方向设置
#define SDA_IN0()  {GPIOE->MODER&=~(3<<(6*2));GPIOE->MODER|=0<<6*2;}	//输入模式
#define SDA_OUT0() {GPIOE->MODER&=~(3<<(6*2));GPIOE->MODER|=1<<6*2;} //输出模式


//IO操作函数	 
#define I2C_SCL0    PEout(4) //SCL
#define I2C_SDA0    PEout(6) //SDA	 
#define SDA_STATE0  PEin(6)  //输入SDA 



void InitI2C0(void);   //I2C初始化
void StartI2C0(void);  //起始信号
void StopI2C0(void);  //停止信号
void NoAck0(void);   //非应答信号
void Ack0(void);
unsigned char TestAck0(void);
unsigned char I2CWrite0(unsigned char DData);   //写一个字节
unsigned char I2CRead0(void);  //读一个字节

#endif 
