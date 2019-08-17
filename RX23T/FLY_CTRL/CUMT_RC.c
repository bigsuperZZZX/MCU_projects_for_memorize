
#include "r_cg_macrodriver.h"
#include "CUMT_RC.h"

// There are only 8 channels in ppm, but there are 10 channels on station.
static uint8_t rc_ch_cnt = 0; // remote channel counter
volatile uint16_t RC_rol = 0, RC_pit = 0, RC_thr = 0, RC_yaw = 0; // roll, pitch, throttle, yaw
volatile uint16_t RC_aux1 = 0, RC_aux2 = 0, RC_aux3 = 0, RC_aux4 = 0;  //auxiliary
volatile uint16_t RC_aux5 = 1500, RC_aux6 = 2000; // just used for making station display stable
static int16_t rc_rol_offset = 380; // make the middle value equal to 1500
static int16_t rc_pit_offset = 380; // make the middle value equal to 1500
static int16_t rc_thr_offset = 379; // make the lowest value equal to 1100 which is the real value of remote
static int16_t rc_yaw_offset = 380; // make the middle value equal to 1500
static int16_t rc_aux_offset = 450; // make the middle value equal to 1500

void CUMT_RC_Init(void)
{
	
}

void CUMT_RC_Get_Channel(uint32_t width)
{
	static uint16_t dt_us;
	//uint8_t send_buf[]={0,0,0xff,0,0,0xff,0,0,0xff,0,0,0xff,0x0d,0x0a};
	dt_us = width; /// need changing???
	
	if(dt_us>2000)
	{
		rc_ch_cnt = 0;
	}
	
	else
	{
		rc_ch_cnt++;
	}
	
	switch(rc_ch_cnt)
	{
		case 1: 
			RC_rol = dt_us + rc_rol_offset; break;
		case 2: 
			RC_pit = dt_us + rc_pit_offset; break;
		case 3: 
			RC_thr = dt_us + rc_thr_offset; break;
		case 4: 
			RC_yaw = dt_us + rc_yaw_offset; break;
		case 5: 
			RC_aux1 = dt_us + rc_aux_offset; break;
		case 6: 
			RC_aux2 = dt_us + rc_aux_offset; break;
		case 7: 
			RC_aux3 = dt_us + rc_aux_offset; break;
		case 8: 
			RC_aux4 = dt_us + rc_aux_offset; break;
		case 9: 
			RC_aux5 = dt_us + rc_aux_offset; break;
		case 10: 
			RC_aux6 = dt_us + rc_aux_offset; break;
		default:
			break;
	}
	/*send_buf[0] = RC_rol>>8;
	send_buf[1] = RC_rol&0x00ff;
	send_buf[3] = RC_pit>>8;
	send_buf[4] = RC_pit&0x00ff;
	send_buf[6] = RC_thr>>8;
	send_buf[7] = RC_thr&0x00ff;
	send_buf[9] = RC_yaw>>8;
	send_buf[10] = RC_yaw&0x00ff;
	R_SCI1_Serial_Send(send_buf,14);*/
}

void CUMT_RC_Stop(void)
{

}