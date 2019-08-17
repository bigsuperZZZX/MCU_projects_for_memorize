#include <stm32f10x.h>
#include "key_scan.h"
#include "delay.h"


u8 key_num()
{
	if(((GPIO_ReadInputData(GPIOD)>>8)&0xf0)!=0xf0)
	{
		delay_ms(2);
		if(((GPIO_ReadInputData(GPIOD)>>8)&0xf0)!=0xf0)
			switch ((GPIO_ReadInputData(GPIOD)>>8))
			{
				case 0xe7:return 1;
				case 0xd7:return 2;
				case 0xb7:return 3;
				case 0x77:return 4;		
				case 0xeb:return 5;
				case 0xdb:return 6;
				case 0xbb:return 7;
				case 0x7b:return 8;		
				case 0xed:return 9;
				case 0xdd:return 10;
				case 0xbd:return 11;
				case 0x7d:return 12;
				case 0xee:return 13;
				case 0xde:return 14;
				case 0xbe:return 15;
				case 0x7e:return 16;
			}
	}
	return 0;
}	

u8 key_scan(void)
{
	char t=0;
	GPIO_ResetBits(GPIOD,GPIO_Pin_8);
	t=key_num();
	if(((GPIO_ReadInputData(GPIOD)>>8)&0xf0)!=0xf0)
		goto key_finish;
	else
		GPIO_SetBits(GPIOD,GPIO_Pin_8);
	GPIO_ResetBits(GPIOD,GPIO_Pin_9);
	t=key_num();
	if(((GPIO_ReadInputData(GPIOD)>>8)&0xf0)!=0xf0)
		goto key_finish;
	else
		GPIO_SetBits(GPIOD,GPIO_Pin_9);
	GPIO_ResetBits(GPIOD,GPIO_Pin_10);
	t=key_num();
	if(((GPIO_ReadInputData(GPIOD)>>8)&0xf0)!=0xf0)
		goto key_finish;
	else
		GPIO_SetBits(GPIOD,GPIO_Pin_10);
	GPIO_ResetBits(GPIOD,GPIO_Pin_11);
	t=key_num();
	if(((GPIO_ReadInputData(GPIOD)>>8)&0xf0)!=0xf0)
		goto key_finish;
	else
		GPIO_SetBits(GPIOD,GPIO_Pin_11);
	key_finish:
	while(((GPIO_ReadInputData(GPIOD)>>8)&0xf0)!=0xf0);
	GPIO_SetBits(GPIOD,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11);
	return t;
}


void key_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //使能A端口时钟
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	  //初始化GPIOD3,6
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	  //初始化GPIOD3,6
	 
	GPIO_SetBits(GPIOD,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11);
}

