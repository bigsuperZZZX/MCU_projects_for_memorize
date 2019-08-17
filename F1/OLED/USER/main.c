//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : HuangKai
//  生成日期   : 2014-0101
//  最近修改   : 
//  功能描述   : OLED 4接口演示例程(51系列)
//              说明: 
//              ----------------------------------------------------------------
//              GND    电源地
//              VCC  接5V或3.3v电源
//              D0   接PA5（SCL）
//              D1   接PA7（SDA）
//              RES  接PB0
//              DC   接PB1
//              CS   接PA4               
//              ----------------------------------------------------------------
// 修改历史   :
// 日    期   : 
// 作    者   : HuangKai
// 修改内容   : 创建文件
//版权所有，盗版必究。
//Copyright(C) 中景园电子2014/3/16
//All rights reserved
//******************************************************************************/


#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"
#include "stdlib.h"
 int main(void)
 {	
 u32 i,k=127;
	 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOD3,6
 	GPIO_ResetBits(GPIOB,GPIO_Pin_2);	
		delay_init();	    	 //延时函数初始化	  
		OLED_Init();			//初始化OLED 
			 for(i=1;i<128;i++)
		 {
			 OLED_ShowPix(i,32-16*cos((float)i/8),'w');
			 delay_ms(10);
		 }
		 
	 while(k++)
	 {
		 OffSet_1();
		 OLED_ShowPix(127,rand()%128,'w');
		 delay_ms(10);
	 }
	/*for(i=0;i<10;i++)
	 {
	 OLED_ShowPix(i,5,1);
		delay_ms(500);
	 }
	 while(1);*/
}

