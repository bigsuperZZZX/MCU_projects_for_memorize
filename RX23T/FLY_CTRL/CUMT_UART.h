#ifndef _CUMT_UART_H
#define _CUMT_UART_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for function. Do not edit comment generated here */
extern volatile uint8_t UART1_rx_buffer;
extern volatile uint8_t UART5_rx_buffer;
//extern volatile uint8_t UART1_is_busy;

extern volatile uint16_t kP_kI_kD[18][3];


void CUMT_UART1_Report_States(void);
void CUMT_UART1_Prepare_AGM(void); //acc+gyro+Mag
void CUMT_UART1_Prepare_Attitude(void); //Roll+Ptich+Yaw+Altitude+Fly Mode+is_Armed
void CUMT_UART1_Report_RC(void);
void CUMT_UART1_Report_Motor(void);
void CUMT_UART1_Report_Thr(void);
void CUMT_UART1_Decode_Frame(unsigned char com_data);
void CUMT_UART5_Decode_Frame(void);

/* End user code. Do not edit comment generated here */
#endif