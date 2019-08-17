#include "IIC.h"
#include "sys.h"
#include "delay.h"

static void IIC_SendAck(void);
static void IIC_NoAck(void);


/****************************************************************************
* Function Name  : IIC_Config
* Description    : 初始化GPIO.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

void IIC_Config(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能A端口时钟
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOD3,6
}

/****************************************************************************
* Function Name  : IIC_Start
* Description    : IIC发送起始信号.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

void IIC_Start(void)
{
	IIC_SDA_OUT();
	
	IIC_SDA_SET;
	IIC_SCL_SET;
	delay_us(10);	

	IIC_SDA_CLR;	//START:when CLK is high,DATA change form high to low
	delay_us(10);	
	
	IIC_SCL_CLR;    //钳住IIC总线，准备发送或接收数据					
}

/****************************************************************************
* Function Name  : IIC_Stop
* Description    : IIC发送结束信号.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

void IIC_Stop(void)
{
	IIC_SDA_OUT();

	IIC_SCL_CLR;
	IIC_SDA_CLR;	//SCL在低电平期间，SDA变化无效

	IIC_SCL_SET;
	delay_us(10);	

	IIC_SDA_SET;    //STOP:when CLK is high DATA change form low to high
	delay_us(10);	
}

/****************************************************************************
* Function Name  : IIC_SendData
* Description    : IIC发送一个8位数据
* Input          : dat：发送的数据
* Output         : None
* Return         : None
****************************************************************************/

void IIC_SendData(uint8_t dat)
{
	uint8_t i;
	
	IIC_SDA_OUT();
	IIC_SCL_CLR;

	for(i=0; i<8; i++)//要发送8位，从最高位开始
	{
		if((dat & 0x80) == 0x80)
		{
			IIC_SDA_SET;
		}
		else
		{
			IIC_SDA_CLR;
		}
		dat <<= 1;
		delay_us(5);	

		IIC_SCL_SET;
		delay_us(10);	
		IIC_SCL_CLR;
		delay_us(5);	
	}	
}

/****************************************************************************
* Function Name  : IIC_ReceiveData
* Description    : IIC接收一个8位数据.
* Input          : ack：1：产生应答；0：不产生应答
* Output         : None
* Return         : 读取到的数据
****************************************************************************/

uint8_t IIC_ReceiveData(uint8_t ack)
{
	uint8_t i, readValue;

	IIC_SDA_OUT();
	IIC_SDA_SET;	
	IIC_SDA_IN();
	IIC_SCL_CLR;

	for(i=0; i<8; i++)	 //接收8个字节,从高到低
	{
		IIC_SCL_SET;
		delay_us(5);	
		readValue <<= 1;
		if(IIC_SDA != 0)
		{
			readValue |= 0x01;
		}		
		delay_us(5);	
		
		IIC_SCL_CLR;
		delay_us(10);			
	}
	
	if(ack)		 //是否应答
	{
		IIC_SendAck();
	}
	else
	{
		IIC_NoAck();
	}

	return readValue;
}

/****************************************************************************
* Function Name  : IIC_WaitAck
* Description    : 等待应答.
* Input          : None
* Output         : None
* Return         : 1：应答成功；0：应答失败
****************************************************************************/

int8_t IIC_WaitAck(void)
{
	uint32_t i;

	IIC_SDA_IN();
	IIC_SDA_SET;
	delay_us(20);	
	IIC_SCL_SET;
	if(IIC_SDA)
	{
		return 0xff;
	}
	IIC_SCL_CLR;
	delay_us(2);	
	return 0;	
}

/****************************************************************************
* Function Name  : IIC_SendAck
* Description    : 发送应答信号.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

static void IIC_SendAck(void)
{	
	IIC_SCL_CLR;
	IIC_SDA_OUT();
		
	IIC_SDA_CLR;
	delay_us(5);	
	IIC_SCL_SET;
	delay_us(5);	
	IIC_SCL_CLR;		
}

/****************************************************************************
* Function Name  : IIC_NoAck
* Description    : 无应答.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

static void IIC_NoAck(void)
{	
	IIC_SCL_CLR;
	IIC_SDA_OUT();
		
	IIC_SDA_SET;
	delay_us(5);	
	IIC_SCL_SET;
	delay_us(5);	
	IIC_SCL_CLR;		
}

void IIC_SendData_16(uint8_t ADDRESS,uint8_t Register,uint16_t Byte)  //仅适用于LDC1314
{
	IIC_Start();
	IIC_SendData((ADDRESS<<1)+0);
	IIC_WaitAck();
	IIC_SendData(Register);
	IIC_WaitAck();
	IIC_SendData((uint8_t)(Byte>>8));
	IIC_WaitAck();
	IIC_SendData((uint8_t)(Byte&0x00ff));
	IIC_WaitAck();
	delay_us(1);
	IIC_Stop();
}


