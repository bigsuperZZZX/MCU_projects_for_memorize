#include "stm32f10x.h"
#include "my_ADC.h"
#include "delay.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include <stdio.h>


vu16 AD_Value[N][M]; //用来存放ADC转换结果，也是DMA的目标地址
vu16 After_filter[M]; //用来存放求平均值之后的结果
int i;
uint16_t ADC_ConvertedValue;

void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
	/* Configure PC.0  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);				
}



void TIM_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
  NVIC_InitTypeDef NVIC_InitStructure; 
   
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
    
  TIM_TimeBaseStructure.TIM_Period = 100;//频率：72MHz  72000000/36000=2000   进入中断时间间隔  TIM_Period/（72M/(TIM_Prescaler+1)）s
  TIM_TimeBaseStructure.TIM_Prescaler = 35999;//36000-1=35999  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
      
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);    
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;    
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;    
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
  NVIC_Init(&NVIC_InitStructure);
	
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );  
  TIM_Cmd(TIM3,ENABLE);  
}

void ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 		     //ADC外设地址，ADC数据地址+偏移值	
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(&ADC_ConvertedValue);	                     //内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                                   //以外设为DMA源
	DMA_InitStructure.DMA_BufferSize = 1;                                                //数据转移量为1）
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	             //外设地址不递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;  			     //内存地址不递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	     //接下来的跟采集一路AD一样
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;										
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* ADC1 configuration */	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	                                      //扫描模式				
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			              //连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	
	ADC_InitStructure.ADC_NbrOfChannel = 1;                                               //ADC通道为4	 								
	ADC_Init(ADC1, &ADC_InitStructure);
	
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);           //初始四路AD通道
	
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	
	ADC_ResetCalibration(ADC1);
	
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	
	ADC_StartCalibration(ADC1);
	
	while(ADC_GetCalibrationStatus(ADC1));
	
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


void ADC1_Init(void)                                                                          //ADC初始化
{
	ADC_GPIO_Config();
	ADC_Mode_Config();
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE); //启动 DMA 通道
}


/////////////////////////////////////////


void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//PA0/1/2 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void RCC_Configuration(void)
{


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE ); //使能ADC1通道时钟，各个管脚时钟

	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //72M/6=12,ADC最大时间不能超过14M
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //使能DMA传输

}


void ADC1_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;

	ADC_DeInit(ADC1); //将外设 ADC1 的全部寄存器重设为缺省值


	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode =ENABLE; //模数转换工作在扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //模数转换工作在连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //外部触发转换关闭
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = M; //顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure); //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器


	//设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
	//ADC1,ADC通道x,规则采样顺序值为y,采样时间为239.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 3, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 4, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 5, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 6, ADC_SampleTime_239Cycles5 );

	// 开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）
	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE); //使能指定的ADC1

	ADC_ResetCalibration(ADC1); //复位指定的ADC1的校准寄存器

	while(ADC_GetResetCalibrationStatus(ADC1)); //获取ADC1复位校准寄存器的状态,设置状态则等待


	ADC_StartCalibration(ADC1); //开始指定ADC1的校准状态

	while(ADC_GetCalibrationStatus(ADC1)); //获取指定ADC1的校准程序,设置状态则等待

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


void DMA_Configuration(void)
{

	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel1); //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value; //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //内存作为数据传输的目的地
	DMA_InitStructure.DMA_BufferSize = N*M; //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //工作在循环缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //根据DMA_InitStruct中指定的参数初始化DMA的通道

	DMA_Cmd(DMA1_Channel1, ENABLE); //启动DMA通道
}


//配置所有外设
void Init_ADC_Periph(void)
{

	RCC_Configuration();

	GPIO_Configuration();

	ADC1_Configuration();

	DMA_Configuration();


}



u16 GetVolt(u16 advalue)

{

	return (u16)(advalue * 330 / 4096); //求的结果扩大了100倍，方便下面求出小数

}




void filter(void)
{
	u32 sum = 0;
	u8 count;
	for(i=0;i<12;i++)
	{

		for ( count=0;count<N;count++)
		{

			sum += AD_Value[count][i];

		}
		After_filter[i]=sum/N;
		sum=0;
	}

}




