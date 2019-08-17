#ifndef _CUMT_LED_H
#define _CUMT_LED_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for function. Do not edit comment generated here */

#define LED0		PORT7.PODR.BIT.B6
#define LED0_ON		PORT7.PODR.BIT.B6 = 0
#define LED0_OFF	PORT7.PODR.BIT.B6 = 1

#define LED1		PORT9.PODR.BIT.B2
#define LED1_ON		PORT9.PODR.BIT.B2 = 0
#define LED1_OFF	PORT9.PODR.BIT.B2 = 1

#define LED2		PORT9.PODR.BIT.B1
#define LED2_ON		PORT9.PODR.BIT.B1 = 0
#define LED2_OFF	PORT9.PODR.BIT.B1 = 1

/*
#define LED1		P5.0
#define LED1_ON		P5.0 = 1
#define LED1_OFF	P5.0 = 0

#define LED2		P5.1
#define LED2_ON		P5.1 = 1
#define LED2_OFF	P5.1 = 0
*/
void CUMT_LED_Init(void);
void CUMT_LED_Change(void);
/* End user code. Do not edit comment generated here */
#endif
