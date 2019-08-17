#ifndef I2C_H
#define I2C_H

#include "sys.h"

#define SDA_IN0()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
#define SDA_OUT0() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}


//IO��������	 
#define I2C_SCL0    PCout(12) //SCL
#define I2C_SDA0    PCout(11) //SDA	 
#define SDA_STATE0  PCin(11)  //����SDA 



void InitI2C0(void);   //I2C��ʼ��
void StartI2C0(void);  //��ʼ�ź�
void StopI2C0(void);  //ֹͣ�ź�
void NoAck0(void);   //��Ӧ���ź�
void Ack0(void);
unsigned char TestAck0(void);
unsigned char I2CWrite0(unsigned char DData);   //дһ���ֽ�
unsigned char I2CRead0(void);  //��һ���ֽ�

#endif 
