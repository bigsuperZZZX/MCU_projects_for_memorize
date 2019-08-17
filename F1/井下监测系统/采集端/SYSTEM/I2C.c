#include <I2C.h>
#include <delay.h>
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
/***************************************************************************
��    �ƣ�void InitI2C0(void)
��    �ܣ���ʼ��IO��
��ڲ�������
���ڲ�������
˵    ����
���÷�����InitI2C0();
***************************************************************************/
void InitI2C0(void)
{		
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOBʱ��

  //GPIOB8,B9��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
	I2C_SCL0=1;
	I2C_SDA0=1;
}

/***************************************************************************
��    �ƣ�void StartI2C0(void)
��    �ܣ�����I2C���ߣ�������I2C��ʼ����
��ڲ�������
���ڲ�������
˵    ����
���÷�����StartI2C0();
***************************************************************************/
void StartI2C0(void)
{
    I2C_SDA0=1;    
    delay_us(100);

    I2C_SCL0=1;	   
    delay_us(100);
 
    I2C_SDA0=0;
    delay_us(100);

    I2C_SCL0=0;	 
    delay_us(100);


}

/***************************************************************************
��    �ƣ�void StopI2C0(void)
��    �ܣ�����I2C���ߣ�������I2C��������
��ڲ�������
���ڲ�������
˵    ����
���÷�����StopI2C0();
***************************************************************************/
void StopI2C0(void)
{
	I2C_SDA0=0;
    delay_us(100);
 
    I2C_SCL0=1;
    delay_us(100);  

    I2C_SDA0=1;
    delay_us(100);
 

}

/***************************************************************************
��    �ƣ�void NoAck0(void)
��    �ܣ���Ӧ��
��ڲ�������
���ڲ�������
˵    ����
���÷�����NoAck0();
***************************************************************************/
void NoAck0(void)
{
	
	I2C_SDA0=1;
	delay_us(100);
	
	I2C_SCL0=1;
	delay_us(100);
	
	I2C_SCL0=0;
	delay_us(100);
	
	I2C_SDA0=0;
	delay_us(100);

}
/***************************************************************************
��    �ƣ�void Ack0(void)
��    �ܣ�����Ӧ��
��ڲ�������
���ڲ�������
˵    ����
���÷�����Ack0();
***************************************************************************/
void Ack0(void)
{
	SDA_OUT0();
 	I2C_SDA0=1;
    delay_us(100); 
    I2C_SCL0=1;
    delay_us(100); 
	I2C_SCL0=0;
    delay_us(100); 
 	I2C_SDA0=1;

	
}
/***************************************************************************
��    �ƣ�unsigned char TestAck0( )
��    �ܣ�Ӧ��У��
��ڲ�������
���ڲ�����Ӧ��״̬
˵    ����
���÷�����m=TestAck0();
***************************************************************************/
unsigned char TestAck0( )
{
	unsigned char ack;

	I2C_SCL0=1;
	delay_us(100);
	SDA_IN0();
	delay_us(100);
	ack=SDA_STATE0;
	delay_us(100);
	I2C_SCL0=0;
	delay_us(100);
	return(ack);


}
/***************************************************************************
��    �ƣ�unsigned char I2CWrite0(unsigned char DData)
��    �ܣ���I2C�����Ϸ���1�ֽ�����
��ڲ�����DData    ����������
���ڲ�������
˵    ����
���÷�����NoAck0();
***************************************************************************/
unsigned char I2CWrite0(unsigned char DData)
{
	unsigned char j,tem;

	for(j=0;j<8;j++) 
	{
		if((DData<<j)&0x80)
		{
			I2C_SDA0=1;
		}
		else
		{
			I2C_SDA0=0;
		}
		delay_us(100);
		I2C_SCL0=1;
		delay_us(100);
		I2C_SCL0=0;;
		delay_us(100);

	}
	delay_us(100);
	
	SDA_IN0();
	delay_us(100);
	I2C_SCL0=1;
	delay_us(1000);
	if(SDA_STATE0==1)
	{
		tem=0;  
	}
	else
	{
		tem=1;   
	}
	I2C_SCL0=0;
	delay_us(100);	
    SDA_OUT0();

	return(tem);  
}

/***************************************************************************
��    �ƣ�unsigned char I2CRead0(void)
��    �ܣ���I2C�����Ͻ���1�ֽ�����
��ڲ�������
���ڲ�������������
˵    ����
���÷�����m=I2CRead0();
***************************************************************************/
unsigned char I2CRead0(void)
{
	unsigned char read,j;
	read=0x00;
	
	SDA_IN0();
	delay_us(100);
	for(j=8;j>0;j--) 
	{		     
		delay_us(100);
		I2C_SCL0=1;
		delay_us(100);
		read=read<<1;
		if(SDA_STATE0==1) 
		{
			read=read+1;
		}
		I2C_SCL0=0;
		delay_us(100);
	}	
	return(read);
}


/***************************************************************************
***************************************************************************/
