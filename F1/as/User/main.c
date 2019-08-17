#include "stm32f10x.h"
#include "pwm.h"
#include "delay.h"

int main(void)
{
	pwm_init();
	while(1)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_8);
		delay_ms(200);
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);	
		delay_ms(200);
	}
}
