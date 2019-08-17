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
* File Name    : r_cg_dtc.c
* Version      : Code Generator for RX23T V1.00.04.02 [29 Nov 2016]
* Device(s)    : R5F523T5AxFM
* Tool-Chain   : CCRX
* Description  : This file implements device driver for DTC module.
* Creation Date: 2017/8/2
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
#include "r_cg_dtc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
#pragma address dtc_vector223=0x00004B7CU
uint32_t dtc_vector223;
st_dtc_data dtc_transferdata0;
/* Start user code for global. Do not edit comment generated here */
extern uint8_t uart5_rx_data[20];
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_DTC_Create
* Description  : This function initializes the DTC module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DTC_Create(void)
{
    /* Cancel DTC module stop state */
    MSTP(DTC) = 0U;

    /* Disable transfer data read skip to clear the flag */
    DTC.DTCCR.BYTE = _08_DTC_TRANSFER_READSKIP_DISABLE;

    /* Set DTC0 transfer data */
    dtc_transferdata0.mra_sar = ((uint32_t)(_00_DTC_SRC_ADDRESS_FIXED | _00_DTC_TRANSFER_SIZE_8BIT | 
                                _00_DTC_TRANSFER_MODE_NORMAL)<<24U) | (_00000666_DTC0_SRC_ADDRESS & 0x00FFFFFF);
    dtc_transferdata0.mrb_dar = ((uint32_t)(_08_DTC_DST_ADDRESS_INCREMENTED | _00_DTC_INTERRUPT_COMPLETED)<<24U) | (
                                _00000555_DTC0_DST_ADDRESS & 0x00FFFFFF);
    dtc_transferdata0.cra_crb = (uint32_t)(_0006_DTC0_TRANSFER_COUNT_CRA) << 16U;

    /* Set transfer data start address in DTC vector table */
    dtc_vector223 = (uint32_t) &dtc_transferdata0;

    /* Set address mode */
    DTC.DTCADMOD.BYTE = _01_DTC_ADDRESS_MODE_SHORT;

    /* Set base address */
    DTC.DTCVBR = (void *)0x00004800U;

    /* Enable DTC module start */
    DTC.DTCST.BYTE = _01_DTC_MODULE_START;
}
/***********************************************************************************************************************
* Function Name: R_DTC0_Start
* Description  : This function enables operation of transfer data DTC0.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DTC0_Start(void)
{
    /* Set enable bit for start source interrupt */
    ICU.DTCER[223].BIT.DTCE = 1U;
}
/***********************************************************************************************************************
* Function Name: R_DTC0_Stop
* Description  : This function disables operation of transfer data DTC0.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DTC0_Stop(void)
{
    /* Clear enable bit for start source interrupt */
    ICU.DTCER[223].BIT.DTCE = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
void my_DTC_Create(void)
{
    /* Cancel DTC module stop state */
    MSTP(DTC) = 0U;

    /* Disable transfer data read skip to clear the flag */
    DTC.DTCCR.BYTE = _08_DTC_TRANSFER_READSKIP_DISABLE;

    /* Set DTC0 transfer data */
    dtc_transferdata0.mra_sar = ((uint32_t)(_00_DTC_SRC_ADDRESS_FIXED | _00_DTC_TRANSFER_SIZE_8BIT | 
                                _00_DTC_TRANSFER_MODE_NORMAL)<<24U) | ((&SCI5.RDR) & 0x00FFFFFF);
    dtc_transferdata0.mrb_dar = ((uint32_t)(_08_DTC_DST_ADDRESS_INCREMENTED | _00_DTC_INTERRUPT_COMPLETED)<<24U) | (
                                uart5_rx_data & 0x00FFFFFF);
    dtc_transferdata0.cra_crb = (uint32_t)(_0006_DTC0_TRANSFER_COUNT_CRA) << 16U;

    /* Set transfer data start address in DTC vector table */
    dtc_vector223 = (uint32_t) &dtc_transferdata0;

    /* Set address mode */
    DTC.DTCADMOD.BYTE = _01_DTC_ADDRESS_MODE_SHORT;

    /* Set base address */
    DTC.DTCVBR = (void *)0x00004800U;

    /* Enable DTC module start */
    DTC.DTCST.BYTE = _01_DTC_MODULE_START;
}
/* End user code. Do not edit comment generated here */
