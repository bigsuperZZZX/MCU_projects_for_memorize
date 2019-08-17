#ifndef _CUMT_DELAY_H
#define _CUMT_DELAY_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for function. Do not edit comment generated here */

extern volatile uint8_t Delay_us_flag;

void CUMT_Delay_us(int16_t cnt_us);
void CUMT_Delay_ms(int16_t cnt_ms);

/* End user code. Do not edit comment generated here */
#endif