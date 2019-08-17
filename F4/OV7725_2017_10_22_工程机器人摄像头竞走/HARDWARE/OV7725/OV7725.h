#ifndef __OV7725_H
#define __OV7725_H

#include "OV7725_REG.h"  
#include "sys.h"
#define uint8 unsigned char  
#define uint16 unsigned short 

#define DCMI_PWDN		PDout(3)  	//掉电模式， 
#define DCMI_RST		PDout(2)		//复位  
   
//配置摄像头 属性?  
#define MAX_ROW            20                                    //定义摄像头图像宽度  
#define MAX_COL            200                                    //定义摄像头图像高度  
  
extern  uint8   ov7725_eagle_init(uint8 *imgaddr);  


void GPIO_Start(void);
void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height);
u8 OV7725_init(void);
  




#endif




