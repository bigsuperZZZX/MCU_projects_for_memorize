
#include "r_cg_macrodriver.h"
#include "CUMT_MPU6050.h"
#include "CUMT_IIC.h"
#include "CUMT_delay.h"
#include "CUMT_filter.h"

volatile int16_t Acc_Gyro[6];
static uint8_t MPU6050_buffer[14];
static int16_t MPU6050_FIFO[6][11];  // change to 9 in total, so that the average can be calculated by >>3
static int16_t gx_offset = 0, gy_offset = 0, gz_offset = 0;
static int16_t ax_new, ay_new, az_new, gx_new, gy_new, gz_new;

// IIR Butterworth Filtering
const uint8_t filter_order = 4;
const float a_10Hz[5] = {1.0, -3.1806, 3.8612, -2.1122, 0.4383};	// Fs = 200Hz, Fcut = 10Hz
const float b_10Hz[5] = {0.0004, 0.0017, 0.0025, 0.0017, 0.0004};	// Fs = 200Hz, Fcut = 10Hz
const float a_20Hz[5] = {1.0, -2.3695, 2.3140, -1.0547, 0.1874};	// Fs = 200Hz, Fcut = 20Hz
const float b_20Hz[5] = {0.0048, 0.0193, 0.0289, 0.0193, 0.0048};	// Fs = 200Hz, Fcut = 20Hz
const float a_30Hz[5] = {1.0, -1.5704, 1.2756, -0.4844, 0.0762};	// Fs = 200Hz, Fcut = 30Hz
const float b_30Hz[5] = {0.0186, 0.0743, 0.1114, 0.0743, 0.0186};	// Fs = 200Hz, Fcut = 30Hz
static float ax_x[5], ax_y[5];
static float ay_x[5], ay_y[5];
static float az_x[5], az_y[5];
static float gx_x[5], gx_y[5];
static float gy_x[5], gy_y[5];
static float gz_x[5], gz_y[5];

uint8_t count;


void CUMT_MPU6050_Update_FIFO(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz)
{
	unsigned char i,j;
	int32_t sum = 0;
	
	for(j=0;j<6;j++)
	{
		for(i=1;i<10;i++)
		{
			MPU6050_FIFO[j][i-1]=MPU6050_FIFO[j][i];
		}
	}
	
	MPU6050_FIFO[0][9] = ax;
	MPU6050_FIFO[1][9] = ay;
	MPU6050_FIFO[2][9] = az;
	MPU6050_FIFO[3][9] = gx;
	MPU6050_FIFO[4][9] = gy;
	MPU6050_FIFO[5][9] = gz;

	for(j=0;j<6;j++)
	{
		sum = 0;
		for(i=0;i<10;i++)
		{	
			 sum += MPU6050_FIFO[j][i];
		}
		MPU6050_FIFO[j][10] = sum/10;
	}
}

/*******************************************************************************
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Stops the clock and keeps the timing generator in reset
*******************************************************************************/
void CUMT_MPU6050_Set_ClockSource(uint8_t source)
{
   CUMT_IIC_Write_Bits(MPU6050_ADDR, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

void CUMT_MPU6050_Set_FullScaleGyroRange(uint8_t range) 
{
  CUMT_IIC_Write_Bits(MPU6050_ADDR, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

void CUMT_MPU6050_Set_FullScaleAccelRange(uint8_t range) 
{
	CUMT_IIC_Write_Bits(MPU6050_ADDR, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

void CUMT_MPU6050_Set_SleepEnabled(uint8_t enabled) 
{
  CUMT_IIC_Write_Bit(MPU6050_ADDR, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}

void CUMT_MPU6050_Set_IICMasterModeEnabled(uint8_t enabled) 
{
  CUMT_IIC_Write_Bit(MPU6050_ADDR, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_IIC_MST_EN_BIT, enabled);
}

void CUMT_MPU6050_Set_IICBypassEnabled(uint8_t enabled) 
{
  CUMT_IIC_Write_Bit(MPU6050_ADDR, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_IIC_BYPASS_EN_BIT, enabled);
}
/*******************************************************************************/


uint8_t CUMT_MPU6050_Get_DeviceID(void) 
{
  CUMT_IIC_Read_Bytes(MPU6050_ADDR, MPU6050_RA_WHO_AM_I, 1, MPU6050_buffer);
  return MPU6050_buffer[0];
}

uint8_t CUMT_MPU6050_TestConnection(void) 
{
  if(CUMT_MPU6050_Get_DeviceID() == 0x68) 
	  return 1;
  else return 0;
}


void CUMT_MPU6050_Init(void) 
{
	int16_t temp[6];
	unsigned char i;
	// set MPU6050 regesters
  	CUMT_MPU6050_Set_ClockSource(MPU6050_CLOCK_PLL_XGYRO); 
	CUMT_Delay_ms(10);
  	CUMT_MPU6050_Set_FullScaleGyroRange(MPU6050_GYRO_FS_2000);
	CUMT_Delay_ms(10);
  	CUMT_MPU6050_Set_FullScaleAccelRange(MPU6050_ACCEL_FS_2);	
	CUMT_Delay_ms(10);
  	CUMT_MPU6050_Set_SleepEnabled(0); 
	CUMT_Delay_ms(10);
	CUMT_MPU6050_Set_IICMasterModeEnabled(0);	
	CUMT_Delay_ms(10);
	CUMT_MPU6050_Set_IICBypassEnabled(1);	 //1
	CUMT_Delay_ms(10);

	CUMT_IIC_Write_Bit(MPU6050_ADDR, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_LEVEL_BIT, 0);
	CUMT_Delay_ms(10);
	CUMT_IIC_Write_Bit(MPU6050_ADDR, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_OPEN_BIT, 0);
	CUMT_Delay_ms(10);
	CUMT_IIC_Write_Bit(MPU6050_ADDR, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_LATCH_INT_EN_BIT, 1);
	CUMT_Delay_ms(10);
	CUMT_IIC_Write_Bit(MPU6050_ADDR, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_RD_CLEAR_BIT, 1);
	CUMT_Delay_ms(10);

  	CUMT_IIC_Write_Bit(MPU6050_ADDR, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_DATA_RDY_BIT, 1);
	CUMT_Delay_ms(10);
	CUMT_IIC_Write_Byte(MPU6050_ADDR, MPU6050_RA_CONFIG, MPU6050_DLPF_BW_42);
	CUMT_Delay_ms(10);
	
	// Initialize FIFO
  	for(i=0;i<100;i++)	
	{
		CUMT_Delay_us(50);
		CUMT_MPU6050_Get_Data6(&temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5]);
	}
	// Get initial gyro offset																		 
	CUMT_MPU6050_Init_GyroOffset();
}


void CUMT_MPU6050_Get_Data6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz)
{ 	
	uint8_t i;
	for(i=0;i<14;i++)
	{
		CUMT_IIC_Read_Bytes(MPU6050_ADDR, MPU6050_RA_ACCEL_XOUT_H+i, 1, &MPU6050_buffer[i]);
	}
	
	ax_new = (((int16_t)MPU6050_buffer[0]) << 8) | MPU6050_buffer[1];
  	ay_new = (((int16_t)MPU6050_buffer[2]) << 8) | MPU6050_buffer[3];
  	az_new = (((int16_t)MPU6050_buffer[4]) << 8) | MPU6050_buffer[5];
	// Skip temperature
  	gx_new = (((int16_t)MPU6050_buffer[8]) << 8) | MPU6050_buffer[9];
  	gy_new = (((int16_t)MPU6050_buffer[10]) << 8) | MPU6050_buffer[11];
  	gz_new = (((int16_t)MPU6050_buffer[12]) << 8) | MPU6050_buffer[13];
	
	gx_new -= gx_offset;
	gy_new -= gy_offset;
	gz_new -= gz_offset;
	
	*ax = CUMT_IIR_Butterworth_LPF(ax_new, ax_x, ax_y, a_20Hz, b_20Hz, filter_order);
	*ay = CUMT_IIR_Butterworth_LPF(ay_new, ay_x, ay_y, a_20Hz, b_20Hz, filter_order);
	*az = CUMT_IIR_Butterworth_LPF(az_new, az_x, az_y, a_20Hz, b_20Hz, filter_order);
	
	*gx = CUMT_IIR_Butterworth_LPF(gx_new, gx_x, gx_y, a_30Hz, b_30Hz, filter_order);
	*gy = CUMT_IIR_Butterworth_LPF(gy_new, gy_x, gy_y, a_30Hz, b_30Hz, filter_order);
	*gz = CUMT_IIR_Butterworth_LPF(gz_new, gz_x, gz_y, a_30Hz, b_30Hz, filter_order);
/*	
	*ax = ax_new;
	*ay = ay_new;
	*az = az_new;
	*gx = gx_new;
	*gy = gy_new;
	*gz = gz_new;
*/
}

void CUMT_MPU6050_Get_Gyro(int16_t* gx, int16_t* gy, int16_t* gz) // raw data used for getting offset
{
	CUMT_IIC_Read_Bytes(MPU6050_ADDR, MPU6050_RA_ACCEL_XOUT_H, 14, MPU6050_buffer); //MPU6050_RA_ACCEL_XOUT_H is the start address
	*gx = (((int16_t)MPU6050_buffer[8]) << 8) | MPU6050_buffer[9];
	*gy = (((int16_t)MPU6050_buffer[10]) << 8) | MPU6050_buffer[11];
 	*gz = (((int16_t)MPU6050_buffer[12]) << 8) | MPU6050_buffer[13];
}

void CUMT_MPU6050_Init_GyroOffset(void)
{
	unsigned char i;
	int16_t temp[3];
	int32_t	tempgx=0,tempgy=0,tempgz=0;
	gx_offset = 0;
	gy_offset = 0;
	gz_offset = 0;

 	for(i=0;i<100;i++)
	{
		CUMT_Delay_us(200);//!!!
		CUMT_MPU6050_Get_Gyro(&temp[0], &temp[1], &temp[2]);
		tempgx += temp[0];
		tempgy += temp[1];
		tempgz += temp[2];
	}

	gx_offset = tempgx/100; // !!!DO NOT TRANSFER TO int16_t
	gy_offset = tempgy/100;
	gz_offset = tempgz/100;
}