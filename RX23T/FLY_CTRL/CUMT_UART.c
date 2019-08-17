
#include "r_cg_macrodriver.h"
#include "CUMT_UART.h"
#include "r_cg_sci.h"
#include "CUMT_MPU6050.h"
#include "CUMT_HMC5883.h"
#include "CUMT_BMP180.h"
#include "CUMT_IMU.h"
#include "CUMT_sonar.h"
#include "CUMT_RC.h"
#include "CUMT_control.h"
#include "CUMT_motor.h"
#include "CUMT_PID.h"
#include "CUMT_delay.h"

//volatile uint8_t UART1_is_busy = 0;

volatile uint16_t kP_kI_kD[18][3];

volatile uint8_t UART1_rx_buffer;
volatile uint8_t uart1_rx_data[50];
static uint8_t uart1_rx_state = 0;
static uint16_t uart1_rx_len = 0;  // data length without functional bytes
static uint16_t uart1_rx_cnt = 0;  // data number counter

volatile uint8_t UART5_rx_buffer;
volatile uint8_t uart5_rx_data[20];
static uint8_t uart5_rx_state = 0;
static uint16_t uart5_rx_len = 0;  // data length without functional bytes
static uint16_t uart5_rx_cnt = 0;  // data number counter

static unsigned char uart1_tx_data_check[7];
static uint16_t tx_len_check = 7;

static unsigned char uart1_tx_data_agm[23];
static unsigned char uart1_tx_data_attitude[17];
static unsigned char uart1_tx_data_states[40];
static uint16_t tx_len_states = 23 + 17;

static unsigned char uart1_tx_data_rc[25];
static uint16_t tx_len_rc = 25;

static unsigned char uart1_tx_data_motor[13];
static uint16_t tx_len_motor = 13;

static unsigned char uart1_tx_data_pid1[23];
static unsigned char uart1_tx_data_pid2[23];
static unsigned char uart1_tx_data_pid3[23];
static unsigned char uart1_tx_data_pid4[23];
static unsigned char uart1_tx_data_pid[23*4];
static uint16_t tx_len_pid = 23*4;

static unsigned char uart1_tx_data_thr[11];
static uint16_t tx_len_thr = 11;

volatile uint8_t rol_position = 60;  //中点值
volatile uint8_t pit_position = 60;  //中点值
volatile signed int yaw_position = 0;  //中点值


extern volatile int16_t Ctrl_drv_rol, Ctrl_drv_pit, Ctrl_drv_yaw;
extern volatile int16_t Ctrl_drv_alt, Keep_Alt_thr; 

extern uint8_t mode_select;
extern uint8_t action_cmd;

extern float yaw_offset_init_value ;

/////////////////////////////////////////// UART1 Transmit ////////////////////////////////////////////////
 //test RC_Thr
void CUMT_UART1_Report_Thr(void)
{
	unsigned int temp = 0xAA + 0xAA + 0x32 + 6; // for sum check
	char ctemp; // for transmitted data
//	unsigned int i;
	
	/*if(RC_aux2>1400)
	{
		return;
	}*/
	if(PORT0.PIDR.BIT.B0 == 1)
	{
		return;
	}
	
	
	uart1_tx_data_thr[0] = 0xAA;
	uart1_tx_data_thr[1] = 0xAA;
	uart1_tx_data_thr[2] = 0x32;
	uart1_tx_data_thr[3] = 6;
	
	ctemp = RC_thr>>8;
	uart1_tx_data_thr[4] = ctemp;
	temp += ctemp;
	ctemp = RC_thr;
	uart1_tx_data_thr[5] = ctemp;
	temp += ctemp;
	
	//YAW
	ctemp = RC_pit>>8;
	uart1_tx_data_thr[6] = ctemp;
	temp += ctemp;
	ctemp = RC_pit;
	uart1_tx_data_thr[7] = ctemp;
	temp += ctemp;
	
	//ROL
	ctemp = RC_rol>>8;
	uart1_tx_data_thr[8] = ctemp;
	temp += ctemp;
	ctemp = RC_rol;
	uart1_tx_data_thr[9] = ctemp;
	temp += ctemp;
	
	uart1_tx_data_thr[10] = temp%256;
	
	R_SCI1_Serial_Send(uart1_tx_data_thr, tx_len_thr);
}


/**************************************
Report all data up to station
This can be done using DMA
***************************************/

void CUMT_UART1_Report_Check(uint8_t cmd_flag, uint8_t check_sum)
{
	unsigned int temp = 0xAA + 0xAA +0xEF +0x02 + cmd_flag + check_sum;
	uart1_tx_data_check[0] = 0xAA;
	uart1_tx_data_check[1] = 0xAA;
	uart1_tx_data_check[2] = 0xEF;
	uart1_tx_data_check[3] = 0x02;
	uart1_tx_data_check[4] = cmd_flag;
	uart1_tx_data_check[5] = check_sum;
	uart1_tx_data_check[6] = temp%256;
	
	R_SCI1_Serial_Send(uart1_tx_data_check, tx_len_check);
}

/**************************************
Report all data up to station
This can be done using DMA
***************************************/
void CUMT_UART1_Report_States(void)
{
	static unsigned int i, j;
	/*
	if(RC_aux2>1400)
	{
		return;
	}	*/
	
	if(PORT0.PIDR.BIT.B0 == 1)
	{
		return;
	}
	
	//*** prepare for the acc_gyro_gam data ***
	CUMT_UART1_Prepare_AGM();
	for(i=0;i<23;i++)
	{
		uart1_tx_data_states[i] = uart1_tx_data_agm[i];
	}	
	
	//*** prepare for the attitude data ***
	CUMT_UART1_Prepare_Attitude();
	for(j=0;j<17;j++)
	{
		uart1_tx_data_states[j+23] = uart1_tx_data_attitude[j];
	}
	
	//start sending
	R_SCI1_Serial_Send(uart1_tx_data_states, tx_len_states);	
}

/**************************************
acc+gyro+Mag
***************************************/
void CUMT_UART1_Prepare_AGM(void) 
{
	unsigned int temp = 0xAA + 0xAA + 0x02 + 18; // for sum check
	char ctemp; // for transmitted data
	unsigned int i;
	
	uart1_tx_data_agm[0] = 0xAA;
	uart1_tx_data_agm[1] = 0xAA;
	uart1_tx_data_agm[2] = 0x02;
	uart1_tx_data_agm[3] = 18;
	
	//Acc_Gyro
	for(i=0;i<6;i++)
	{
		ctemp = Acc_Gyro[i]>>8;
		uart1_tx_data_agm[2*i+4] = ctemp;
		temp += ctemp;
		ctemp = Acc_Gyro[i];
		uart1_tx_data_agm[2*i+5] = ctemp;
		temp += ctemp;
	}
	
	//Mag
	for(i=0;i<3;i++)
	{
		ctemp = Mag[i]>>8;
		uart1_tx_data_agm[2*i+16] = ctemp;
		temp += ctemp;
		ctemp = Mag[i];
		uart1_tx_data_agm[2*i+17] = ctemp;
		temp += ctemp;
	}
	
	// send sum check byte
	uart1_tx_data_agm[22] = temp%256;
}

/**************************************
Roll+Ptich+Yaw+Sonar_Alt+Fly Mode+is_Armed
Roll+Ptich+Yaw: (int16_t)RPY*100 unit: deg
Sonar_Alt: Sonar_height/10*100	 unit: cm
Fly Mode:	
is_Armed:		0-locked
				1-unlocked
***************************************/
void CUMT_UART1_Prepare_Attitude(void) 
{
	unsigned int temp = 0xAA + 0xAA + 0x01 + 12; //  for sum check
	char ctemp; // for transmitted data
	int16_t atemp; // for data multiplication

	uart1_tx_data_attitude[0] = 0xAA;
	uart1_tx_data_attitude[1] = 0xAA;
	uart1_tx_data_attitude[2] = 0x01;
	uart1_tx_data_attitude[3] = 12;
	
	//Roll
	atemp = (int16_t)(Roll*100.0);
	ctemp = atemp>>8;
	uart1_tx_data_attitude[4] = ctemp;
	temp += ctemp;
	ctemp = atemp;
	uart1_tx_data_attitude[5] = ctemp;
	temp += ctemp;
	
	//Pitch
	atemp = (int16_t)(Pitch*100.0);
	ctemp = atemp>>8;
	uart1_tx_data_attitude[6] = ctemp;
	temp += ctemp;
	ctemp = atemp;
	uart1_tx_data_attitude[7] = ctemp;
	temp += ctemp;
	
	//Yaw
	atemp = (int16_t)(Yaw*100.0);
	ctemp = atemp>>8;
	uart1_tx_data_attitude[8] = ctemp;
	temp += ctemp;
	ctemp = atemp;
	uart1_tx_data_attitude[9] = ctemp;
	temp += ctemp;
	
	// Sonar_height
	atemp = Sonar_height*10;
	ctemp = 0;
//	ctemp = atemp>>24;
	uart1_tx_data_attitude[10] = ctemp;
	temp += ctemp;
	ctemp = 0;
//	ctemp = atemp>>16;
	uart1_tx_data_attitude[11] = ctemp;
	temp += ctemp;
	ctemp = atemp>>8;
	uart1_tx_data_attitude[12] = ctemp;
	temp += ctemp;
	ctemp = atemp;
	uart1_tx_data_attitude[13] = ctemp;
	temp += ctemp;
	

	//Fly_mode
	ctemp = Fly_mode;
	uart1_tx_data_attitude[14] = ctemp;
	temp += ctemp;
	
	// is_Armed
	ctemp = is_Armed;
	uart1_tx_data_attitude[15] = ctemp;
	temp += ctemp;
	
	// send sum check byte
	uart1_tx_data_attitude[16] = temp%256;
}

/**************************************
rc
THR+YAW+ROL+PIT+AUX1+AUX2
***************************************/
void CUMT_UART1_Report_RC(void)
{
	unsigned int temp = 0xAA + 0xAA + 0x03 + 20; //  for sum check
	char ctemp; // for transmitted data
	
	/*if(RC_aux2>1400)
	{
		return;
	}*/
	
	if(PORT0.PIDR.BIT.B0 == 1)
	{
		return;
	}
	
	uart1_tx_data_rc[0] = 0xAA;
	uart1_tx_data_rc[1] = 0xAA;
	uart1_tx_data_rc[2] = 0x03;
	uart1_tx_data_rc[3] = 20;
	
	//THR
	ctemp = RC_thr>>8;
	uart1_tx_data_rc[4] = ctemp;
	temp += ctemp;
	ctemp = RC_thr;
	uart1_tx_data_rc[5] = ctemp;
	temp += ctemp;
	
	//YAW
	ctemp = RC_yaw>>8;
	uart1_tx_data_rc[6] = ctemp;
	temp += ctemp;
	ctemp = RC_yaw;
	uart1_tx_data_rc[7] = ctemp;
	temp += ctemp;
	
	//ROL
	ctemp = RC_rol>>8;
	uart1_tx_data_rc[8] = ctemp;
	temp += ctemp;
	ctemp = RC_rol;
	uart1_tx_data_rc[9] = ctemp;
	temp += ctemp;
	
	//PIT
	ctemp = RC_pit>>8;
	uart1_tx_data_rc[10] = ctemp;
	temp += ctemp;
	ctemp = RC_pit;
	uart1_tx_data_rc[11] = ctemp;
	temp += ctemp;
	
	//AUX1
	ctemp = RC_aux1>>8;
	uart1_tx_data_rc[12] = ctemp;
	temp += ctemp;
	ctemp = RC_aux1;
	uart1_tx_data_rc[13] = ctemp;
	temp += ctemp;
	
	//AUX2
	ctemp = RC_aux2>>8;
	uart1_tx_data_rc[14] = ctemp;
	temp += ctemp;
	ctemp = RC_aux2;
	uart1_tx_data_rc[15] = ctemp;
	temp += ctemp;
	
	//AUX3
	ctemp = RC_aux3>>8;
	uart1_tx_data_rc[16] = ctemp;
	temp += ctemp;
	ctemp = RC_aux3;
	uart1_tx_data_rc[17] = ctemp;
	temp += ctemp;
	
	//AUX4
	ctemp = RC_aux4>>8;
	uart1_tx_data_rc[18] = ctemp;
	temp += ctemp;
	ctemp = RC_aux4;
	uart1_tx_data_rc[19] = ctemp;
	temp += ctemp;
	
	//AUX5
	ctemp = RC_aux5>>8;
	uart1_tx_data_rc[20] = ctemp;
	temp += ctemp;
	ctemp = RC_aux5;
	uart1_tx_data_rc[21] = ctemp;
	temp += ctemp;
	
	//AUX6
	ctemp = RC_aux6>>8;
	uart1_tx_data_rc[22] = ctemp;
	temp += ctemp;
	ctemp = RC_aux6;
	uart1_tx_data_rc[23] = ctemp;
	temp += ctemp;
	
	// send sum check byte
	uart1_tx_data_rc[24] = temp%256;
	
	//start sending
	R_SCI1_Serial_Send(uart1_tx_data_rc, tx_len_rc);
}

void CUMT_UART1_Report_Motor(void)
{
	unsigned int temp = 0xAA + 0xAA + 0x06 + 8; //  for sum check
	char ctemp; // for transmitted data
//	int16_t atemp; // for data multiplication

	/*if(RC_aux2>1400)
	{
		return;
	}*/
	ctemp = PORT0.PIDR.BIT.B0;
	if(PORT0.PIDR.BIT.B0 == 1)
	{
		return;
	}

	uart1_tx_data_motor[0] = 0xAA;
	uart1_tx_data_motor[1] = 0xAA;
	uart1_tx_data_motor[2] = 0x06;
	uart1_tx_data_motor[3] = 8;
	
	//ROL
	ctemp = Ctrl_drv_rol>>8;
	uart1_tx_data_motor[4] = ctemp;
	temp += ctemp;
	ctemp = Ctrl_drv_rol;
	uart1_tx_data_motor[5] = ctemp;
	temp += ctemp;
	
	//PIT
	ctemp = Ctrl_drv_pit>>8;
	uart1_tx_data_motor[6] = ctemp;
	temp += ctemp;
	ctemp = Ctrl_drv_pit;
	uart1_tx_data_motor[7] = ctemp;
	temp += ctemp;
	
	//thr
	ctemp = (Ctrl_drv_alt + Keep_Alt_thr)>>8;
	uart1_tx_data_motor[8] = ctemp;
	temp += ctemp;
	ctemp = (Ctrl_drv_alt + Keep_Alt_thr);
	uart1_tx_data_motor[9] = ctemp;
	temp += ctemp;
	
	//YAW
	ctemp = Ctrl_drv_yaw>>8;
	uart1_tx_data_motor[10] = ctemp;
	temp += ctemp;
	ctemp = Ctrl_drv_yaw;
	uart1_tx_data_motor[11] = ctemp;
	temp += ctemp;
	
	// send sum check byte
	uart1_tx_data_motor[12] = temp%256;
	
	//start sending
	R_SCI1_Serial_Send(uart1_tx_data_motor, tx_len_motor);
}

void CUMT_UART1_Prepare_PID1(void) 
{
	unsigned int temp = 0xAA + 0xAA + 0x10 + 18; // for sum check
	char ctemp; // for transmitted data
	unsigned int i, j;
	
	uart1_tx_data_pid1[0] = 0xAA;
	uart1_tx_data_pid1[1] = 0xAA;
	uart1_tx_data_pid1[2] = 0x10;
	uart1_tx_data_pid1[3] = 18;
	
	kP_kI_kD[0][0] = kP_p*100;
	kP_kI_kD[0][1] = kI_p*100;
	kP_kI_kD[0][2] = kD_p*100;
	
	kP_kI_kD[1][0] = kP_q*100;
	kP_kI_kD[1][1] = kI_q*100;
	kP_kI_kD[1][2] = kD_q*100;
	
	kP_kI_kD[2][0] = kP_r*100;
	kP_kI_kD[2][1] = kI_r*100;
	kP_kI_kD[2][2] = kD_r*100;
	
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			ctemp = kP_kI_kD[i][j]>>8;
			uart1_tx_data_pid1[6*i+2*j+4] = ctemp;
			temp += ctemp;
			ctemp = kP_kI_kD[i][j];
			uart1_tx_data_pid1[6*i+2*j+5] = ctemp;
			temp += ctemp;
		}
	}
	// send sum check byte
	uart1_tx_data_pid1[22] = temp%256;
}

void CUMT_UART1_Prepare_PID2(void) 
{
	unsigned int temp = 0xAA + 0xAA + 0x11 + 18; // for sum check
	char ctemp; // for transmitted data
	unsigned int i, j;
	
	uart1_tx_data_pid2[0] = 0xAA;
	uart1_tx_data_pid2[1] = 0xAA;
	uart1_tx_data_pid2[2] = 0x11;
	uart1_tx_data_pid2[3] = 18;
	
	kP_kI_kD[3][0] = kP_rol*100;
	kP_kI_kD[3][1] = kI_rol*100;
	kP_kI_kD[3][2] = kD_rol*100;
	
	kP_kI_kD[4][0] = kP_pit*100;
	kP_kI_kD[4][1] = kI_pit*100;
	kP_kI_kD[4][2] = kD_pit*100;
	
	kP_kI_kD[5][0] = kP_yaw*100;
	kP_kI_kD[5][1] = kI_yaw*100;
	kP_kI_kD[5][2] = kD_yaw*100;
	
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			ctemp = kP_kI_kD[i+3][j]>>8;
			uart1_tx_data_pid2[6*i+2*j+4] = ctemp;
			temp += ctemp;
			ctemp = kP_kI_kD[i+3][j];
			uart1_tx_data_pid2[6*i+2*j+5] = ctemp;
			temp += ctemp;
		}
	}
	// send sum check byte
	uart1_tx_data_pid2[22] = temp%256;
}

void CUMT_UART1_Prepare_PID3(void) 
{
	unsigned int temp = 0xAA + 0xAA + 0x12 + 18; // for sum check
	char ctemp; // for transmitted data
	unsigned int i, j;
	
	uart1_tx_data_pid3[0] = 0xAA;
	uart1_tx_data_pid3[1] = 0xAA;
	uart1_tx_data_pid3[2] = 0x12;
	uart1_tx_data_pid3[3] = 18;
	
	kP_kI_kD[6][0] = kP_w*100;
	kP_kI_kD[6][1] = kI_w*100;
	kP_kI_kD[6][2] = kD_w*100;
	
	kP_kI_kD[7][0] = kP_alt*100;
	kP_kI_kD[7][1] = kI_alt*100;
	kP_kI_kD[7][2] = kD_alt*100;
	
	kP_kI_kD[8][0] = 0;
	kP_kI_kD[8][1] = 0;
	kP_kI_kD[8][2] = 0;
	
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			ctemp = kP_kI_kD[i+6][j]>>8;
			uart1_tx_data_pid3[6*i+2*j+4] = ctemp;
			temp += ctemp;
			ctemp = kP_kI_kD[i+6][j];
			uart1_tx_data_pid3[6*i+2*j+5] = ctemp;
			temp += ctemp;
		}
	}
	// send sum check byte
	uart1_tx_data_pid3[22] = temp%256;
}

void CUMT_UART1_Prepare_PID4(void) 
{
	unsigned int temp = 0xAA + 0xAA + 0x13 + 18; // for sum check
	char ctemp; // for transmitted data
	unsigned int i, j;
	
	uart1_tx_data_pid4[0] = 0xAA;
	uart1_tx_data_pid4[1] = 0xAA;
	uart1_tx_data_pid4[2] = 0x13;
	uart1_tx_data_pid4[3] = 18;
	
	kP_kI_kD[9][0] = kP_pos*100;
	kP_kI_kD[9][1] = kI_pos*100;
	kP_kI_kD[9][2] = kD_pos*100;
	
	kP_kI_kD[10][0] = kP_IMU*100;
	kP_kI_kD[10][1] = kI_IMU*1000;
	kP_kI_kD[10][2] = 0;
	
	kP_kI_kD[11][0] = 0;
	kP_kI_kD[11][1] = 0;
	kP_kI_kD[11][2] = 0;
	
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			ctemp = kP_kI_kD[i+9][j]>>8;
			uart1_tx_data_pid4[6*i+2*j+4] = ctemp;
			temp += ctemp;
			ctemp = kP_kI_kD[i+9][j];
			uart1_tx_data_pid4[6*i+2*j+5] = ctemp;
			temp += ctemp;
		}
	}
	// send sum check byte
	uart1_tx_data_pid4[22] = temp%256;
}

void CUMT_UART1_Report_PID(void) 
{
	uint8_t i;
	
	CUMT_UART1_Prepare_PID1();
	CUMT_UART1_Prepare_PID2();
	CUMT_UART1_Prepare_PID3();
	CUMT_UART1_Prepare_PID4();
	
	for(i=0;i<23;i++)
	{
		uart1_tx_data_pid[i] = uart1_tx_data_pid1[i];
	}
	for(i=0;i<23;i++)
	{
		uart1_tx_data_pid[i+23] = uart1_tx_data_pid2[i];
	}
	for(i=0;i<23;i++)
	{
		uart1_tx_data_pid[i+46] = uart1_tx_data_pid3[i];
	}
	for(i=0;i<23;i++)
	{
		uart1_tx_data_pid[i+69] = uart1_tx_data_pid4[i];
	}
	
	R_SCI1_Serial_Send(uart1_tx_data_pid, tx_len_pid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////// UART1 Receive ///////////////////////////////////////////

void CUMT_UART1_Get_Command1(void)
{
	static uint8_t cmd1;
	cmd1 = uart1_rx_data[4];
	if(!is_Armed)
	{
		switch(cmd1)
		{
			case 0x01:  // acc calibration
				break;
			case 0x02:  // gyro calibration
				CUMT_MPU6050_Init_GyroOffset(); break;
			case 0x04:	// Mag calibration
				break;
			case 0x05:	// bara calibration
				break;
			default:
				break;
		}
	}
}

void CUMT_UART1_Get_Command2(void)
{
	static uint8_t cmd2;
	cmd2 = uart1_rx_data[4];
	if(!is_Armed)
	{
		switch(cmd2)
		{
			case 0x01: // PID reading
				CUMT_UART1_Report_PID();
				break;
			case 0x02: // Fly mode reading
				break;
			case 0x21: // path point number reading
				break;
			default:
				break;
		}
	}
}

void CUMT_UART1_Get_PID(uint8_t cmd_flag)
{
//	static uint16_t temp;
//	temp = uart1_rx_data[4];
	uint8_t i, j;
	
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			kP_kI_kD[cmd_flag+i][j] = uart1_rx_data[6*i+2*j+4]<<8 | uart1_rx_data[6*i+2*j+5];
		}
	}
	if(!is_Armed)
	{
		if(cmd_flag==15) // PID writing is over
		{
			kP_p = kP_kI_kD[0][0]/100.0;
			kI_p = kP_kI_kD[0][1]/100.0;
			kD_p = kP_kI_kD[0][2]/100.0;
			
			kP_q = kP_kI_kD[1][0]/100.0;
			kI_q = kP_kI_kD[1][1]/100.0;
			kD_q = kP_kI_kD[1][2]/100.0;
			
			kP_r = kP_kI_kD[2][0]/100.0;
			kI_r = kP_kI_kD[2][1]/100.0;
			kD_r = kP_kI_kD[2][2]/100.0;
			
			kP_rol = kP_kI_kD[3][0]/100.0;
			kI_rol = kP_kI_kD[3][1]/100.0;
			kD_rol = kP_kI_kD[3][2]/100.0;
			
			kP_pit = kP_kI_kD[4][0]/100.0;	
			kI_pit = kP_kI_kD[4][1]/100.0;
			kD_pit = kP_kI_kD[4][2]/100.0;
			
			kP_yaw = kP_kI_kD[5][0]/100.0;
			kI_yaw = kP_kI_kD[5][1]/100.0;
			kD_yaw = kP_kI_kD[5][2]/100.0;
				
			kP_w = kP_kI_kD[6][0]/100.0;
			kI_w = kP_kI_kD[6][1]/100.0;
			kD_w = kP_kI_kD[6][2]/100.0;
			
			kP_alt = kP_kI_kD[7][0]/100.0;
			kI_alt = kP_kI_kD[7][1]/100.0;
			kD_alt = kP_kI_kD[7][2]/100.0;
			
			kP_pos = kP_kI_kD[9][0]/100.0;
			kI_pos = kP_kI_kD[9][1]/100.0;
			kD_pos = kP_kI_kD[9][2]/100.0;
			
			kP_IMU = kP_kI_kD[10][0]/100.0;
			kI_IMU = kP_kI_kD[10][1]/1000.0;
		}
	}
}

unsigned char CUMT_UART1_Sumcheck(void)
{
	uint8_t i;
  uint8_t checksum = 0; 
  for(i=0;i<uart1_rx_data[3]+4;i++)
  {
		checksum += uart1_rx_data[i];
	}
  if((checksum%256) == uart1_rx_data[i])
   return(0x01);
  else
   return(0x00);
}

unsigned char CUMT_UART5_Sumcheck(void)
{
	uint8_t i;
	uint16_t checksum = 0; 
	if((uint8_t)((uart5_rx_data[1] + uart5_rx_data[2])&0x00ff) == uart5_rx_data[3])
		return(0x01);
	else
		return(0x00);
}

void CUMT_UART1_Decode_Command(void)
{
	if(CUMT_UART1_Sumcheck())
	{
		static uint8_t cp_cmd;
		cp_cmd = uart1_rx_data[2];
		switch (cp_cmd)
		{
			case 0x01:
				CUMT_UART1_Get_Command1();	break;
			case 0x02:
				CUMT_UART1_Get_Command2();	break;
			case 0x10:
				CUMT_UART1_Get_PID(0);	break;
			case 0x11:
				CUMT_UART1_Get_PID(3);	break;
			case 0x12:
				CUMT_UART1_Get_PID(6);	break;
			case 0x13:
				CUMT_UART1_Get_PID(9);	break;
			case 0x14:
				CUMT_UART1_Get_PID(12);	break;
			case 0x15:
				CUMT_UART1_Get_PID(15);	break;
			default:
				break;
		}
	}
}


void CUMT_UART1_Decode_Frame(unsigned char com_data)
{
	if(uart1_rx_state==0 && com_data==0xAA) // header AA
	{
		uart1_rx_state = 1;
		uart1_rx_data[0] = com_data;
	}
	else if(uart1_rx_state==1 && com_data==0xAF) //header AF
	{
		uart1_rx_state = 2;
		uart1_rx_data[1] = com_data;
	}
	else if(uart1_rx_state==2 && com_data>0 && com_data<0XF1) //command
	{
		uart1_rx_state = 3;
		uart1_rx_data[2] = com_data;
	}
	else if(uart1_rx_state==3 && com_data<50) // data length byte
	{
		uart1_rx_state = 4;
		uart1_rx_data[3] = com_data;
		uart1_rx_len = com_data;
		uart1_rx_cnt = 0;
	}
	else if(uart1_rx_state==4 && uart1_rx_cnt<uart1_rx_len) // data bytes
	{
		uart1_rx_data[uart1_rx_cnt+4] = com_data;
		uart1_rx_cnt++;
		if(uart1_rx_cnt==uart1_rx_len)
			uart1_rx_state = 5;
	}
	else if(uart1_rx_state==5)  // sum check byte
	{
		uart1_rx_state = 0;
		uart1_rx_data[4+uart1_rx_cnt] = com_data;
		CUMT_UART1_Report_Check(uart1_rx_data[2], com_data);
		CUMT_UART1_Decode_Command();
	}
	else
		uart1_rx_state = 0;
}


void CUMT_UART5_Decode_Frame(void)
{
	if(uart5_rx_data[0] == 0xa0)
	{
		rol_position = uart5_rx_data[1];
		pit_position = uart5_rx_data[2];
	}
	if(uart5_rx_data[0] == 0xa1)
	{
		rol_position = uart5_rx_data[1];
		pit_position = uart5_rx_data[2];
	}
	if(uart5_rx_data[0] == 0xa3)
	{
		mode_select = MISSION_1;
		yaw_offset_init_value = Yaw;
	}
	if(uart5_rx_data[0] == 0xa4)
	{
		mode_select = MISSION_2;
		yaw_offset_init_value = Yaw;
	}
	if(uart5_rx_data[0] == 0xa5)
	{
		mode_select = MISSION_3;
		yaw_offset_init_value = Yaw;
	}
	if(uart5_rx_data[0] == 0xa6)
	{
		mode_select = MISSION_4;
		yaw_offset_init_value = Yaw;
	}
	if(uart5_rx_data[0] == 0xa7)
	{
		mode_select = MISSION_5;
		yaw_offset_init_value = Yaw;
	}
	if(uart5_rx_data[0] == 0xa8)
	{
		action_cmd = UNLOCK;
	}
}

