#ifndef I2C_H
#define I2C_H

#include "sys.h"

//IO��������
#define SDA_IN0()  {GPIOE->MODER&=~(3<<(6*2));GPIOE->MODER|=0<<6*2;}	//����ģʽ
#define SDA_OUT0() {GPIOE->MODER&=~(3<<(6*2));GPIOE->MODER|=1<<6*2;} //���ģʽ


//IO��������	 
#define I2C_SCL0    PEout(4) //SCL
#define I2C_SDA0    PEout(6) //SDA	 
#define SDA_STATE0  PEin(6)  //����SDA 



void InitI2C0(void);   //I2C��ʼ��
void StartI2C0(void);  //��ʼ�ź�
void StopI2C0(void);  //ֹͣ�ź�
void NoAck0(void);   //��Ӧ���ź�
void Ack0(void);
unsigned char TestAck0(void);
unsigned char I2CWrite0(unsigned char DData);   //дһ���ֽ�
unsigned char I2CRead0(void);  //��һ���ֽ�

#endif 
