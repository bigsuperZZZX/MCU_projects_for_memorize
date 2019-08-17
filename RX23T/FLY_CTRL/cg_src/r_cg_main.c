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
* File Name    : r_cg_main.c
* Version      : Code Generator for RX23T V1.00.04.02 [29 Nov 2016]
* Device(s)    : R5F523T5AxFM
* Tool-Chain   : CCRX
* Description  : This file implements main function.
* Creation Date: 2017/8/23
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */

/*  This programme is created by CUMT team to support the National Undergraduate Electronic Design Contest.  */
/*  Please do NOT share this programme onto the Internet. CUMT team will take legal actions against violators. */
/*  ********************************** COPYRIGHT ALL RIGHTS RESERVED! **************************************** */

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_icu.h"
#include "r_cg_port.h"
#include "r_cg_mtu3.h"
#include "r_cg_cmt.h"
#include "r_cg_sci.h"
/* Start user code for include. Do not edit comment generated here */
#include "includes.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
volatile uint32_t Systime_cnt_5ms = 0;   // time counter
volatile uint8_t Systime_cnt_flag = 0x00;   //system time interrupt indicator
uint8_t MPU6050_ID = 0x00;
uint8_t HMC5883_Status = 0x00;
extern uint8_t UART5_rx_buffer;
extern uint8_t mode_select;
extern uint8_t action_cmd;
/* End user code. Do not edit comment generated here */


static void R_MAIN_UserInit(void);
/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
    R_MAIN_UserInit();
    /* Start user code. Do not edit comment generated here */
	
	CUMT_PID_Set_Pitch(2.80, 1.1, 0.5, 0.00); //(float kp_pit, float kp_q, float ki_q, float kd_q);
	CUMT_PID_Set_Roll(2.80, 1.1, 0.5, 0.00); //(float kp_rol, float kp_p, float ki_p, float kd_p);
	CUMT_PID_Set_Yaw(3.00, 3.0, 0.00, 0.00); //(float kp_yaw, float kp_r, float ki_r, float kd_r);
    	//CUMT_PID_Set_Altitude(0.0, 100.0, 0.0, 0.0);
	kP_alt = 0.3;
	kI_alt = 0.05;
	kD_alt = 8;
	
	kP_rol = 1.9;
	kI_rol = 0.0;
	kD_rol = 25;
	
	kP_pit = 1.9;
	kI_pit = 0.0;
	kD_pit = 25;
	
	kP_yaw = 9.5;
	kI_yaw = 0.1;
	kD_yaw = 0;
	
	
	while (1U)
    {
		if(Systime_cnt_flag == 0x01)
		{
			Systime_cnt_flag = 0x00;
			// Use prime numbers to take full advantage of the free time of CPU!!!
			if(Systime_cnt_5ms % 1 == 0x00) //200Hz
			{
				CUMT_Loop_200Hz();
			}
			if(Systime_cnt_5ms % 2 == 0x00) //100Hz
			{
				CUMT_Loop_100Hz();
			}
			if(Systime_cnt_5ms % 3 == 0x00) //67Hz
			{
				CUMT_Loop_67Hz();
			}
			if(Systime_cnt_5ms % 5 == 0x00) //40Hz
			{
				CUMT_Loop_40Hz();
			}
			if(Systime_cnt_5ms % 7 == 0x00) //29Hz
			{
				CUMT_Loop_29Hz();
			}
			if(Systime_cnt_5ms % 11 == 0x00) //18Hz
			{
				CUMT_Loop_18Hz();
			}
			if(Systime_cnt_5ms % 13 == 0x00) //15Hz
			{
				CUMT_Loop_15Hz();
			}
			if(Systime_cnt_5ms % 17 == 0x00) //12Hz
			{
				CUMT_Loop_12Hz();
			}
			if(Systime_cnt_5ms % 19 == 0x00) //11Hz
			{
				CUMT_Loop_11Hz();
			}
			if(Systime_cnt_5ms % 21 == 0x00) //10Hz
			{
				CUMT_Loop_10Hz();
			}
			if(Systime_cnt_5ms % 101 == 0x00) //2Hz
			{
				CUMT_Loop_2Hz();
			}
		}
		else { }
    }
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */
	CUMT_MTU3_Init();
	CUMT_LED_Init();
	CUMT_Delay_ms(1000);
	CUMT_IMU_Init();
	R_SCI1_Start();
	R_SCI1_Serial_Receive(&UART1_rx_buffer, 1);
	R_SCI5_Start();
	R_SCI5_Serial_Receive(&UART5_rx_buffer, 1); //启动必须要加
	R_CMT0_Start();	// system time counter
	R_ICU_IRQ5_Start();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
