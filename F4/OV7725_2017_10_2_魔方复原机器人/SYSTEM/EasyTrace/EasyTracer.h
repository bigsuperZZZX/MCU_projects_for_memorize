#include "stm32f4xx.h"

#ifndef EASY_TRACER_H
#define EASY_TRACER_H

#define IMG_X 0	  //ͼƬx����
#define IMG_Y 0	  //ͼƬy����
#define IMG_W 320 //ͼƬ���
#define IMG_H 240 //ͼƬ�߶�

#define ALLOW_FAIL_PER 3 //�ݴ��ʣ�ÿ1<<ALLOW_FAIL_PER�����������һ������㣬�ݴ���Խ��Խ����ʶ�𣬵�������Խ��
#define ITERATE_NUM    8 //������������������Խ��ʶ��Խ��ȷ����������Խ��


#define	IMG_MAX_X	240
#define IMG_MAX_Y	320
#define CUBEORDER				3						//ħ������
#define COLOR_NUM				7						//��ɫ��		����ħ����ɫ

typedef struct{
    unsigned int  H_MIN;//Ŀ����Сɫ��
    unsigned int  H_MAX;//Ŀ�����ɫ��	
    
	unsigned char  S_MIN;//Ŀ����С���Ͷ�  
    unsigned char  S_MAX;//Ŀ����󱥺Ͷ�
	
	unsigned char  L_MIN;//Ŀ����С����  
    unsigned char  L_MAX;//Ŀ���������
	
	unsigned int  WIDTH_MIN;//Ŀ����С���
	unsigned int  HIGHT_MIN;//Ŀ����С�߶�

	unsigned int  WIDTH_MAX;//Ŀ�������
	unsigned int  HIGHT_MAX;//Ŀ�����߶�
	
	unsigned int x_init;
	unsigned int y_init;
	
	unsigned char flag_temp;
}TARGET_CONDI;//�ж�Ϊ��Ŀ������

typedef struct{
	unsigned int x;//Ŀ���x����
	unsigned int y;//Ŀ���y����
	unsigned int w;//Ŀ��Ŀ��
	unsigned int h;//Ŀ��ĸ߶�
}RESULT;//ʶ����


typedef struct{
    unsigned char  red;             // [0,255]
    unsigned char  green;           // [0,255]
    unsigned char  blue;            // [0,255]
}COLOR_RGB;//RGB��ʽ��ɫ

typedef struct{
    unsigned char hue;              // [0,240]
    unsigned char saturation;       // [0,240]
    unsigned char luminance;        // [0,240]
}COLOR_HSL;//HSL��ʽ��ɫ

typedef struct{
    unsigned int X_Start;              
    unsigned int X_End;
	unsigned int Y_Start;              
    unsigned int Y_End;
}SEARCH_AREA;//����

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
  Comp_ColorTypeDef		ColorCondition;			//��ɫ�ж�����	
	SearchArea_TypeDef	SearchArea;	
	u8									ReadInterval	;			//�ɼ��������ֵ

}TargetProperty_TypeDef;//�ж�Ϊ��Ŀ������

void RGBtoHSL(const COLOR_RGB *Rgb, COLOR_HSL *Hsl);
	

//Ψһ��API���û���ʶ������д��Conditionָ��Ľṹ���У��ú���������Ŀ���x��y����ͳ���
//����1ʶ��ɹ�������1ʶ��ʧ��
int Trace(TARGET_CONDI *Condition,RESULT *Resu);

#endif
