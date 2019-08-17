
#include "r_cg_macrodriver.h"
#include "CUMT_delay.h"
#include "r_cg_cmt.h"

volatile uint8_t Delay_us_flag;

/*
void CUMT_Delay_us(int16_t cnt_us)
{
	int i;
	while(cnt_us--)
	{
		for(i=0;i<2;i++)
		{
			;
		}
	}
}
*/

// accurate delay function 
// Only use this function in the initialization part ortherwise unexpected error would happen
void CUMT_Delay_us(int16_t cnt_us)
{
	R_CMT1_Start();
	while(cnt_us)
	{
		Delay_us_flag = 1;
		while(Delay_us_flag)
		{
			nop();
		}
		cnt_us--;
	}
	R_CMT1_Stop();
}

void CUMT_Delay_ms(int16_t cnt_ms)
{
	while(cnt_ms--)
	{
		CUMT_Delay_us(1000);
	}
}