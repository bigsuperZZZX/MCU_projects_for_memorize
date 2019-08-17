#include "HMC5883.H"


u8 HMC5883_BUF[6] = "0";

//==========================================
//初始化HMC5883
//==========================================
void HMC5883_Init(void)
{
	I2C_init();
//	I2C_Write_Reg(0x00,0x70);
//	I2C_Write_Reg(0x01,0x00);
	I2C_Write_Reg(0x02,0x00);
}

//==========================================
//连续读取6个数据
//==========================================
void HMC5883_Read_6Data(void)
{
	u8 i = 0;
	
	I2C_Start();                				//起始信号
	I2C_Write(SlaveAddress);    	//发送设备地址+写信号（+0）
	I2C_Write(0x03);    				 	//发送存储单元地址
	I2C_Start();                				//起始信号
	I2C_Write(SlaveAddress+1); 	//发送设备地址+读信号（+1）
	
	for(i = 0;i < 6;i++)
	{
		HMC5883_BUF[i] = I2C_Read();
		if(i == 5)
			I2C_SendACK(1);			
		else
			I2C_SendACK(0);
	}
	I2C_Stop();
}





