
#include "r_cg_macrodriver.h"
#include "CUMT_schedule.h"
#include "CUMT_MPU6050.h"
#include "CUMT_HMC5883.h"
#include "CUMT_BMP180.h"
#include "CUMT_IMU.h"
#include "CUMT_control.h"
#include "CUMT_RC.h"
#include "CUMT_sonar.h"
#include "CUMT_led.h"
#include "CUMT_UART.h"
#include "Key.h"

uint8_t tx[20] = {0x55,0x56,0x55,0x56,0x55,0x56,0x55,0x56,0x55,0x56,0x55,0x56,0x55,0x56,0x55,0x56,0x55,0x56,0x55,0x56};
uint8_t rx[20] ;
uint8_t tx2 = 0x56;
uint8_t rx2;
extern volatile uint8_t uart5_rx_cplt; 
extern volatile uint8_t uart5_rx_data[20];
extern uint8_t action_cmd;
extern uint8_t action_state;
extern uint8_t mode_select;
extern volatile int16_t Ctrl_drv_rol, Ctrl_drv_pit, Ctrl_drv_yaw;
extern volatile int16_t Ctrl_drv_alt; 
extern uint8_t auto_rol_pit_switch;
extern uint8_t roll_mid, pitch_mid;

extern uint8_t key_pressed;

void CUMT_Loop_200Hz(void)	//5ms
{
	CUMT_MPU6050_Get_Data6(&Acc_Gyro[0],&Acc_Gyro[1],&Acc_Gyro[2],&Acc_Gyro[3],&Acc_Gyro[4],&Acc_Gyro[5]);
}

void CUMT_Loop_100Hz(void)	//10ms
{
	
	CUMT_IMU_Get_Attitude(Acc_Gyro[0],Acc_Gyro[1],Acc_Gyro[2],Acc_Gyro[3],Acc_Gyro[4],Acc_Gyro[5],Mag[0],Mag[1],Mag[2]);
//	CUMT_Ctrl_Attitude_Outer_Loop();
//	CUMT_Ctrl_Attitude_Inner_Loop();
	CUMT_PWM_Output_Direct();
	 
	circle_delay(0);    //参数0表示执行定时
	
	if(uart5_rx_cplt == 1) uart5_rx_cplt = 0;  //实际运行频率20HZ
	else return;
//	if(action_state != LOCK_CPLT)
	{
		if(uart5_rx_data[0] == 0xa0 || uart5_rx_data[0] == 0xa1)
		{
//			Alt_tgt = 1200;
			if(auto_rol_pit_switch == 1)
			{
				ROLL_Control(roll_mid);
				PITCH_Control(pitch_mid);
			}
			else
			{
				Ctrl_drv_rol = ROLL_BASE_VALUE;
				Ctrl_drv_pit = PITCH_BASE_VALUE;
			}
			if(action_cmd != UNLOCK && action_cmd != LOCK)
			{
				YAW_Control(0.0);
			}
			
		}
	}
}

void CUMT_Loop_67Hz(void)	//15ms
{
	//the max output frequency of HMC5883 is 75Hz
	//CUMT_HMC5883_Get_Data3(&Mag[0], &Mag[1], &Mag[2]);	
}

void CUMT_Loop_40Hz(void)	//25ms
{
	mission_action(mode_select);
}

void CUMT_Loop_29Hz(void)	//35ms
{
	HIGH_Control();
	CUMT_Sonar_Trig();
	dot_track(0,1);
}

void CUMT_Loop_18Hz(void)	//55ms
{
	//CUMT_Ctrl_Check_Arm();
	//CUMT_Ctrl_Check_Mode();
	CUMT_UART1_Report_States();
}

void CUMT_Loop_15Hz(void)	//65ms
{
//	CUMT_Ctrl_Check_Arm();
//	CUMT_Ctrl_Check_Mode();
	key_pressed = key_scan();
	decode_key(key_pressed);
}

void CUMT_Loop_12Hz(void)	//85ms
{
	//CUMT_HMC5883_Get_MaxMin();
	//CUMT_UART1_Report_Motor();
}

void CUMT_Loop_11Hz(void)	//95ms
{
	CUMT_UART1_Report_Motor();
}

void CUMT_Loop_10Hz(void)	//105ms
{
	//CUMT_UART1_Report_RC();
}

void CUMT_Loop_2Hz(void)
{
	CUMT_LED_Change();
	
//	CUMT_UART1_Report_PID();
}
