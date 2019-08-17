#include "LDC1314.h"
#include "delay.h"

u16 coil_0=0,coil_1=0,coil_2=0,coil_3=0;

void LDC_Write_Reg(u8 add,u16 value)
{

	 IIC_Start(); 
	IIC_Send_Byte(0X56);	   //����д����ADDR=0  ���ߵ�ַ0x2a������0x2b������
	IIC_Wait_Ack();
	IIC_Send_Byte(add);
	IIC_Wait_Ack();
	IIC_Send_Byte(value>>8);
	IIC_Wait_Ack();
	IIC_Send_Byte(value&0xff);
	IIC_Wait_Ack();
  IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

u16 LDC_Read_Reg(u8 add)
{
		u16 status;
	  u16 a,b;
	 IIC_Start(); 
	 IIC_Send_Byte(0X56);	   //����д����ADDR=0
	 IIC_Wait_Ack();
	 IIC_Send_Byte(add);
	 IIC_Wait_Ack();
	 IIC_Start(); 
	 IIC_Send_Byte(0X57);	   //����д����ADDR=0
	 IIC_Wait_Ack(); 
	 a=IIC_Read_Byte(1); 
	 b=IIC_Read_Byte(0);		
	 status=(a<<8)+b;
	 IIC_Stop();
	return (status);
}


void LDC1314Multi_init(void)
{
	 GPIO_InitTypeDef GPIO_InitStructure;   ///SDA  PG6   SCL  PG5
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��ʱ��
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11 ;
	 GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	 GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��io�ڣ��������
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//��ΪƬѡ�˿�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //PG8����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOG
//	GPIO_SetBits(GPIOG,GPIO_Pin_5);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//��ΪƬѡ�˿�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //PG8����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOG
	
	
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);//PG5��6����	
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	delay_ms(100);
	
	LDC_Write_Reg(0x08,0x04D6);//��ͨ����ʼ��
	LDC_Write_Reg(0x09,0x04D6);
	LDC_Write_Reg(0x0a,0x04D6);
	LDC_Write_Reg(0x0b,0x04D6);
	
	LDC_Write_Reg(0x10,0x000A);
	LDC_Write_Reg(0x11,0x000A);
	LDC_Write_Reg(0x12,0x000A);
	LDC_Write_Reg(0x13,0x000A);
	
	LDC_Write_Reg(0x14,0x1004);
	LDC_Write_Reg(0x15,0x1004);
	LDC_Write_Reg(0x16,0x1004);
	LDC_Write_Reg(0x17,0x1004);
	
	LDC_Write_Reg(0x19,0x0000);
	LDC_Write_Reg(0x1B,0xc20C);
	LDC_Write_Reg(0x1E,0x9000);
	LDC_Write_Reg(0x1F,0x9000);
	LDC_Write_Reg(0x1A,0x1401);
}

void LDC1314_DataPrepare(void)
{
	u16 	BUFF[2][5];
	while(1)
	{
		BUFF[0][0]=LDC_Read_Reg(0X02);
		delay_ms(50);
		BUFF[1][0]=LDC_Read_Reg(0X04);
		delay_ms(50);
		BUFF[0][1]=LDC_Read_Reg(0X02);
		delay_ms(50);
		BUFF[1][1]=LDC_Read_Reg(0X04);
		delay_ms(50);
		BUFF[0][2]=LDC_Read_Reg(0X02);
		delay_ms(50);
		BUFF[1][2]=LDC_Read_Reg(0X04);
		delay_ms(50);
		BUFF[0][3]=LDC_Read_Reg(0X02);
		delay_ms(50);
		BUFF[1][3]=LDC_Read_Reg(0X04);
		delay_ms(50);
		BUFF[0][4]=LDC_Read_Reg(0X02);
		delay_ms(50);
		BUFF[1][4]=LDC_Read_Reg(0X04);
		delay_ms(50);
		if(BUFF[0][0]==BUFF[0][1]&&BUFF[0][0]==BUFF[0][2]&&BUFF[0][0]==BUFF[0][3]&&BUFF[0][0]==BUFF[0][4]&&BUFF[1][0]==BUFF[1][1]&&BUFF[1][0]==BUFF[1][2]&&BUFF[1][0]==BUFF[1][3]&&BUFF[1][0]==BUFF[1][4])
			break;
	}
	coil_0=LDC_Read_Reg(0X00);
	coil_1=BUFF[0][0];
	coil_2=BUFF[1][0];
	coil_3=LDC_Read_Reg(0X06);
	
}


