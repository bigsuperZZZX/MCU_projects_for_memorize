#ifndef _key_H
#define _key_H
#include "stm32f10x.h"

#define K_UP GPIO_Pin_0 //PA0
#define K_DOWN GPIO_Pin_3 //PE3
#define K_LEFT GPIO_Pin_2 //PE2
#define K_RIGHT GPIO_Pin_4 //PE4

#define k_up GPIO_ReadInputDataBit(GPIOA,K_UP)		  //获取按键的状态
#define k_down GPIO_ReadInputDataBit(GPIOE,K_DOWN)
#define k_left GPIO_ReadInputDataBit(GPIOE,K_LEFT)
#define k_right GPIO_ReadInputDataBit(GPIOE,K_RIGHT)

#define Key1 PBin(12)
#define Key2 PBin(13)
#define Key3 PBin(14)
#define Key4 PBin(15)

u8 Key_Scan(void);
void key_init(void);


#endif
