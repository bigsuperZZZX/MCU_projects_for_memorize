#include "stm32f10x.h"
#include "led.h"
#include "KeyScan.h"
#include "delay.h"
#include "exti.h"	
#include "timer.h"

int main(void)
{
	
	LED_Init();
	GPIO_Write(GPIOC,(u16)0x00);
	TIMER_Init();
//	exti_init();
	while(1);
}
