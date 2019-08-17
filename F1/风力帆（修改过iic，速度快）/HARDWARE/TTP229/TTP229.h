#ifndef _PPT229_H_
#define _PPT229_H_

#include "stm32f10x.h"
 //PA12 SCL PA11 SDO  
#define SCLH()  GPIO_SetBits(GPIOA,GPIO_Pin_12)
#define SCLL()  GPIO_ResetBits(GPIOA,GPIO_Pin_12)
#define SDOH()  GPIO_SetBits(GPIOA,GPIO_Pin_11)
#define SDOL()  GPIO_ResetBits(GPIOA,GPIO_Pin_11)
#define SDO()   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)
#define SDO_IN()  {GPIOA->CRH&=0XFFFF0FFF;GPIOA->CRH|=8<<12;}
#define SDO_OUT() {GPIOA->CRH&=0XFFFF0FFF;GPIOA->CRH|=3<<12;}

void TTP229_init(void);
u16 get_key(void);
u8 get_key2(u8* Pressed2);


#endif

