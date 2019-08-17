#include "IIC.h"
#include "sys.h"
#include "delay.h"

static void IIC_SendAck(void);
static void IIC_NoAck(void);


/****************************************************************************
* Function Name  : IIC_Config
* Description    : ��ʼ��GPIO.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

void IIC_Config(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��A�˿�ʱ��
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //��ʼ��GPIOD3,6
}

/****************************************************************************
* Function Name  : IIC_Start
* Description    : IIC������ʼ�ź�.
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
	
	IIC_SCL_CLR;    //ǯסIIC���ߣ�׼�����ͻ��������					
}

/****************************************************************************
* Function Name  : IIC_Stop
* Description    : IIC���ͽ����ź�.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

void IIC_Stop(void)
{
	IIC_SDA_OUT();

	IIC_SCL_CLR;
	IIC_SDA_CLR;	//SCL�ڵ͵�ƽ�ڼ䣬SDA�仯��Ч

	IIC_SCL_SET;
	delay_us(10);	

	IIC_SDA_SET;    //STOP:when CLK is high DATA change form low to high
	delay_us(10);	
}

/****************************************************************************
* Function Name  : IIC_SendData
* Description    : IIC����һ��8λ����
* Input          : dat�����͵�����
* Output         : None
* Return         : None
****************************************************************************/

void IIC_SendData(uint8_t dat)
{
	uint8_t i;
	
	IIC_SDA_OUT();
	IIC_SCL_CLR;

	for(i=0; i<8; i++)//Ҫ����8λ�������λ��ʼ
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
* Description    : IIC����һ��8λ����.
* Input          : ack��1������Ӧ��0��������Ӧ��
* Output         : None
* Return         : ��ȡ��������
****************************************************************************/

uint8_t IIC_ReceiveData(uint8_t ack)
{
	uint8_t i, readValue;

	IIC_SDA_OUT();
	IIC_SDA_SET;	
	IIC_SDA_IN();
	IIC_SCL_CLR;

	for(i=0; i<8; i++)	 //����8���ֽ�,�Ӹߵ���
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
	
	if(ack)		 //�Ƿ�Ӧ��
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
* Description    : �ȴ�Ӧ��.
* Input          : None
* Output         : None
* Return         : 1��Ӧ��ɹ���0��Ӧ��ʧ��
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
* Description    : ����Ӧ���ź�.
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
* Description    : ��Ӧ��.
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

void IIC_SendData_16(uint8_t ADDRESS,uint8_t Register,uint16_t Byte)  //��������LDC1314
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


