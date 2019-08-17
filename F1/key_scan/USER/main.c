//按键接在PD8到PD15上

#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "key_scan.h"



 int main(void)
 {	u8 t=0;
	 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能A端口时钟
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);	  //初始化GPIOD3,6
	 GPIO_SetBits(GPIOC,GPIO_Pin_1);
		delay_init();	    	 //延时函数初始化	  
		OLED_Init();			//初始化OLED  
	  key_init();
		OLED_ShowCHinese(10,4,0);
	 while(1)
	 {
		 if((t=key_scan())!=0)
		 {
			 OLED_ShowNum(80,0,t,4,16);
			 if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_1)==0)
				 GPIO_SetBits(GPIOC,GPIO_Pin_1);
			 else
				 GPIO_ResetBits(GPIOC,GPIO_Pin_1);
		 }
	 }
}

