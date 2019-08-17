#ifndef __pulse_H
#define __pulse_H	
#include "stm32f10x.h"

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)
#define N 50 //每通道采50次
#define M 6 //为6个通道

void ADC_GPIO_Config(void);
void ADC_Mode_Config(void);
void ADC1_Init(void) ;
void TIM_Init(void);
u8 get_pulse(u16 ADC_Value[20]);
void filter(void);
void GPIO_Configuration(void);
void RCC_Configuration(void);
void ADC1_Configuration(void);
void DMA_Configuration(void);
void Init_ADC_Periph(void);
u16 GetVolt(u16 advalue);
void filter(void);



#endif


