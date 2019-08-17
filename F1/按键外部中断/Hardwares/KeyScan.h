#ifndef _KeyScan_H
#define _KeyScan_H
#include "stm32f10x.h"

#define K_UP GPIO_Pin_0
#define K_DOWN GPIO_Pin_3
#define K_LEFT GPIO_Pin_2
#define K_RIGHT GPIO_Pin_4

#define READ_KeyUp GPIO_ReadInputDataBit(GPIOA,K_UP)
#define READ_KeyDown GPIO_ReadInputDataBit(GPIOE,K_DOWN)
#define READ_KeyLeft GPIO_ReadInputDataBit(GPIOE,K_LEFT)
#define READ_KeyRight GPIO_ReadInputDataBit(GPIOE,K_RIGHT)

void KEY_Init(void);
void key_pros(void);
#endif
