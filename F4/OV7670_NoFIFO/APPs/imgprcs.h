#ifndef __imgprcs_h
#define __imgprcs_h
#include "sys.h"
#include "delay.h"
#include "OV7670.h"
#include "sccb.h"
#include "OV7670cfg.h"
#include "usart.h"
#include "dcmi.h" 

#define COLORAMOUNT 5

typedef struct 
{
	u16 acre;			//��������ص�����
	u32 area_x;		//x����  ��ͼ������Ϊԭ�㣩
	u32 area_y;		//y����
	u8 color;			//��ɫ��������λһλλ�洢,KΪ00000010,R00000100,G00001000,B00010000,Y00100000 ʹ�ø�ɫ���Ե��ӣ����ڼ����ɫ�ظ�ʶ��
	u8 Y_N;       //�Ƿ���Բ
	u8 get;       //Ҳ����Ϊ�뾶��С�뾶��
	u8 leave;			//��뾶
}ImgPrcs_TypeDef;

typedef struct   
{
	u16 StdR;
	u16 StdG;
	u16 StdB;
	char R_devi;
	char G_devi;
	char B_devi;
}StdColor_TypeDef;

typedef struct
{
	u16 area_last;
	u16 area_curr;
	u8 x;
	u8 y;
}ColorPosi_TypeDef;

u8 get_stdcolor(void); 
void img_display(u8 img[120][120]);
void roberts(u8 in[120][120],int rows,int cols);
int abs(int i);
u8 area_grow(void) ;
void data_output(u16 k , u8 area_num2 , ImgPrcs_TypeDef area_info[20] , u8 data_valid);
u8 get_balloon(void);
void devition_init(void);
u8 get_middle(u8 color[5]);
void color_transfer(u8 which_f, u8 color, u8 x, u8 y, u16 area);
void get_point(u8 posi[2]);


#endif

