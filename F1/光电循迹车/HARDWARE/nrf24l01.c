#include "nrf24l01.h"
#include "spi.h"
#include "sys.h"
#include "delay.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "oled.h"
#include <stdio.h>
#include "DataScope_DP.h"

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址

void nrf24l01_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    //CE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;    //IRQ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	EXTI_ClearITPendingBit(EXTI_Line9);
	
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource9);
  EXTI_InitStructure.EXTI_Line=EXTI_Line9;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键所在的外部中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2， 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure); 
	
	NRF24L01_CE=0; 			//使能24L01
	NRF24L01_CSN=1;			//SPI片选取消	 
}

void nrf24l01_tim_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源
	TIM_TimeBaseStructure.TIM_Period = 400;//设置自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;//设置用来作为TIMx时钟频率预分频值，100Khz计数频率
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);	
	TIM_Cmd(TIM3,ENABLE); //使能或者失能TIMx外设
	/* 设置中断参数，并打开中断 */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );	//使能或者失能指定的TIM中断
	
	/* 设置NVIC参数 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //打开TIM3_IRQn的全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;	//抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;  //响应优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能
	NVIC_Init(&NVIC_InitStructure);	
}


u8 NRF24L01_Check(void)
{
	u8 buf[5]={0Xa5,0Xa5,0Xa5,0Xa5,0Xa5};
	u8 i;
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
	delay_us(5);
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
	delay_us(10);
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//检测24L01错误	
	return 0;		 //检测到24L01
}	 	 

//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
  NRF24L01_CSN=0;                 //使能SPI传输
  status =SPI2_WriteReadData(reg);//发送寄存器号 
  SPI2_WriteReadData(value);      //写入寄存器的值
  NRF24L01_CSN=1;                 //禁止SPI传输	   
  return(status);       			//返回状态值
}
//读取SPI寄存器值
//reg:要读的寄存器
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
  NRF24L01_CSN = 0;          //使能SPI传输		
  SPI2_WriteReadData(reg);   //发送寄存器号
  reg_val=SPI2_WriteReadData(0XFF);//读取寄存器内容
  NRF24L01_CSN = 1;          //禁止SPI传输		    
  return(reg_val);           //返回状态值
}	
//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
  u8 status,u8_ctr;	       
  NRF24L01_CSN = 0;           //使能SPI传输
  status=SPI2_WriteReadData(reg);//发送寄存器值(位置),并读取状态值   	   
  for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_WriteReadData(0XFF);//读出数据
  NRF24L01_CSN=1;       //关闭SPI传输
  return status;        //返回读到的状态值
}
//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
  u8 status,u8_ctr;	    
 	NRF24L01_CSN = 0;          //使能SPI传输
  status = SPI2_WriteReadData(reg);//发送寄存器值(位置),并读取状态值
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_WriteReadData(*pBuf++); //写入数据	 
  NRF24L01_CSN = 1;       //关闭SPI传输
  return status;          //返回读到的状态值
}				   
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
 	//SPI1_SetSpeed(SPI_BaudRatePrescaler_8);//spi速度为10.5Mhz（24L01的最大SPI时钟为10Mhz）   
	NRF24L01_CE=0;
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
 	NRF24L01_CE=1;//启动发送	   
	while(NRF24L01_IRQ!=0);//等待发送完成
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(sta&TX_OK)//发送完成
	{
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		    							   
	//SPI1_SetSpeed(SPI_BaudRatePrescaler_8); //spi速度为10.5Mhz（24L01的最大SPI时钟为10Mhz）   
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&RX_OK)//接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
		return 0; 
	} 
	return 1;//没收到任何数据
}					    
//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void NRF24L01_RX_Mode(void)
{
	u8 temp[32];
  NRF24L01_CE=0;	  
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址  	 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //设置RF通信频率		  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
  NRF24L01_CE = 1; //CE为高,进入接收模式 
	NRF24L01_RxPacket(temp);
}						 
//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE=0;	    
  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  

  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //设置RF通道为40
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_CE=1;//CE为高,10us后启动发送
}

extern float cur_x,cur_x_v;
extern long pulse_r,pulse_l;
extern char dir_r,dir_l;  
extern short pwm_r,pwm_l;
extern float set_x;
extern long pulse_r,pulse_l;
extern struct
{
	float SumError;
	float LastError;
	float Proportion;
	float Integral;
	float Derivative;	
}pid_r,pid_l;

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line9)==SET)
	{
		u8 tmp_buf[33];
		signed char i=0;
		EXTI_ClearITPendingBit(EXTI_Line9);
		NRF24L01_RxPacket(tmp_buf);
	/********************/
		while(tmp_buf[i]!='#')
			i++;
		tmp_buf[i]='\0'; 
		for(i--;i>=0;i--)
		{
			if((tmp_buf[i]<'0'||tmp_buf[i]>'9')&&tmp_buf[i]!='-'&&tmp_buf[i]!='.')
			{
				switch (tmp_buf[i])
					{
						case 'a' : pid_r.Proportion=atoi(&tmp_buf[i+1]);  break;
						case 'b' : pid_r.Integral=atof(&tmp_buf[i+1]);    break;
						case 'c' : pid_r.Derivative=atof(&tmp_buf[i+1]);  break;
						case 'd' : set_x=atof(&tmp_buf[i+1]);             break;
						default :																				  break;
					}
				tmp_buf[i]='\0';
			}
		} 	
	/********************/
	}
}


void TIM3_IRQHandler()
{
	u8 statue,tx_buff[32];
	u8 words[64];  //oled显示缓冲
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
  EXTI->IMR&=~(1<<9);//屏蔽line9上的中断  
	NRF24L01_TX_Mode();
	//sprintf(tx_buff,"cur_x:%5.2f cur_x_v:%5.2f",cur_x,cur_x_v);
	tx_buff_prepare_Balance(tx_buff);
	statue=NRF24L01_TxPacket(tx_buff);	
	if(statue==MAX_TX)
		OLED_ShowString(0,4,"MX_TX");
	else if(statue!=TX_OK)
		OLED_ShowString(0,4,"TX_Wrong!");
	NRF24L01_RX_Mode();
	EXTI->IMR|=1<<9;//不屏蔽line9上的中断
	
//sprintf(words,"               |               |               |               |"); 
	sprintf(words,"%7.2f  %7.2fpwml%4dpwmr%4d%6ld  r%6ld",cur_x,cur_x_v,pwm_l,pwm_r,pulse_l,pulse_r); //向OLED显示缓冲中填内容
	OLED_ShowString(0,0,words);
}

u8* tx_buff_prepare_Hunter(u8 *pbuff)
{
	signed short temp;
	temp=(signed short)(cur_x*100);
	*(pbuff+0)=temp>>8;
	*(pbuff+1)=temp&0x00ff;
	temp=(signed short)(cur_x_v*100);
	*(pbuff+2)=temp>>8;
	*(pbuff+3)=temp&0x00ff;
	*(pbuff+4)='\0';
	return pbuff;
}

u8* tx_buff_prepare_Balance(u8 *pbuff)
{
	DataScope_Get_Channel_Data(pbuff,cur_x, 1 );
	DataScope_Get_Channel_Data(pbuff,cur_x_v, 2 );
	DataScope_Data_Generate(pbuff,2);
	return pbuff;
}

