#include "HMC5883.H"


u8 HMC5883_BUF[6] = "0";

//==========================================
//��ʼ��HMC5883
//==========================================
void HMC5883_Init(void)
{
	I2C_init();
//	I2C_Write_Reg(0x00,0x70);
//	I2C_Write_Reg(0x01,0x00);
	I2C_Write_Reg(0x02,0x00);
}

//==========================================
//������ȡ6������
//==========================================
void HMC5883_Read_6Data(void)
{
	u8 i = 0;
	
	I2C_Start();                				//��ʼ�ź�
	I2C_Write(SlaveAddress);    	//�����豸��ַ+д�źţ�+0��
	I2C_Write(0x03);    				 	//���ʹ洢��Ԫ��ַ
	I2C_Start();                				//��ʼ�ź�
	I2C_Write(SlaveAddress+1); 	//�����豸��ַ+���źţ�+1��
	
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





