#ifndef __pulse_H
#define __pulse_H	
#include "stm32f10x.h"

void ADC_GPIO_Config(void);
void ADC_Mode_Config(void);
void ADC1_Init(void) ;
void TIM_Init(void);
u8 get_pulse(u16 ADC_Value[20]);


#endif


