/** @file
 * @brief Implementation of functions for LDC13xx16xx_evm
 */

#include "LDC13xx16xx_cmd.h"
#include "LDC13xx16xx_evm.h"
//#include "host_interface.h"
//#include "spi_1p1.h"
#include "IIC.h"
#include "delay.h"

/** Defaults */
#define INFOD_START (0x1800)

#pragma DATA_SECTION(FlashConfig, ".infoD")
uint16_t  FlashConfig[EVM_DEFAULTS_SIZE*sizeof(uint16_t)];

/** DRDY */
static volatile uint8_t dataReady;
static uint16_t allData[8];

static uint8_t default_addr=0x2b;

/*uint8_t evm_readDefaults(uint8_t offset,uint8_t * buffer,uint8_t size) 
{
	memcpy(&buffer[0],(uint8_t*)&FlashConfig[offset],size*sizeof(uint16_t));
	return size;
}*/

/** Initialization */
void evm_init() 
{
	uint8_t i;
	dataReady = 0;
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_RESET_DEVICE,0x8000);
	delay_ms(10);
	// TODO: put addrs in a const array; iterate over array
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_REF_COUNT_CH0,0xFFFF); // 4 clock periods
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_REF_COUNT_CH1,0xFFFF);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_REF_COUNT_CH2,0xFFFF);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_REF_COUNT_CH3,0xFFFF);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_OFFSET_CH0,0x0000);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_OFFSET_CH1,0x0000);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_OFFSET_CH2,0x0000);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_OFFSET_CH3,0x0000);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_SETTLE_COUNT_CH0,0x0400); // 1 clock period
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_SETTLE_COUNT_CH1,0x0400);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_SETTLE_COUNT_CH2,0x0400);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_SETTLE_COUNT_CH3,0x0400);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_CLOCK_DIVIDERS_CH0,0x0000); // bypass dividers
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_CLOCK_DIVIDERS_CH1,0x0000);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_CLOCK_DIVIDERS_CH2,0x0000);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_CLOCK_DIVIDERS_CH3,0x0000);
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_STATUS,0x0001); // report only DRDYs to INT
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_CONFIG,0x0200); // CLKIN pin
		// IIC_SendData_16(default_addr,LDC13xx16xx_CMD_MUX_CONFIG,0x820F); // ch0, ch1
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_SYSTEM_CLOCK_CONFIG,0x0200); // default, divide by 2
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_DRIVE_CURRENT_CH0,0x0000); //
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_DRIVE_CURRENT_CH1,0x0000); //
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_DRIVE_CURRENT_CH2,0x0000); //
	IIC_SendData_16(default_addr,LDC13xx16xx_CMD_DRIVE_CURRENT_CH3,0x0000); //
}

/*void evm_processDRDY() {    ********************************************** 
	if (dataReady) {
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_MSB_CH0,&allData[0]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_LSB_CH0,&allData[1]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_MSB_CH1,&allData[2]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_LSB_CH1,&allData[3]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_MSB_CH2,&allData[4]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_LSB_CH2,&allData[5]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_MSB_CH3,&allData[6]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_LSB_CH3,&allData[7]);
	  dataReady = 0;
	}
}*/

// TODO: use DMA to transfer data
// ch 0-3
/*uint8_t evm_readFreq(uint8_t ch, uint8_t * buffer) 
{
	buffer[0] = allData[ch*2] >> 8;
	buffer[1] = allData[ch*2] & 0xFF;
	buffer[2] = allData[ch*2+1] >> 8;
	buffer[3] = allData[ch*2+1] & 0xFF;
	return 4;
}*/



/*uint8_t evm_changeAddr(uint8_t addr) {
	if (default_addr == addr) return default_addr;
	if (addr == EVM_MAX_I2CADDR) {
		EVM_ADDR_OUT |= EVM_ADDR_BIT;
		default_addr = EVM_MAX_I2CADDR;
	}
	else if (addr == EVM_MIN_I2CADDR) {
		EVM_ADDR_OUT &= ~EVM_ADDR_BIT;
		default_addr = EVM_MIN_I2CADDR;
	}
	return default_addr;
}*/

//void evm_setLDCLK(evm_ldclk_state_t state) {
//	uint16_t reg;
//	switch (state) {
//	case OFF:
//		EVM_LDCLK_DIR &= ~EVM_LDCLK_BIT;   // Set to input (hiz)
//		EVM_LDCLK_SEL &= ~EVM_LDCLK_BIT;   // disable function
//		clockState = state;
//		break;
//	case DIV4: case DIV8: case DIV16: case DIV32:
//		EVM_LDCLK_DIR |= EVM_LDCLK_BIT;   // LDC CLK for Freq counter (set to output ACLK)
//		EVM_LDCLK_SEL |= EVM_LDCLK_BIT;   // LDC CLK for freq counter (set to output ACLK)
//		reg = EVM_LDCLK_REG;
//		reg &= ~EVM_LDCLK_MASK;
//		reg |= state;
//		EVM_LDCLK_REG = reg;
//		clockState = state;
//		break;
//	default:
//		break;
//	}
//}

//evm_ldclk_state_t evm_readLDCLK() {
//	return clockState;
//}

