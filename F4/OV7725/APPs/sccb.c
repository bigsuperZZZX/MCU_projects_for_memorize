#include "sys.h"
#include "sccb.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F407������
//OVϵ������ͷ SCCB ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/14
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//��ʼ��SCCB�ӿ� 
void SCCB_Init(void)
{											      	 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*****ʱ���������******/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIOAʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD,&GPIO_InitStructure);   
}			 

//SCCB��ʼ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĸߵ���,ΪSCCB��ʼ�ź�
//�ڼ���״̬��,SDA��SCL��Ϊ�͵�ƽ
void SCCB_Start(void)
{
    SCCB_SDA=1;     //�����߸ߵ�ƽ	   
    SCCB_SCL=1;	    //��ʱ���߸ߵ�ʱ���������ɸ�����
    delay_us(120);  
    SCCB_SDA=0;
    delay_us(120);	 
    SCCB_SCL=0;	    //�����߻ָ��͵�ƽ��������������Ҫ	  
}

//SCCBֹͣ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĵ͵���,ΪSCCBֹͣ�ź�
//����״����,SDA,SCL��Ϊ�ߵ�ƽ
void SCCB_Stop(void)
{
    SCCB_SDA=0;
    delay_us(120);	 
    SCCB_SCL=1;	
    delay_us(120); 
    SCCB_SDA=1;	
    delay_us(120);
}  
//����NA�ź�
void SCCB_No_Ack(void)
{
	delay_us(120);
	SCCB_SDA=1;	
	SCCB_SCL=1;	
	delay_us(120);
	SCCB_SCL=0;	
	delay_us(120);
	SCCB_SDA=0;	
	delay_us(120);
}
//SCCB,д��һ���ֽ�
//����ֵ:0,�ɹ�;1,ʧ��. 
u8 SCCB_WR_Byte(u8 dat)
{
	u8 j,res;	 
	for(j=0;j<8;j++) //ѭ��8�η�������
	{
		if(dat&0x80)SCCB_SDA=1;	
		else SCCB_SDA=0;
		dat<<=1;
		delay_us(120);
		SCCB_SCL=1;	
		delay_us(120);
		SCCB_SCL=0;		   
	}			 
	SCCB_SDA_IN();		//����SDAΪ���� 
	delay_us(120);
	SCCB_SCL=1;			//���յھ�λ,���ж��Ƿ��ͳɹ�
	delay_us(120);
	if(SCCB_READ_SDA)res=1;  //SDA=1����ʧ�ܣ�����1
	else res=0;         //SDA=0���ͳɹ�������0
	SCCB_SCL=0;		 
	SCCB_SDA_OUT();		//����SDAΪ���    
	return res;  
}	 
//SCCB ��ȡһ���ֽ�
//��SCL��������,��������
//����ֵ:����������
u8 SCCB_RD_Byte(void)
{
	u8 temp=0,j;    
	SCCB_SDA_IN();		//����SDAΪ����  
	for(j=8;j>0;j--) 	//ѭ��8�ν�������
	{		     	  
		delay_us(120);
		SCCB_SCL=1;
		temp=temp<<1;
		if(SCCB_READ_SDA)temp++;   
		delay_us(120);
		SCCB_SCL=0;
	}	
	SCCB_SDA_OUT();		//����SDAΪ���    
	return temp;
} 							    
//д�Ĵ���
//����ֵ:0,�ɹ�;1,ʧ��.
u8 SCCB_WR_Reg(u8 reg,u8 data)
{
	u8 res=0;
	SCCB_Start(); 					//����SCCB����
	delay_us(220);	
	if(SCCB_WR_Byte(SCCB_ID))res=1;	//д����ID	  
	delay_us(220);
  	if(SCCB_WR_Byte(reg))res=1;		//д�Ĵ�����ַ	  
	delay_us(220);
  	if(SCCB_WR_Byte(data))res=1; 	//д����	 
	delay_us(22);	
  	SCCB_Stop();	  
  	return	res;
}		  					    
//���Ĵ���
//����ֵ:�����ļĴ���ֵ
u8 SCCB_RD_Reg(u8 reg)
{
	u8 val=0;
	SCCB_Start(); 				//����SCCB����
	delay_us(22);	
	SCCB_WR_Byte(SCCB_ID);		//д����ID	  
	delay_us(220);	 
  	SCCB_WR_Byte(reg);			//д�Ĵ�����ַ	  
	delay_us(220);	  
	SCCB_Stop();   
	delay_us(220);	   
	//���üĴ�����ַ�󣬲��Ƕ�
	SCCB_Start();
	delay_us(22);	
	SCCB_WR_Byte(SCCB_ID|0X01);	//���Ͷ�����	  
	delay_us(220);
  	val=SCCB_RD_Byte();		 	//��ȡ����
	delay_us(22);	
  	SCCB_No_Ack();
	delay_us(22);	
  	SCCB_Stop();
  	return val;
}














