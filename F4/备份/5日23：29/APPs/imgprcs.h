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
	u16 acre;			//面积（像素点数）
	u32 area_x;		//x坐标  （图像左上为原点）
	u32 area_y;		//y坐标
	u8 color;			//颜色，二进制位一位位存储,K为00000010,R00000100,G00001000,B00010000,Y00100000 使得各色可以叠加，便于检测颜色重复识别
	u8 Y_N;       //是否是圆
	u8 get;       //也可作为半径（小半径）
	u8 leave;			//大半径
}ImgPrcs_TypeDef;

typedef struct   
{
	u8 StdR;
	u8 StdG;
	u8 StdB;
}BaseColor_TypeDef;
	
typedef struct   
{
	BaseColor_TypeDef BaseColor_K;
	BaseColor_TypeDef BaseColor_R;
	BaseColor_TypeDef BaseColor_G;
	BaseColor_TypeDef BaseColor_B;
	BaseColor_TypeDef BaseColor_Y;
}StdColor_TypeDef;

u8 get_stdcolor(void); 
void img_display(u8 img[120][120]);
void roberts(u8 in[120][120],int rows,int cols);
int abs(int i);
u8 area_grow(void) ;
void data_output(u16 k , u8 area_num2 , ImgPrcs_TypeDef area_info[20] , u8 data_valid);


#endif

