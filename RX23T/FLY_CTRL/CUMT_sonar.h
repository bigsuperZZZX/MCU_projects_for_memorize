#ifndef _CUMT_SONAR_H
#define _CUMT_SONAR_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
/* Start user code for function. Do not edit comment generated here */

#define SONAR_TRIG_H PORT9.PODR.BIT.B4 = 1
#define SONAR_TRIG_L PORT9.PODR.BIT.B4 = 0

volatile extern uint32_t Sonar_height;
volatile extern uint32_t Sonar_height_last;

void CUMT_Sonar_Init(void);
void CUMT_Sonar_Trig(void);
void CUMT_Sonar_Get_Height(uint32_t width_us);
void CUMT_Sonar_Stop(void);
/* End user code. Do not edit comment generated here */
#endif