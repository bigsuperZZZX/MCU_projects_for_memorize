#include "r_cg_macrodriver.h"
#include "CUMT_image.h"
#include "CUMT_IIC.h"
#include "CUMT_delay.h"
#include "CUMT_filter.h"
#include "EasyTracer.h"

extern COLOR_HSL Gray2[24][24];
uint8_t uart1_rx_data2[20];
uint16_t row[32];
uint16_t img[24*24]; 
uint16_t *img_p = img;
/***************目标特征 = {Hmin,Hmax,Smin,Smax,Lmin,Lmax,WidMin ,HeiMin,WidMax  ,HeiMax    }; */
TARGET_CONDI condition  = {  0 ,  0 ,  0  , 0 ,  0  , 0 ,   0   ,   0   ,23  ,    23    };
RESULT result;

#define thr 21
#define l_thr 40
#define s_thr 40

#define COLORED 0x01
#define BLACKED 0x02

#define GET_COLOR 1
#define NOT_GET_COLOR 0


/*uint8_t img1_4[160*12];
uint8_t* pimg1_4 = img1_4;*/

uint8_t rows_count = 0;

void dot_track(uint8_t get_color_cmd , uint8_t color_black_select)
{
	static uint16_t x=12,y=12,x_temp,y_temp,x_color_last =12 , y_color_last =12 , x_black_last =12, y_black_last =12;
	static uint16_t x_color =12 , y_color =12 , x_black =12 , y_black =12;
	static COLOR_RGB pix_RGB;
	static COLOR_HSL pix_HSL;
	signed int i,j;
	unsigned long sum_color_x = 0 , sum_color_y = 0,  sum_color_count = 0 ,sum_black_x = 0 , sum_black_y = 0, sum_black_count = 0;;
	for(y=0;y<24;y+=2)
	{
		for(x=0;x<24;x+=2)
		{
			pix_RGB.red   =	 (unsigned char)((img[y*24+x]&0xf800)>>8);
			pix_RGB.green =	 (unsigned char)((img[y*24+x]&0x07e0)>>3);
			pix_RGB.blue  =     (unsigned char)((img[y*24+x]&0x001f)<<3);
			RGBtoHSL(&pix_RGB, &pix_HSL);
			if(color_black_select & BLACKED)   //处理黑白，识别黑色
			{
				if(pix_HSL.luminance > condition.L_MIN && pix_HSL.luminance < condition.L_MAX) 
				{
					sum_black_x += x;
					sum_black_y += y;
					sum_black_count++;
					img[y*24+x] = 0x07e0;
				}
			}
			if(color_black_select & COLORED)   //处理彩色
			{
				if((pix_HSL.hue > 24-thr || pix_HSL.hue < thr) && (condition.H_MIN > condition.H_MAX))
				{
					if((pix_HSL.hue < condition.H_MIN || pix_HSL.hue > condition.H_MAX) &&
					pix_HSL.saturation > condition.S_MIN && pix_HSL.saturation < condition.S_MAX)
					{
						sum_color_x += x;
						sum_color_y += y;
						sum_color_count++;
						img[y*24+x] = 0x0000;
					}
				}
				else if(pix_HSL.hue > condition.H_MIN && pix_HSL.hue < condition.H_MAX &&
						pix_HSL.saturation > condition.S_MIN && pix_HSL.saturation < condition.S_MAX)
				{
					sum_color_x += x;
					sum_color_y += y;
					sum_color_count++;
					img[y*24+x] = 0x0000;
				}
			}
		}
	}
			
	if(color_black_select & COLORED)   //处理黑白，识别黑色
	{
		x = sum_color_x/sum_color_count;
		y = sum_color_y/sum_color_count;
		sum_color_count = 0;
		x_temp = x;
		y_temp = y;
		if(x<16) x_temp=16;
		if(x>238) x_temp=238;
		if(x_temp%2) x_temp += 1;
		if(y<16) y_temp=16;
		if(y>238) y_temp=238;
		if(y_temp%2) y_temp += 1;
		for(i=-16;i<=16;i+=2)
		{
			for(j=-16;j<=16;j+=2)
			{
				if(img[(i+y_temp)*24+x_temp+j] == 0x0000)
				{
					sum_color_count ++;
				}
			}
		}
		if(sum_color_count < 50)
		{
			x_color = x_color_last;
			y_color = y_color_last;
		}
		else
		{
			x_color = x;
			y_color = y;
			x_color_last = x_color;
			y_color_last = y_color;
		}
				
		x_color = 24- x_color ;
				
	}
			
	if(color_black_select & BLACKED)   //处理黑白，识别黑色
	{
		x = sum_black_x/sum_black_count;
		y = sum_black_y/sum_black_count;
		sum_black_count = 0;
		x_temp = x;
		y_temp = y;
		if(x<16) x_temp=16;
		if(x>24) x_temp=24;
		if(x_temp%2) x_temp += 1;
		if(y<16) y_temp=16;
		if(y>24) y_temp=24;
		if(y_temp%2) y_temp += 1;
		for(i=-16;i<=16;i+=2)
		{
			for(j=-16;j<=16;j+=2)
			{
				if(img[(i+y_temp)*24+x_temp+j] == 0x07e0)
				{
					sum_black_count ++;
				}
			}
		}
		if(sum_black_count < 50)
		{
			x_black = x_black_last;
			y_black = y_black_last;
		}
		else
		{
			x_black = x;
			y_black = y;
			x_black_last = x_black;
			y_black_last = y_black;
		}
				
		x_black = 24- x_black ;
	}
}

void line_track(void)
{
	uint8_t line_count = 0, i=0 ,j=9;
//	static uint16_t temp = 110;
	static uint16_t x=12,y=12;
	static signed char line_angle = 0;
	static unsigned char line_mid = 0;
	unsigned long sum_y[10] = {0,0,0,0,0,0,0,0,0,0} , sum_count[10] = {0,0,0,0,0,0,0,0,0,0} ;
	static signed int y_position[10], last_y_position[10]  = {12,12,12,12,12,12,12,12,12,12};
//	static uint8_t last_sum_y[10] = {12,12,12,12,12,12,12,12,12,12};
	static COLOR_RGB pix_RGB;
	static COLOR_HSL pix_HSL;
	for(x=12;x<24;x+=24)
	{
		line_count = (x-12)/24;
		sum_count[line_count] = 0;
		for(y=0;y<24;y++)
		{
			pix_RGB.red   =	 (unsigned char)((img[y*24+x]&0xf800)>>8);
			pix_RGB.green =	 (unsigned char)((img[y*24+x]&0x07e0)>>3);
			pix_RGB.blue  =     (unsigned char)((img[y*24+x]&0x001f)<<3);
			RGBtoHSL(&pix_RGB, &pix_HSL);
					/****寻黑线****/
				/*	if(pix_HSL.luminance > condition.L_MIN && pix_HSL.luminance < condition.L_MAX) 
					
					{
						sum_y[line_count] += y;
						sum_count[line_count]++;
						img[y*24+x] = 0x07e0;
					}*/
					/****寻彩线****/
					if((pix_HSL.hue > 24-thr || pix_HSL.hue < thr) && (condition.H_MIN > condition.H_MAX))
					{
						if((pix_HSL.hue < condition.H_MIN || pix_HSL.hue > condition.H_MAX) &&
						 pix_HSL.saturation > condition.S_MIN && pix_HSL.saturation < condition.S_MAX)
						{
						sum_y[line_count] += y;
						sum_count[line_count]++;
						img[y*24+x] = 0x07e0;
						}
					}
					else if(pix_HSL.hue > condition.H_MIN && pix_HSL.hue < condition.H_MAX &&
						 pix_HSL.saturation > condition.S_MIN && pix_HSL.saturation < condition.S_MAX)
					{
						sum_y[line_count] += y;
						sum_count[line_count]++;
						img[y*24+x] = 0x07e0;
					}
				if(sum_count[line_count] > 3)  //线足够宽
				{
					y_position[line_count] = sum_y[line_count]/sum_count[line_count];
					
					//镜头畸变矫正
					switch(line_count)
					{
						case 0:;
						case 9: y_position[line_count] = 88 + (y_position[line_count] - 88)*112/100; break;
						case 1:;
						case 8: y_position[line_count] = 88 + (y_position[line_count] - 88)*107/100; break;
						case 2:;
						case 7: y_position[line_count] = 88 + (y_position[line_count] - 88)*105/100; break;
						case 3:;
						case 6: y_position[line_count] = 88 + (y_position[line_count] - 88)*105/100; break;
						case 4:;
						case 5: break;
					}
					if(y_position[line_count]>255) y_position[line_count] = 255;
					if(y_position[line_count]< 0 ) y_position[line_count] = 0;
					last_y_position[line_count] = y_position[line_count];
				}
				else
				{
					y_position[line_count] = last_y_position[line_count];
				}	
			}
			
			//从前后分别开始寻找有效的采集值
			while(sum_count[i] < 3) i++; 
			while(sum_count[j] < 3) j--;
			if(j==i) j+=1;
			line_angle = (y_position[i] - y_position[j])*4/(j-i);
			line_mid = (y_position[i] + y_position[j])/2;
		}
		
}


uint8_t decode_frame(void)
{
	if(uart1_rx_data2[0] == 0xb0)
		return COLORED;
	if(uart1_rx_data2[0] == 0xb1)
		return BLACKED;
	return 0xff;
}

uint8_t decode_frame_get_color_cmd(void)
{
	if(uart1_rx_data2[0] == 0xb3)
	{
		uart1_rx_data2[0] = 0x00;
		return GET_COLOR;
	}
	else
	{
		return NOT_GET_COLOR;
	}
}


unsigned short ReadPoint(unsigned short x,unsigned short y)
{
	return *(img_p + y*24 + x);
}







