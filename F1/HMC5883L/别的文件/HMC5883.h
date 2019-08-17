#ifndef _HMC5883_H_
#define _HMC5883_H_
#include "i2c.h"

extern u8 HMC5883_BUF[6];

void HMC5883_Init(void);
void HMC5883_Read_6Data(void);


#endif
