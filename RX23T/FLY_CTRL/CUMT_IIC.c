
#include "r_cg_macrodriver.h"
#include "CUMT_IIC.h"
#include "CUMT_delay.h"

void CUMT_IIC_Delay(void)
{
	uint8_t i;
	for(i=0;i<2;i++)
	{
		nop();
	}
}

void CUMT_IIC_Init(void)
{
	SET_IIC_SDA_OUTPUT;
	IIC_SCL = 1;
	CUMT_IIC_Delay();
	IIC_SDA_O = 0;
	CUMT_IIC_Delay();
}

//START:when CLK is high,DATA change form high to low 
void CUMT_IIC_Start(void)
{  
	SET_IIC_SDA_OUTPUT;
	IIC_SDA_O = 1;
	IIC_SCL = 1;
	CUMT_IIC_Delay();
	IIC_SDA_O = 0;
	CUMT_IIC_Delay();
	IIC_SCL = 0;
}

//STOP:when CLK is high DATA change form low to high
void CUMT_IIC_Stop(void)
{
	SET_IIC_SDA_OUTPUT;
	IIC_SCL = 0;
	IIC_SDA_O = 0;
 	CUMT_IIC_Delay();
	IIC_SCL = 1; 
	IIC_SDA_O = 1;
	CUMT_IIC_Delay();
}


uint8_t CUMT_IIC_WaitAck(void)
{
	uint8_t ucErrTime = 0;
	SET_IIC_SDA_INPUT; 
	SET_IIC_SDA_PULLUP;
	IIC_SCL = 1;
	CUMT_IIC_Delay();
	while(IIC_SDA_I)
	{
		ucErrTime++;
		if(ucErrTime>50)
		{
			CUMT_IIC_Stop();
			return 1;
		}
	  CUMT_IIC_Delay();
	}
	IIC_SCL = 0;
	return 0;  
} 


void CUMT_IIC_Ack(void)
{
	SET_IIC_SDA_OUTPUT;
	IIC_SCL = 0;
	IIC_SDA_O = 0;
	CUMT_IIC_Delay();
	IIC_SCL = 1;
	CUMT_IIC_Delay();
	IIC_SCL = 0;
}
	

void CUMT_IIC_NAck(void)
{
	SET_IIC_SDA_OUTPUT;
	IIC_SCL = 0;
	IIC_SDA_O = 1;
	CUMT_IIC_Delay();
	IIC_SCL = 1;
	CUMT_IIC_Delay();
	IIC_SCL = 0;
}					 				     


void CUMT_IIC_Send_Byte(uint8_t txd)
{                        
	uint8_t t;   
	SET_IIC_SDA_OUTPUT;
	IIC_SCL = 0;
	for(t=0;t<8;t++)
	{              
		IIC_SDA_O = (txd&0x80)>>7;
		txd <<= 1; 	  
		CUMT_IIC_Delay();
		IIC_SCL = 1;
		CUMT_IIC_Delay();
		IIC_SCL = 0;	
		CUMT_IIC_Delay();
	}	 
} 	 
   

uint8_t CUMT_IIC_Read_Byte(unsigned char ack)
{
	uint8_t i, receive = 0;
	SET_IIC_SDA_INPUT;
	SET_IIC_SDA_PULLUP;
  	for(i=0;i<8;i++ )
	{
		IIC_SCL = 0; 
		CUMT_IIC_Delay();
		IIC_SCL = 1;
		receive <<= 1;
		if(IIC_SDA_I)receive++;
		CUMT_IIC_Delay();
	}					 
	if (ack)
			CUMT_IIC_Ack();
	else
			CUMT_IIC_NAck();
	return receive;
}

uint8_t CUMT_IIC_Read_Bytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
{
	uint8_t count = 0;
	
	CUMT_IIC_Start();
	CUMT_IIC_Send_Byte(dev);	  
	CUMT_IIC_WaitAck();
	CUMT_IIC_Send_Byte(reg);  
	CUMT_IIC_WaitAck();	  
	CUMT_IIC_Start();
	CUMT_IIC_Send_Byte(dev+1);
	CUMT_IIC_WaitAck();
	
	for(count=0;count<length;count++)
	{
		if(count!=length-1)data[count] = CUMT_IIC_Read_Byte(1);  //Read with ACK
		else  data[count] = CUMT_IIC_Read_Byte(0);	//Read without ACK
	}
	CUMT_IIC_Stop();
	return count;
}


uint8_t CUMT_IIC_Write_Bytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
{
 	uint8_t count = 0;
	CUMT_IIC_Start();
	CUMT_IIC_Send_Byte(dev);	  
	CUMT_IIC_WaitAck();
	CUMT_IIC_Send_Byte(reg);   
	CUMT_IIC_WaitAck();	  
	for(count=0;count<length;count++)
	{
		CUMT_IIC_Send_Byte(data[count]); 
		CUMT_IIC_WaitAck(); 
	}
	CUMT_IIC_Stop();

  return 1; //status == 0;
}


uint8_t CUMT_IIC_Write_Byte(uint8_t dev, uint8_t reg, uint8_t data)
{
   return CUMT_IIC_Write_Bytes(dev, reg, 1, &data);
}


uint8_t CUMT_IIC_Write_Bits(uint8_t dev, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t data)
{
	uint8_t b;
	if (CUMT_IIC_Read_Bytes(dev, reg, 1, &b) != 0)
	{
		uint8_t mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
		data <<= (8 - length);
		data >>= (7 - bitStart);
		b &= mask;
		b |= data;
		return CUMT_IIC_Write_Byte(dev, reg, b);
	} 
	else 
	{
		return 0;
	}
}


uint8_t CUMT_IIC_Write_Bit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data)
{
	uint8_t b;
	CUMT_IIC_Read_Bytes(dev, reg, 1, &b);
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	return CUMT_IIC_Write_Byte(dev, reg, b);
}