/*******************************************************************************
*  Copyright (C) 2010 - All Rights Reserved
*		
* ��������:	��ɼ���
* ��Ȩ����: ��ɵ���	
* ��������:	2012��8��18�� 
* ������ʷ:	2012��8��18���װ�
* Version:  1.0 
* Demo �Ա���ַ��http://store.taobao.com/shop/view_shop.htm?asker=wangwang&shop_nick=qifeidianzi
**********************************************************************************************************************************************
����������������������������������������������������������������������������������������������������������������������������������������������
����������������������һ������������������������������������������������������������һ����������������������������һһһһһһһ��������������
������һһһһ��һһһһһһ������������һһһһһһһһ�������������������ą�����һһ������һ������������������һһһһ����һһһ������������
������������һ������һ��һ������������һһһһ�ą���һ����һһ������������һһһһһһһһһһһ������������������������һһһһ��������������
�������ą���һ��������������������������������������һ��һһ��������������һһ����һһ������һһ������������������������һһ������������������
������һ����һһһһһһһһһһһ������������������һһ������������������һһһһһһһһһһ������������һһһһһһһһһһһһһһһ������
������һһһһһ����һһ��һ����������������������һһһһһ��������������һһ����һһ������һ������������һһ����������һһ������һһ��������
��������������һ��һ��һ��һ������������������������һ����һһ������������һһһһһһһһһһ��������������������������һһ������������������
����һһһ��������һһһ��һ������������������������һһ����������������������������һ��������������һ������������������һһ������������������
������������һ����������һһ��������������������������һһһһһһ������������������һһһһһһһһһ������������������һһ������������������
��������һһһ������һһһһ����������������������������һһһһһ��������������������һһһһһһһ��������������һһһһ��������������������
����������������������������������������������������������������������������������������������������������������������������������������������
**********************************************************************************************************************************************/

#ifndef OV7670_H
#define OV7670_H

#define OV7670_DEVICE_WRITE_ADDRESS    0x42
#define OV7670_DEVICE_READ_ADDRESS     0x43

#define Cmos_VSYNC PCout(4)
#define Cmos_CS   PAout(0)
#define WriteControl PCout(13)

/*OV7670FIFOģ��������Ŷ���*/
#define FIFO_WR    PCout(10) 
#define FIFO_WRST  PAout(10)
#define FIFO_RCK   PAout(2)
#define FIFO_RRST  PAout(12)
#define FIFO_OE	   PAout(11)

void set_Cmos7670reg(void);	
unsigned char Cmos7670_init(void);
unsigned char WrCmos7670(unsigned char regID, unsigned char regDat);
unsigned char rdCmos7670Reg(unsigned char regID);
//void Cmos7670_Size(unsigned int Startx,unsigned int Starty,unsigned int width,unsigned int height);

#endif
