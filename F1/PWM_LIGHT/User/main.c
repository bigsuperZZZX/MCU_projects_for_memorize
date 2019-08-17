#include "stm32f10x.h"
#include "led.h"
#include "pwm.h"
#include "delay.h"

int main(void)
{
	u8 fx=1;
	u16 ti=0;
	pwm_init();
	{
		/*	while(1)
delay(1000);
		if(fx==1)
		{
			ti++;
			if(ti==6000)
				fx=0;
		}
		else
		{
			ti--;
			if(ti==0)
				fx=1;
		}*/
	TIM_SetCompare3(TIM2,1000);
	}
}
