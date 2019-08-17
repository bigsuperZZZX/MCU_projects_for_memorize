#include "stm32f4xx.h"

#ifndef EASY_TRACER_H
#define EASY_TRACER_H

#define IMG_X 0	  //图片x坐标
#define IMG_Y 0	  //图片y坐标
#define IMG_W 320 //图片宽度
#define IMG_H 240 //图片高度

#define ALLOW_FAIL_PER 3 //容错率，每1<<ALLOW_FAIL_PER个点允许出现一个错误点，容错率越大越容易识别，但错误率越大
#define ITERATE_NUM    8 //迭代次数，迭代次数越多识别越精确，但计算量越大


#define	IMG_MAX_X	240
#define IMG_MAX_Y	320
#define CUBEORDER				3						//魔方阶数
#define COLOR_NUM				7						//颜色数		包括魔方底色

typedef struct{
    unsigned int  H_MIN;//目标最小色调
    unsigned int  H_MAX;//目标最大色调	
    
	unsigned char  S_MIN;//目标最小饱和度  
    unsigned char  S_MAX;//目标最大饱和度
	
	unsigned char  L_MIN;//目标最小亮度  
    unsigned char  L_MAX;//目标最大亮度
	
	unsigned int  WIDTH_MIN;//目标最小宽度
	unsigned int  HIGHT_MIN;//目标最小高度

	unsigned int  WIDTH_MAX;//目标最大宽度
	unsigned int  HIGHT_MAX;//目标最大高度
	
	unsigned int x_init;
	unsigned int y_init;
	
	unsigned char flag_temp;
}TARGET_CONDI;//判定为的目标条件

typedef struct{
	unsigned int x;//目标的x坐标
	unsigned int y;//目标的y坐标
	unsigned int w;//目标的宽度
	unsigned int h;//目标的高度
}RESULT;//识别结果


typedef struct{
    unsigned char  red;             // [0,255]
    unsigned char  green;           // [0,255]
    unsigned char  blue;            // [0,255]
}COLOR_RGB;//RGB格式颜色

typedef struct{
    unsigned char hue;              // [0,240]
    unsigned char saturation;       // [0,240]
    unsigned char luminance;        // [0,240]
}COLOR_HSL;//HSL格式颜色

typedef struct{
    unsigned int X_Start;              
    unsigned int X_End;
	unsigned int Y_Start;              
    unsigned int Y_End;
}SEARCH_AREA;//区域

typedef	struct 
	{
		int xmin;
		int xmax;
		int ymin;
		int ymax;
		int xmin_temp;
		int xmax_temp;
		int ymin_temp;
		int ymax_temp;
		int x_step;
		int y_step;
	}ScanRange_TypeDef;

typedef struct point
{
	u16	x;
	u16 y;
}Point_TypeDef;

typedef struct hsicolor
{
	int16_t	H;
	int16_t	S;
	int16_t	I;
}HSI_ColorTypeDef;

typedef struct rgbcolor
{
	u8	R;
	u8	G;
	u8	B;
}RGB_ColorTypeDef;

typedef struct comparecolor
{
	HSI_ColorTypeDef	ColorMin;
	HSI_ColorTypeDef	ColorMax;
}Comp_ColorTypeDef;

typedef struct searcharea
{
	u16	xs;
	u16 ys;
	u16 xe;
	u16 ye;
}SearchArea_TypeDef;

typedef struct targetproperty
{
  Comp_ColorTypeDef		ColorCondition;			//颜色判定条件	
	SearchArea_TypeDef	SearchArea;	
	u8									ReadInterval	;			//采集点数间隔值

}TargetProperty_TypeDef;//判定为的目标条件

void RGBtoHSL(const COLOR_RGB *Rgb, COLOR_HSL *Hsl);
	

//唯一的API，用户将识别条件写入Condition指向的结构体中，该函数将返回目标的x，y坐标和长宽
//返回1识别成功，返回1识别失败
int Trace(TARGET_CONDI *Condition,RESULT *Resu);

#endif
