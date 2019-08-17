#ifndef _CUMT_IIC_H
#define _CUMT_IIC_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"

/* Start user code for function. Do not edit comment generated here */


#define IIC_SCL     	    PORT4.PODR.BIT.B0
#define IIC_SDA_O         	PORT4.PODR.BIT.B1
#define IIC_SDA_I         	PORT4.PIDR.BIT.B1

#define SET_IIC_SDA_OUTPUT	PORT4.PDR.BIT.B1 = 1
#define SET_IIC_SDA_INPUT	PORT4.PDR.BIT.B1 = 0
#define SET_IIC_SDA_PULLUP	PORT4.PCR.BYTE = 0x02

void CUMT_IIC_Init(void);
void CUMT_IIC_Start(void);
void CUMT_IIC_Stop(void);
void CUMT_IIC_Ack(void);
void CUMT_IIC_NAck(void);
void CUMT_IIC_Send_Byte(uint8_t txd);

uint8_t CUMT_IIC_WaitAck(void);
uint8_t CUMT_IIC_Read_Byte(unsigned char ack);
uint8_t CUMT_IIC_Read_Bytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data);
uint8_t CUMT_IIC_Write_Bytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data);
uint8_t CUMT_IIC_Write_Byte(uint8_t dev, uint8_t reg, uint8_t data);
uint8_t CUMT_IIC_Write_Bits(uint8_t dev, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t data);
uint8_t CUMT_IIC_Write_Bit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data);

/* End user code. Do not edit comment generated here */
#endif