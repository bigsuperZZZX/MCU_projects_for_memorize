#include "delay.h"
#include "oled.h"
#include "math.h"
#include "stm32f10x.h"
#include "IIC.h"
#include "filter.h"
#include  <math.h>    //Keil library  

#define   uchar unsigned char
#define   uint unsigned int	

// ����MPU6050�ڲ���ַ
//****************************************
#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48

#define	PWR_MGMT_1		0x6B	//��Դ��������ֵ��0x00(��������)
#define	WHO_AM_I		0x75	//IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)
#define	MPU6050_Addr   0xD0	  //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�


//****************************


short A_X,A_Y,A_Z,G_X,G_Y,G_Z;		 //X,Y,Z��ֱ�Ӷ�����ֵ
float A_Xf,A_Yf,A_Zf,G_Xf,G_Yf,G_Zf,FYf,HGf,PHf; //�����X,Y,Z�ļ��ٶȣ����ٶ�  xyz��ӦFY,HG,PH
extern float angle_x,angle_y,angle_z;


unsigned char Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)		     //void
{
  	I2C_Start();
    I2C_Send_Byte(SlaveAddress);   //�����豸��ַ+д�ź�//I2C_Send_Byte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(I2C_Wait_Ack()){I2C_Stop(); return 0;}
    I2C_Send_Byte(REG_Address);   //���õ���ʼ��ַ      
    I2C_Wait_Ack();	
    I2C_Send_Byte(REG_data);
    I2C_Wait_Ack();   
    I2C_Stop(); 
    delay_ms(5);
    return 1;
}

//���ֽڶ�ȡ*****************************************
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   unsigned char REG_data;     	
		I2C_Start();
    I2C_Send_Byte(SlaveAddress); //I2C_Send_Byte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(I2C_Wait_Ack()){I2C_Stop(); return 0;}
    I2C_Send_Byte((u8) REG_Address);   //���õ���ʼ��ַ      
    I2C_Wait_Ack();
    I2C_Start();
    I2C_Send_Byte(SlaveAddress+1);
    I2C_Wait_Ack();
		REG_data= I2C_Read_Byte(0);
    I2C_Stop();
		return REG_data;
}						      


//��ʼ��MPU6050��������Ҫ��ο�pdf�����޸�************************
void Init_MPU6050(void)
{
  Single_Write(MPU6050_Addr,PWR_MGMT_1, 0x00);	//�������״̬
	Single_Write(MPU6050_Addr,SMPLRT_DIV, 0x07);
	Single_Write(MPU6050_Addr,CONFIG, 0x06);
	Single_Write(MPU6050_Addr,GYRO_CONFIG, 0x18);		 //0x00 0x08 0x10 0x18   
	Single_Write(MPU6050_Addr,ACCEL_CONFIG, 0x08);  //0x00 0x08 0x10 0x18 
}
	
//******��ȡMPU6050����****************************************
void READ_MPU6050(void)
{
	 unsigned char BUF[2];
   BUF[0]=Single_Read(MPU6050_Addr,ACCEL_XOUT_L); 
   BUF[1]=Single_Read(MPU6050_Addr,ACCEL_XOUT_H);
   A_X=	(BUF[1]<<8)|BUF[0];
   A_Xf=(float)A_X/16383.5; 						   //��ȡ����X������
   BUF[0]=Single_Read(MPU6050_Addr,ACCEL_YOUT_L);
   BUF[1]=Single_Read(MPU6050_Addr,ACCEL_YOUT_H);
   A_Y=	(BUF[1]<<8)|BUF[0];
   A_Yf=(float)A_Y/16383.5; 						   //��ȡ����Y������
   BUF[0]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_L);
   BUF[1]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_H);
   A_Z=	(BUF[1]<<8)|BUF[0];
   A_Zf=(float)A_Z/16383.5; 					       //��ȡ����Z������
	
	
	 BUF[0]=Single_Read(MPU6050_Addr,GYRO_XOUT_L); 
   BUF[1]=Single_Read(MPU6050_Addr,GYRO_XOUT_H);
   G_X=	(BUF[1]<<8)|BUF[0];
   G_Xf=(float)G_X/16.4; 						   //��ȡ����X������
   BUF[0]=Single_Read(MPU6050_Addr,GYRO_YOUT_L);
   BUF[1]=Single_Read(MPU6050_Addr,GYRO_YOUT_H);
   G_Y=	(BUF[1]<<8)|BUF[0];
   G_Yf=(float)G_Y/16.4;  						   //��ȡ����Y������
   BUF[0]=Single_Read(MPU6050_Addr,GYRO_ZOUT_L);
   BUF[1]=Single_Read(MPU6050_Addr,GYRO_ZOUT_H);
   G_Z=	(BUF[1]<<8)|BUF[0];
   G_Zf=(float)G_Z/16.4; 					       //��ȡ����Z������
}

/**************************************************************************
�������ܣ���ȡ�Ƕ� �����㷨�������ǵĵ�У�����ǳ����� 
��ڲ�������ȡ�ĸ��Ƕ�  x y z
����  ֵ����
**************************************************************************/
void Get_Angle(void)   //  �õ��˺ܶ��ȫ�ֱ���
{ 
	float temp_x,temp_y,temp_z;
	READ_MPU6050();
	temp_x=sqrt(A_X*A_X+A_Y*A_Y)/A_Z;
	PHf=atan(temp_x)*57.3;
	temp_y=A_Y/sqrt(A_X*A_X+A_Z*A_Z);
	HGf=atan(temp_y)*57.3;
	temp_z=A_X/sqrt(A_Z*A_Z+A_Y*A_Y);
	FYf=atan(temp_z)*57.3;
 // Kalman_Filter(A_Xf,G_Xf);//�������˲�		
	Yijielvbo(FYf,HGf,PHf,G_Xf,G_Yf,G_Zf);
//	Erjielvbo(A_Xf,G_Xf);
	OLED_ShowNum(0,0,(short)(angle_x*100),6,16);
	OLED_ShowNum(0,2,(short)(angle_y*100),6,16);
	OLED_ShowNum(0,4,(short)(angle_z*100),6,16);
}


int main(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; delay_init();	    	 //��ʱ������ʼ��	
	I2C_Init() ;
	OLED_Init();	
	delay_ms(10);
//  Init_MPU6050();		     //��ʼ��MPU6050
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��A�˿�ʱ��
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //��ʼ��GPIOD3,6
	
//	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	
		delay_ms(1000);
	Single_Write(0x2b,0x1C,0x80);
	
  while(1)
  {
		delay_ms(1000);
	Single_Write(0x2b,0x1C,0x80);
//		Get_Angle(); 
		delay_ms(5);
  }
}

/*************����***************/
