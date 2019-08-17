#ifndef _LED_H_
#define _LED_H_

#include "stm32f10x.h"

/* ����LED��ID��*/
#define LED_RED		0
#define LED_GREEN	1

/* ����LED��RCC��GPIO��PIN */
#define LED0_RCC		RCC_APB2Periph_GPIOA	
#define LED0_GPIO_PIN	GPIO_Pin_8
#define LED0_GPIO		GPIOA


#define LED1_RCC		RCC_APB2Periph_GPIOD	
#define LED1_GPIO_PIN	GPIO_Pin_2
#define LED1_GPIO		GPIOD

/* ����LED��״̬ */
typedef enum _led_status
{
	LED_ON = 0,
	LED_OFF
}LED_STATUS;

/**
  * @brief  �Ƴ�ʼ��.
  * @param  None
  * @retval None
  */
void Led_Init(void);

/**
  * @brief  ����LED��.
  * @param  led: ��ID��
  * @param  status: ��״̬.
  * @retval None
  */
void Led_Set(int led, LED_STATUS status);

#endif
