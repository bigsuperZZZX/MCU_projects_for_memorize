#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "timer.h" 

//ALIENTEK 探索者STM32F407开发板  实验46_2
////FPU测试(Julia分形) 实验  -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

//46_2,本版本为关闭硬件FPU版本.

//FPU模式提示
#if __FPU_USED==1
#define SCORE_FPU_MODE                  "FPU On" 
#else
#define SCORE_FPU_MODE                  "FPU Off" 
#endif

#define 	ITERATION			128			//迭代次数
#define	 	REAL_CONSTANT		0.285f		//实部常量
#define	 	IMG_CONSTANT		0.01f		//虚部常量
//颜色表
u16 color_map[ITERATION];
//缩放因子列表
const u16 zoom_ratio[] =
{
	120, 110, 100, 150, 200, 275, 350, 450,
	600, 800, 1000, 1200, 1500, 2000, 1500,
	1200, 1000, 800, 600, 450, 350, 275, 200,
	150, 100, 110,
}; 

//初始化颜色表
//clut:颜色表指针
void InitCLUT(u16 * clut)
{
 	u32 i=0x00;
	u16  red=0,green=0,blue=0;  
	for(i=0;i<ITERATION;i++)//产生颜色表
	{
		//产生RGB颜色值
		red=(i*8*256/ITERATION)%256;
		green=(i*6*256/ITERATION)%256;
		blue=(i*4*256 /ITERATION)%256; 
 		//将RGB888,转换为RGB565
		red=red>>3;
		red=red<<11;
		green=green>>2;
		green=green<<5;
		blue=blue>>3; 
		clut[i]=red+green+blue;
	}
} 

//产生Julia分形图形
//size_x,size_y:屏幕x,y方向的尺寸
//offset_x,offset_y:屏幕x,y方向的偏移
//zoom:缩放因子
void GenerateJulia_fpu(u16 size_x,u16 size_y,u16 offset_x,u16 offset_y,u16 zoom)
{
	u8 i;
	u16 x,y;  
	float tmp1,tmp2;
	float num_real,num_img;
	float radius; 
	for(y=0;y<size_y;y++)
	{
       	for(x=0;x<size_x;x++)
   		{
       		num_real=y-offset_y;
         	num_real=num_real/zoom;
           	num_img=x-offset_x;
          	num_img=num_img/zoom;
           	i=0;
          	radius=0;
           	while((i<ITERATION-1)&&(radius<4))
           	{
           		tmp1=num_real*num_real;
               	tmp2=num_img*num_img;
              	num_img=2*num_real*num_img+IMG_CONSTANT;
             	num_real=tmp1-tmp2+REAL_CONSTANT;
               	radius=tmp1+tmp2;
              	i++;
           	}
			LCD->LCD_RAM=color_map[i];//绘制到屏幕 
       	}
	}
}
u8 timeout;

int main(void)
{ 
	delay_init(168);  //初始化延时函数
	uart_init(256000);		//初始化串口波特率为115200
	int i;
	double a=0;
	while(1)
	{	
		for(i=0;i<1000000;i++)
			a+=i*2.0;
		printf("%lf",a);
	}
}
