#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "usart.h"

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

uint16_t ADC_ConvertedValue[2];

void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
	/* Configure PC.0  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);				
}


void ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 		     //ADC外设地址，ADC数据地址+偏移值	
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue;	                     //内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                                   //以外设为DMA源
	DMA_InitStructure.DMA_BufferSize = 2;                                                //数据转移量为4（4路AD）
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	             //外设地址不递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			     //内存地址递增（将4路AD只放在一个数组里）
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
	ADC_InitStructure.ADC_NbrOfChannel = 2;                                               //ADC通道为4	 								
	ADC_Init(ADC1, &ADC_InitStructure);
	
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);           //初始四路AD通道
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	
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

void KEY_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);				
}

void ADC1_Init(void)                                                                          //ADC初始化
{
	ADC_GPIO_Config();
	ADC_Mode_Config();
}


 int main(void)
 {
	 u8 send_value=0;    //0左右中0前后中
	 delay_init();	    	 //延时函数初始化	
	 ADC1_Init();
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		DMA_Cmd(DMA1_Channel1, ENABLE); //启动 DMA 通道
	 KEY_init();
	 uart_init(115200);
	 while(1)
	 {
		 if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7))
		 {
				if(ADC_ConvertedValue[1] > 4000) send_value &= 0xF0 , send_value |= 0x04;
			  if(ADC_ConvertedValue[1] > 100 && ADC_ConvertedValue[1] <= 4000) send_value &= 0xF0 , send_value |= 0x01;
			  if(ADC_ConvertedValue[1] <=100) send_value &= 0xF0 , send_value |= 0x02;
			 
			  if(ADC_ConvertedValue[0] > 4000) send_value &= 0x0F , send_value |= 0x20;
		 	  if(ADC_ConvertedValue[0] > 100 && ADC_ConvertedValue[0] <= 4000) send_value &= 0x0F , send_value |= 0x10;
			  if(ADC_ConvertedValue[0] <=100) send_value &= 0x0F , send_value |= 0x40;
			 
			  USART_SendData(USART1,send_value);
		 }
		 else
		 {
			  USART_SendData(USART1,0x88);
		 }
		
		 delay_ms(50);
	 }
}

