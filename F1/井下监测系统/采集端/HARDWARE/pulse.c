#include "stm32f10x.h"
#include "pulse.h"
#include "delay.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

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

u8 get_pulse(u16 ADC_Value[60])
{
//	static char xinlv_last=50;
	static char xinlv2[4];
	static char top=0;
	char xinlv=0;
	u16 ADC_Value2[60];
	char i, pulse=0;
	char start,end;
	u32 sum_aver=0;
	ADC_Value2[0]=ADC_Value[0];
	ADC_Value2[1]=(ADC_Value[0]+ADC_Value[1]+ADC_Value[2])/3;
	ADC_Value2[59]=ADC_Value[59];
	ADC_Value2[58]=(ADC_Value[57]+ADC_Value[58]+ADC_Value[59])/3;
	for(i=2;i<=57;i++)
	{
		ADC_Value2[i]=(ADC_Value[i-2]+ADC_Value[i-1]+ADC_Value[i]+ADC_Value[i+1]+ADC_Value[i+2])/5;
	}
	for(i=0;i<=59;i++)
	{
		sum_aver += ADC_Value2[i];
	}
	sum_aver = sum_aver/60+20;
	for(i=1;i<=58;i++)
	{
		if(ADC_Value2[i] > ADC_Value2[i-1] && ADC_Value2[i] > ADC_Value2[i+1] && ADC_Value2[i]> sum_aver )
		{
			pulse++;
			start = i;
			break;
		}	
	}
	i+= 5;
	for(;i<=58;i++)
	{
		if(ADC_Value2[i] > ADC_Value2[i-1] && ADC_Value2[i] > ADC_Value2[i+1] && ADC_Value2[i]> sum_aver )
		{
			pulse++;
			end = i;
			break;
		}	
	}
	i+=1;
	for(;i<=58;i++)
	{
		if(ADC_Value2[i] > ADC_Value2[i-1] && ADC_Value2[i] > ADC_Value2[i+1] && ADC_Value2[i]> sum_aver )
		{
			pulse++;
		}	
	}
//	xinlv = (int)(60/((float)(end-start)*0.05)+0.5);
/*	if( xinlv - xinlv_last > 10 ||  xinlv_last - xinlv > 10)
	{
		return xinlv_last;
	}
	else;
	xinlv_last = xinlv;*/
	xinlv2[top]= pulse;
	top++;
	if(top == 4) top= 0;
	for(i=0;i<4;i++)
		xinlv += xinlv2[i];
	xinlv *=5;
	return xinlv;
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



