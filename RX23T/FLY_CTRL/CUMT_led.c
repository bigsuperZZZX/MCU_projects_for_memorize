
#include "r_cg_macrodriver.h"
#include "CUMT_led.h"
#include "CUMT_control.h"

void CUMT_LED_Init(void)
{
	LED0_ON;
	LED1_OFF;
	LED2_ON;
}

void CUMT_LED_Change(void)
{
	LED0 = ~LED0;
	
	if(is_Armed)
	{
		LED1 = ~LED1;
	}
	else
		LED1_ON;
		
	if(Fly_mode == MODE_MANUAL)
	{
		LED2_ON;
	}
	else
	{
		LED2 = ~LED2;
	}
}