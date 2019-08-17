#ifndef _DS18B20_H
#define _DS18B20_H
#include "stm32f10x.h"
#define dq (GPIO_Pin_1) //PA1
#define GPIO_ds18b20 GPIOA
#define ds18b20_dq_H GPIO_SetBits(GPIO_ds18b20,dq)
#define ds18b20_dq_L GPIO_ResetBits(GPIO_ds18b20,dq)
void ds18b20_init(void);
void DQOUTINT(void);	 // ‰≥ˆ≈‰÷√
void DQININT(void);	 // ‰»Î≈‰÷√
void ds18b20init(void);
void ds18b20wr(u8 dat);
u8 DS18b20rd(void);
double readtemp(void);


#endif
