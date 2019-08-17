#ifndef __OV7670_H
#define __OV7670_H
#include "sys.h"
#include "sccb.h"

#define DCMI_PWDN		PDout(3)  	//掉电模式， 
#define DCMI_RST		PDout(4)		//复位

u8 OV7670_init(void);
void OV7670_Light_Mode(u8 mode);
void OV7670_Color_Saturation(u8 sat);
void OV7670_Brightness(u8 bright);
void OV7670_Contrast(u8 contrast);
void OV7670_Special_Effects(u8 eft);
void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height);
void GPIO_Start(void);

#endif






















