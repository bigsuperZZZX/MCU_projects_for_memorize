#ifndef _CUMT_HMC5883_H
#define _CUMT_HMC5883_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for function. Do not edit comment generated here */

#define HMC5883_ADDR 0x3C // 7 bit address of the HMC5883 used with the Wire library
#define HMC_POS_BIAS 1
#define HMC_NEG_BIAS 2
// HMC5883 register map. For details look over HMC5883 datasheet
#define HMC5883_R_CONFA 0
#define HMC5883_R_CONFB 1
#define HMC5883_R_MODE 2

#define HMC5883_R_XM 3		// Register address for XM.
#define HMC5883_R_XL 4		// Register address for XL.
#define HMC5883_R_YM (7)  // Register address for YM.
#define HMC5883_R_YL (8)  // Register address for YL.
#define HMC5883_R_ZM (5)  // Register address for ZM.
#define HMC5883_R_ZL (6)  // Register address for ZL.

#define HMC5883_R_STATUS 9
#define HMC5883_R_IDA 10
#define HMC5883_R_IDB 11
#define HMC5883_R_IDC 12

extern volatile int16_t Mag[3];

void CUMT_HMC5883_Init(void);
// void CUMT_HMC5883_Update_FIFO(int16_t x, int16_t y, int16_t z);
void CUMT_HMC5883_Get_Data3(int16_t *mx, int16_t *my, int16_t *mz); 
void CUMT_HMC5883_Get_MaxMin(void);

/* End user code. Do not edit comment generated here */
#endif