#ifndef __OV7725_H
#define __OV7725_H

#include "OV7725_REG.h"  
#include "sys.h"
#define uint8 unsigned char  
#define uint16 unsigned short 

#define DCMI_PWDN		PDout(3)  	//����ģʽ�� 
#define DCMI_RST		PDout(2)		//��λ  
   
//��������ͷ ����?  
#define MAX_ROW            20                                    //��������ͷͼ����  
#define MAX_COL            200                                    //��������ͷͼ��߶�  
  
extern  uint8   ov7725_eagle_init(uint8 *imgaddr);  


void GPIO_Start(void);
void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height);
u8 OV7725_init(void);
  




#endif




