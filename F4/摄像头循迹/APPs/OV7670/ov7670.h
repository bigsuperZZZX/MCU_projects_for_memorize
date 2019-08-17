#ifndef _OV7670_H
#define _OV7670_H
#include "sys.h"
#include "sccb.h"

#define OV7670_VSYNC  	PFin(9)		//同步信号检测IO
#define OV7670_RRST		PEout(5)  	//读指针复位
#define OV7670_WRST		PEout(3)		//写指针复位
#define OV7670_CS			PCout(13)  	//片选信号(OE)
#define OV7670_WREN		PCout(3)		//写入FIFO使能
#define OV7670_RCK		PCout(1)		//读数据时钟
															  					 
#define OV7670_DATA   GPIOF->IDR&0x00FF  					//数据输入端口
/////////////////////////////////////////									
	    				 
u8   OV7670_Init(void);		  	   		 
void OV7670_Light_Mode(u8 mode);
void OV7670_Color_Saturation(u8 sat);
void OV7670_Brightness(u8 bright);
void OV7670_Contrast(u8 contrast);
void OV7670_Special_Effects(u8 eft);
void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height);
void camera_convert_display(void);


#endif





















