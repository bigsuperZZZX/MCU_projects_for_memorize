#ifndef _CUMT_RC_H
#define _CUMT_RC_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for function. Do not edit comment generated here */
#define RC_THR_MIN 1100
#define RC_THR_MAX 1951
#define RC_YAW_MIN 1120
#define RC_YAW_MAX 1860

volatile extern uint16_t RC_rol, RC_pit, RC_thr, RC_yaw;
volatile extern uint16_t RC_aux1, RC_aux2, RC_aux3, RC_aux4, RC_aux5, RC_aux6;

void CUMT_RC_Init(void);
void CUMT_RC_Stop(void);
void CUMT_RC_Get_Channel(uint32_t width);
/* End user code. Do not edit comment generated here */
#endif