#include "sys.h"
#include "PWM_to_PPM.h"


extern u8 RC_GetCplt;

int main(void)
{ 
	delay_init();                 //ÑÓÊ±³õÊ¼»¯
	CAP_GPIO_init();
	sys_pulse();

	while(1)
	{
		/*if((u16)GPIOA->IDR & 0x7F)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_7);
		}
		else
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		}*/
	}
}

