#include "led.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

/**
  * @brief  µ∆≥ı ºªØ.
  * @param  None
  * @retval None
  */
void Led_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(LED0_RCC | LED1_RCC, ENABLE);	 
		
	GPIO_InitStructure.GPIO_Pin = LED0_GPIO_PIN;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(LED0_GPIO, &GPIO_InitStructure);					 
	GPIO_SetBits(LED0_GPIO, LED0_GPIO_PIN);	

	GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(LED1_GPIO, &GPIO_InitStructure);					 
	GPIO_SetBits(LED1_GPIO, LED1_GPIO_PIN);	
	
}

/**
  * @brief  …Ë÷√LEDµ∆.
  * @param  led: µ∆ID∫≈
  * @param  status: µ∆◊¥Ã¨.
  * @retval None
  */
void Led_Set(int led, LED_STATUS status)
{
	switch(led)
	{
		case LED_RED:
			if(LED_ON == status)
			{
				GPIO_ResetBits(LED0_GPIO, LED0_GPIO_PIN);
			}
			else
			{
				GPIO_SetBits(LED0_GPIO, LED0_GPIO_PIN);
			}
			break;
		case LED_GREEN:
			if(LED_ON == status)
			{
				GPIO_ResetBits(LED1_GPIO, LED1_GPIO_PIN);
			}
			else
			{
				GPIO_SetBits(LED1_GPIO, LED1_GPIO_PIN);
			}
			break;
		default:
			break;
	}
}
