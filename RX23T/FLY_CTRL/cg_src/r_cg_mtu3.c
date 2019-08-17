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
* File Name    : r_cg_mtu3.c
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
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_MTU3_Create
* Description  : This function initializes the MTU3 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MTU3_Create(void)
{
    /* Cancel MTU stop state in LPC */
    MSTP(MTU) = 0U;

    /* Enable read/write to MTU registers */
    MTU.TRWERA.BYTE = _01_MTU_RWE_ENABLE;

    /* Stop all channels */
    MTU.TSTRA.BYTE = _00_MTU_CST0_OFF | _00_MTU_CST1_OFF | _00_MTU_CST2_OFF | _00_MTU_CST3_OFF | _00_MTU_CST4_OFF;
    MTU5.TSTR.BYTE = _00_MTU_CSTW5_OFF | _00_MTU_CSTV5_OFF | _00_MTU_CSTU5_OFF;

    /* Set interrupt priority level */
    IPR(MTU2, TGIA2) = _0E_MTU_PRIORITY_LEVEL14;

    /* Channel 0 is used as PWM2 mode */
    MTU0.TCR.BYTE = _02_MTU_PCLK_16 | _00_MTU_CKEG_RISE | _20_MTU_CKCL_A;
    MTU0.TCR2.BYTE = _00_MTU_PCLK_1;
    MTU.TSYRA.BIT.SYNC0 = 0U;
    MTU0.TMDR1.BYTE = _03_MTU_PWM2;
    MTU0.TIORH.BYTE = _50_MTU_IOB_HL | _00_MTU_IOA_DISABLE;
    MTU0.TIORL.BYTE = _50_MTU_IOD_HL | _05_MTU_IOC_HL;
    MTU0.TGRA = _C34F_TGRA_VALUE;
    MTU0.TGRB = _0A8C_TGRB_VALUE;
    MTU0.TGRC = _0FD2_TGRC_VALUE;
    MTU0.TGRD = _0FD2_TGRD_VALUE;
    MTU0.TGRE = _0064_TGRE_VALUE;
    MTU0.TGRF = _0064_TGRF_VALUE;
    MTU0.TIER2.BYTE = _00_MTU_TGIEE_DISABLE | _00_MTU_TGIEF_DISABLE | _00_MTU_TTGE2_DISABLE;
    MTU0.TIER.BYTE = _00_MTU_TGIEA_DISABLE | _00_MTU_TGIEB_DISABLE | _00_MTU_TGIEC_DISABLE | _00_MTU_TGIED_DISABLE | 
                     _00_MTU_TCIEU_DISABLE | _00_MTU_TCIEV_DISABLE | _00_MTU_TTGE_DISABLE;

    /* Channel 1 is used as PWM2 mode */
    MTU1.TCR.BYTE = _02_MTU_PCLK_16 | _00_MTU_CKEG_RISE | _20_MTU_CKCL_A;
    MTU1.TCR2.BYTE = _00_MTU_PCLK_1;
    MTU.TSYRA.BIT.SYNC1 = 0U;
    MTU1.TMDR1.BYTE = _03_MTU_PWM2;
    MTU1.TIOR.BYTE = _50_MTU_IOB_HL | _00_MTU_IOA_DISABLE;
    MTU1.TGRA = _C34F_TGRA_VALUE;
    MTU1.TGRB = _0E74_TGRB_VALUE;
    MTU1.TIER.BYTE = _00_MTU_TGIEA_DISABLE | _00_MTU_TGIEB_DISABLE | _00_MTU_TGIEC_DISABLE | _00_MTU_TGIED_DISABLE | 
                     _00_MTU_TCIEU_DISABLE | _00_MTU_TCIEV_DISABLE | _00_MTU_TTGE_DISABLE;

    /* Channel 2 is used as normal mode */
    MTU.TSYRA.BIT.SYNC2 = 0U;
    MTU2.TCR.BYTE = _00_MTU_CKEG_RISE | _00_MTU_CKCL_DIS;
    MTU2.TCR2.BYTE = _03_MTU_PCLK_32;
    MTU2.TIER.BYTE = _01_MTU_TGIEA_ENABLE | _02_MTU_TGIEB_ENABLE | _00_MTU_TGIEC_DISABLE | _00_MTU_TGIED_DISABLE | 
                     _00_MTU_TCIEU_DISABLE | _00_MTU_TCIEV_DISABLE | _00_MTU_TTGE_DISABLE;
    MTU2.TMDR1.BYTE = _00_MTU_NORMAL;
    MTU2.TIOR.BYTE = _08_MTU_IOA_IR | _80_MTU_IOB_IR;

    /* Disable read/write to MTU registers */
    MTU.TRWERA.BYTE = _00_MTU_RWE_DISABLE;

    /* Set MTIOC0B pin */
    MPC.PB2PFS.BYTE = 0x01U;
    PORTB.PMR.BYTE |= 0x04U;
    /* Set MTIOC0C pin */
    MPC.PB1PFS.BYTE = 0x01U;
    PORTB.PMR.BYTE |= 0x02U;
    /* Set MTIOC0D pin */
    MPC.PB0PFS.BYTE = 0x01U;
    PORTB.PMR.BYTE |= 0x01U;
    /* Set MTIOC1B pin */
    MPC.PA4PFS.BYTE = 0x01U;
    PORTA.PMR.BYTE |= 0x10U;
    /* Set MTIOC2A pin */
    MPC.PA3PFS.BYTE = 0x01U;
    PORTA.PMR.BYTE |= 0x08U;
    /* Set MTIOC2B pin */
    MPC.PA2PFS.BYTE = 0x01U;
    PORTA.PMR.BYTE |= 0x04U;
}
/***********************************************************************************************************************
* Function Name: R_MTU3_C0_Start
* Description  : This function starts MTU3 channel 0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MTU3_C0_Start(void)
{
    MTU.TSTRA.BYTE |= _01_MTU_CST0_ON;
}
/***********************************************************************************************************************
* Function Name: R_MTU3_C0_Stop
* Description  : This function stops MTU3 channel 0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MTU3_C0_Stop(void)
{
    MTU.TSTRA.BIT.CST0 = 0U;
}
/***********************************************************************************************************************
* Function Name: R_MTU3_C1_Start
* Description  : This function starts MTU3 channel 1 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MTU3_C1_Start(void)
{
    MTU.TSTRA.BYTE |= _02_MTU_CST1_ON;
}
/***********************************************************************************************************************
* Function Name: R_MTU3_C1_Stop
* Description  : This function stops MTU3 channel 1 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MTU3_C1_Stop(void)
{
    MTU.TSTRA.BIT.CST1 = 0U;
}
/***********************************************************************************************************************
* Function Name: R_MTU3_C2_Start
* Description  : This function starts MTU3 channel 2 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MTU3_C2_Start(void)
{
    /* Enable TGIA2 interrupt in ICU */
    IEN(MTU2,TGIA2) = 1U;

    /* Enable TGIB2 interrupt in ICU */
    IEN(MTU2,TGIB2) = 1U;

    MTU.TSTRA.BYTE |= _04_MTU_CST2_ON;
}
/***********************************************************************************************************************
* Function Name: R_MTU3_C2_Stop
* Description  : This function stops MTU3 channel 2 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MTU3_C2_Stop(void)
{
    /* Disable TGIA2 interrupt in ICU */
    IEN(MTU2,TGIA2) = 0U;

    /* Disable TGIB2 interrupt in ICU */
    IEN(MTU2,TGIB2) = 0U;

    MTU.TSTRA.BIT.CST2 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/***********************************************************************************************************************
* Function Name: R_MTU3_C2_Stop
* Description  : This function stops MTU3 channel 2 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void CUMT_MTU3_Init(void)
{
    /* Enable read/write to MTU registers */
    MTU.TRWERA.BYTE = _01_MTU_RWE_ENABLE;

    R_MTU3_C0_Start();
	R_MTU3_C1_Start();
	R_MTU3_C2_Start();
}
/* End user code. Do not edit comment generated here */
