
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "CUMT_sonar.h"

volatile uint32_t Sonar_height = 0; //unit: mm
volatile uint32_t Sonar_height_last = 0; //unit: mm  Used for filtering
extern uint16_t Keep_Alt_thr;


void CUMT_Sonar_Init(void)
{
	SONAR_TRIG_L;
}

void CUMT_Sonar_Trig(void)
{
	int i;
	SONAR_TRIG_H;
	for(i=33;i>0;i--)
	{
		;
	}
	SONAR_TRIG_L;
}

void CUMT_Sonar_Get_Height(uint32_t width_us)
{
	Sonar_height = width_us*17/100;  //unit: mm
	if(Sonar_height > 2000 || Sonar_height < 100 )
	{
		Sonar_height = Sonar_height_last;
	}
	else if(Sonar_height_last - Sonar_height < 63 && Sonar_height_last - Sonar_height > 41)
	{
		Sonar_height += 52;
	}
	else if(Sonar_height > 180 && Sonar_height < 220 && Sonar_height_last > 600)
	{
		Sonar_height = Sonar_height_last;
	}
	else
	{
		Sonar_height = 0.3 * Sonar_height_last + 0.7 * Sonar_height;  //filtering
	}
	Sonar_height_last = Sonar_height;
}

void CUMT_Sonar_Stop(void)
{
	SONAR_TRIG_L;
}