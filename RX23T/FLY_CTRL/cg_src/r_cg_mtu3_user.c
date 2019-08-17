/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2015, 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_mtu3_user.c
* Version      : Code Generator for RX23T V1.00.04.02 [29 Nov 2016]
* Device(s)    : R5F523T5AxFM
* Tool-Chain   : CCRX
* Description  : This file implements device driver for MTU3 module.
* Creation Date: 2017/8/23
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_mtu3.h"
/* Start user code for include. Do not edit comment generated here */
#include "CUMT_sonar.h"
#include "CUMT_RC.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */

/* For MTU3_a2 pulse measurement */
static uint8_t  mtu3_ch2_a2_sta = 0U;
static uint32_t mtu3_ch2_a2_width = 0U;
static uint32_t mtu3_ch2_a2_cnt_start = 0U;
static uint32_t mtu3_ch2_a2_cnt_end = 0U;

/* For MTU3_b2 pulse measurement */
static uint8_t  mtu3_ch2_b2_sta = 0U;
static uint32_t mtu3_ch2_b2_width = 0U;
static uint32_t mtu3_ch2_b2_cnt_start = 0U;
static uint32_t mtu3_ch2_b2_cnt_end = 0U;

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_mtu3_tgia2_interrupt
* Description  : This function MTU3 TGIA2 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#if FAST_INTERRUPT_VECTOR == VECT_MTU2_TGIA2
#pragma interrupt r_mtu3_tgia2_interrupt(vect=VECT(MTU2,TGIA2),fint)
#else
#pragma interrupt r_mtu3_tgia2_interrupt(vect=VECT(MTU2,TGIA2))
#endif
static void r_mtu3_tgia2_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
		static uint32_t width_us = 0;
//		if(mtu3_ch2_a2_sta&0x80==0)
//		{
			if(mtu3_ch2_a2_sta&0x40)
			{
				mtu3_ch2_a2_cnt_end = CUMT_MTU3_Get_TCNT_CH2();
				mtu3_ch2_a2_sta |= 0x80;  // to avoid interruption
				if(mtu3_ch2_a2_cnt_end<mtu3_ch2_a2_cnt_start)  // timer counter has overflowed
				{
					mtu3_ch2_a2_width = mtu3_ch2_a2_cnt_end + 0x10001 - mtu3_ch2_a2_cnt_start;
				}
				else
				{
					mtu3_ch2_a2_width = mtu3_ch2_a2_cnt_end + 0x0001 - mtu3_ch2_a2_cnt_start;
				}
				width_us = mtu3_ch2_a2_width*8/10; // 0.8us for each counter
				// calculate 
				CUMT_RC_Get_Channel(width_us);
				MTU2.TIOR.BYTE &= 0xF8; // set mtu3_ch2_a2 to rising edge capture mode
				mtu3_ch2_a2_sta = 0x00; // release interrupt
			}
			else
			{
				mtu3_ch2_a2_cnt_start = CUMT_MTU3_Get_TCNT_CH2();
				MTU2.TIOR.BYTE |= 0x09; // set mtu3_ch2_a2 to falling edge capture mode
				mtu3_ch2_a2_sta = 0x40;
			}
//		}
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_mtu3_tgib2_interrupt
* Description  : This function MTU3 TGIB2 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#if FAST_INTERRUPT_VECTOR == VECT_MTU2_TGIB2
#pragma interrupt r_mtu3_tgib2_interrupt(vect=VECT(MTU2,TGIB2),fint)
#else
#pragma interrupt r_mtu3_tgib2_interrupt(vect=VECT(MTU2,TGIB2))
#endif
static void r_mtu3_tgib2_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
		static uint32_t width_us = 0;
//		if(mtu3_ch2_b2_sta&0x80==0)
//		{
			if(mtu3_ch2_b2_sta&0x40)
			{
				mtu3_ch2_b2_cnt_end = CUMT_MTU3_Get_TCNT_CH2();
				mtu3_ch2_b2_sta |= 0x80;  // to avoid interruption
				if(mtu3_ch2_b2_cnt_end<mtu3_ch2_b2_cnt_start)  // timer counter has overflowed
				{
					mtu3_ch2_b2_width = mtu3_ch2_b2_cnt_end + 0x10001 - mtu3_ch2_b2_cnt_start;
				}
				else
				{
					mtu3_ch2_b2_width = mtu3_ch2_b2_cnt_end + 0x0001 - mtu3_ch2_b2_cnt_start;
				}
				width_us = mtu3_ch2_b2_width*8/10; // 0.8us for each counter
				// calculate 
				CUMT_Sonar_Get_Height(width_us);
				MTU2.TIOR.BYTE &= 0x8F; // set mtu3_ch2_b2 to rising edge capture mode
				mtu3_ch2_b2_sta = 0x00; // release interrupt
			}
			else
			{
				mtu3_ch2_b2_cnt_start = CUMT_MTU3_Get_TCNT_CH2();
				MTU2.TIOR.BYTE |= 0x90; // set mtu3_ch2_b2 to falling edge capture mode
				mtu3_ch2_b2_sta = 0x40;
			}
//		}
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
uint16_t CUMT_MTU3_Get_TCNT_CH2(void)
{
	static uint16_t temp;
	temp = MTU2.TCNT;
	return temp;
}
/* End user code. Do not edit comment generated here */
