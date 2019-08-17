#include "delay.h"
#include "oled.h"
#include "math.h"
#include "stm32f10x.h"
#include "IIC.h"
#include "filter.h"
#include  <math.h>    //Keil library  

#define   uchar unsigned char
#define   uint unsigned int	

// 定义MPU6050内部地址
//****************************************
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
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

#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I		0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	MPU6050_Addr   0xD0	  //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改


//****************************


short A_X,A_Y,A_Z,G_X,G_Y,G_Z;		 //X,Y,Z轴直接读出的值
float A_Xf,A_Yf,A_Zf,G_Xf,G_Yf,G_Zf,FYf,HGf,PHf; //计算后X,Y,Z的加速度，角速度  xyz对应FY,HG,PH
extern float angle_x,angle_y,angle_z;


unsigned char Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)		     //void
{
  	I2C_Start();
    I2C_Send_Byte(SlaveAddress);   //发送设备地址+写信号//I2C_Send_Byte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//设置高起始地址+器件地址 
    if(I2C_Wait_Ack()){I2C_Stop(); return 0;}
    I2C_Send_Byte(REG_Address);   //设置低起始地址      
    I2C_Wait_Ack();	
    I2C_Send_Byte(REG_data);
    I2C_Wait_Ack();   
    I2C_Stop(); 
    delay_ms(5);
    return 1;
}

//单字节读取*****************************************
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   unsigned char REG_data;     	
		I2C_Start();
    I2C_Send_Byte(SlaveAddress); //I2C_Send_Byte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//设置高起始地址+器件地址 
    if(I2C_Wait_Ack()){I2C_Stop(); return 0;}
    I2C_Send_Byte((u8) REG_Address);   //设置低起始地址      
    I2C_Wait_Ack();
    I2C_Start();
    I2C_Send_Byte(SlaveAddress+1);
    I2C_Wait_Ack();
		REG_data= I2C_Read_Byte(0);
    I2C_Stop();
		return REG_data;
}						      


//初始化MPU6050，根据需要请参考pdf进行修改************************
void Init_MPU6050(void)
{
  Single_Write(MPU6050_Addr,PWR_MGMT_1, 0x00);	//解除休眠状态
	Single_Write(MPU6050_Addr,SMPLRT_DIV, 0x07);
	Single_Write(MPU6050_Addr,CONFIG, 0x06);
	Single_Write(MPU6050_Addr,GYRO_CONFIG, 0x18);		 //0x00 0x08 0x10 0x18   
	Single_Write(MPU6050_Addr,ACCEL_CONFIG, 0x08);  //0x00 0x08 0x10 0x18 
}
	
//******读取MPU6050数据****************************************
void READ_MPU6050(void)
{
	 unsigned char BUF[2];
   BUF[0]=Single_Read(MPU6050_Addr,ACCEL_XOUT_L); 
   BUF[1]=Single_Read(MPU6050_Addr,ACCEL_XOUT_H);
   A_X=	(BUF[1]<<8)|BUF[0];
   A_Xf=(float)A_X/16383.5; 						   //读取计算X轴数据
   BUF[0]=Single_Read(MPU6050_Addr,ACCEL_YOUT_L);
   BUF[1]=Single_Read(MPU6050_Addr,ACCEL_YOUT_H);
   A_Y=	(BUF[1]<<8)|BUF[0];
   A_Yf=(float)A_Y/16383.5; 						   //读取计算Y轴数据
   BUF[0]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_L);
   BUF[1]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_H);
   A_Z=	(BUF[1]<<8)|BUF[0];
   A_Zf=(float)A_Z/16383.5; 					       //读取计算Z轴数据
	
	
	 BUF[0]=Single_Read(MPU6050_Addr,GYRO_XOUT_L); 
   BUF[1]=Single_Read(MPU6050_Addr,GYRO_XOUT_H);
   G_X=	(BUF[1]<<8)|BUF[0];
   G_Xf=(float)G_X/16.4; 						   //读取计算X轴数据
   BUF[0]=Single_Read(MPU6050_Addr,GYRO_YOUT_L);
   BUF[1]=Single_Read(MPU6050_Addr,GYRO_YOUT_H);
   G_Y=	(BUF[1]<<8)|BUF[0];
   G_Yf=(float)G_Y/16.4;  						   //读取计算Y轴数据
   BUF[0]=Single_Read(MPU6050_Addr,GYRO_ZOUT_L);
   BUF[1]=Single_Read(MPU6050_Addr,GYRO_ZOUT_H);
   G_Z=	(BUF[1]<<8)|BUF[0];
   G_Zf=(float)G_Z/16.4; 					       //读取计算Z轴数据
}

/**************************************************************************
函数功能：获取角度 三种算法经过我们的调校，都非常理想 
入口参数：获取哪个角度  x y z
返回  值：无
**************************************************************************/
void Get_Angle(void)   //  用到了很多的全局变量
{ 
	float temp_x,temp_y,temp_z;
	READ_MPU6050();
	temp_x=sqrt(A_X*A_X+A_Y*A_Y)/A_Z;
	PHf=atan(temp_x)*57.3;
	temp_y=A_Y/sqrt(A_X*A_X+A_Z*A_Z);
	HGf=atan(temp_y)*57.3;
	temp_z=A_X/sqrt(A_Z*A_Z+A_Y*A_Y);
	FYf=atan(temp_z)*57.3;
 // Kalman_Filter(A_Xf,G_Xf);//卡尔曼滤波		
	Yijielvbo(FYf,HGf,PHf,G_Xf,G_Yf,G_Zf);
//	Erjielvbo(A_Xf,G_Xf);
	OLED_ShowNum(0,0,(short)(angle_x*100),6,16);
	OLED_ShowNum(0,2,(short)(angle_y*100),6,16);
	OLED_ShowNum(0,4,(short)(angle_z*100),6,16);
}


int main(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; delay_init();	    	 //延时函数初始化	
	I2C_Init() ;
	OLED_Init();	
	delay_ms(10);
//  Init_MPU6050();		     //初始化MPU6050
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能A端口时钟
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOD3,6
	
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

/*************结束***************/
