
#include "r_cg_macrodriver.h"
#include "CUMT_HMC5883.h"
#include "CUMT_IIC.h"
#include "CUMT_delay.h"
#include "CUMT_math.h"
#include "CUMT_filter.h"

volatile int16_t Mag[3];
static uint8_t HMC5883_buffer[6];
static int16_t HMC5883_FIFO[3][11]; //
static int16_t mx_new, my_new, mz_new;
static int16_t mx_last, my_last, mz_last;

//static int16_t mx_offset = 0, my_offset = 0, mz_offset = 0; // important!!! offset = (max + min)/2
static int16_t mx_offset = 121, my_offset = -226, mz_offset = -15; //GY87-2 // important!!! offset = (max + min)/2
static int16_t mx_max = 0, my_max = 0, mz_max = 0;
static int16_t mx_min = 0, my_min = 0, mz_min = 0;


void CUMT_HMC5883_Init(void)
{
	unsigned char i;	
	int16_t temp[3];
	CUMT_Delay_ms(10);
	CUMT_IIC_Write_Byte(HMC5883_ADDR, HMC5883_R_CONFA, 0x18);
	CUMT_Delay_ms(10);
	CUMT_IIC_Write_Byte(HMC5883_ADDR, HMC5883_R_CONFB, 0x20); //0x00?
	CUMT_Delay_ms(10);
	CUMT_IIC_Write_Byte(HMC5883_ADDR, HMC5883_R_MODE,  0x00);
	CUMT_Delay_ms(200); // waite for device IIC start -- 200 us
	
	// Initialize FIFO
	for(i=0;i<50;i++)
	{
		CUMT_HMC5883_Get_Data3(&temp[0], &temp[1], &temp[2]);
		CUMT_Delay_ms(20); //the highest update frequency of HMC5883 is 50Hz
	}
}

void CUMT_HMC5883_Get_Mag(int16_t *mx, int16_t *my, int16_t *mz) // raw data used for getting offset
{
	CUMT_IIC_Read_Bytes(HMC5883_ADDR, HMC5883_R_XM, 6, HMC5883_buffer);

	*mx = ((int16_t)HMC5883_buffer[0] << 8) | HMC5883_buffer[1];
	*my = ((int16_t)HMC5883_buffer[4] << 8) | HMC5883_buffer[5];
	*mz = ((int16_t)HMC5883_buffer[2] << 8) | HMC5883_buffer[3];
}

void CUMT_HMC5883_Get_MaxMin(void)
{
	int16_t temp[3];
	CUMT_HMC5883_Get_Mag(&temp[0], &temp[1], &temp[2]);
	mx_max = CUMT_MAX(mx_max, temp[0]);
	mx_min = CUMT_MIN(mx_min, temp[0]);
	my_max = CUMT_MAX(my_max, temp[1]);
	my_min = CUMT_MIN(my_min, temp[1]);
	mz_max = CUMT_MAX(mz_max, temp[2]);
	mz_min = CUMT_MIN(mz_min, temp[2]);
}

void CUMT_HMC5883_Update_FIFO(int16_t x, int16_t y, int16_t z)
{
	unsigned char i, j;
	int32_t sum=0;

	for(j=0;j<3;j++)
	{
		for(i=1;i<10;i++)
		{
			HMC5883_FIFO[j][i-1] = HMC5883_FIFO[j][i];
		}
	}

	HMC5883_FIFO[0][9] = x;
	HMC5883_FIFO[1][9] = y;
	HMC5883_FIFO[2][9] = z;

	for(j=0;j<3;j++)
	{
		sum=0;
		for(i=0;i<10;i++)
		{	
   		sum += HMC5883_FIFO[j][i];
		}
		HMC5883_FIFO[j][10] = sum/10; // the 11th data is the average of 10 data
	}
} 


void CUMT_HMC5883_Get_Data3(int16_t *mx, int16_t *my, int16_t *mz) 
{
	uint8_t i;
	
	for(i=0;i<6;i++)
	{
		CUMT_IIC_Read_Bytes(HMC5883_ADDR, HMC5883_R_XM+i, 1, &HMC5883_buffer[i]);
	}

	 mx_new = ((int16_t)HMC5883_buffer[0] << 8) | HMC5883_buffer[1];
	 my_new = ((int16_t)HMC5883_buffer[4] << 8) | HMC5883_buffer[5];
	 mz_new = ((int16_t)HMC5883_buffer[2] << 8) | HMC5883_buffer[3];
	 
	 mx_new -= mx_offset;
	 my_new -= my_offset;
	 mz_new -= mz_offset;

	 *mx = (mx_new + mx_last)/2;
	 *my = (my_new + my_last)/2;
	 *mz = (mz_new + mz_last)/2;
	 
	 mx_last = *mx;
	 my_last = *my;
	 mz_last = *mz;
}