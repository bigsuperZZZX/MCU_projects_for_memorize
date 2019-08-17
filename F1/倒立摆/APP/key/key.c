#include "key.h"
#include "sys.h"
#include "systick.h"

/*******************************************************************************
* 函 数 名         : key_init
* 函数功能		   : 按键端口初始化函数	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void key_init()	   //
{
	GPIO_InitTypeDef GPIO_InitStructure;	  //声明一个结构体变量，用来初始化GPIO

	SystemInit();
	/* 开启GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	   //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	   //设置传输速率
	GPIO_Init(GPIOB,&GPIO_InitStructure);		  /* 初始化GPIO */
	
	/*  配置GPIO的模式和IO口 */
}

u8 Key_Scan()
{
	u8 key;
	if(Key1==0)
	{
		delay_ms(2);
		if(Key1==0)
		{
			key=1;
			while(Key1==0);
		}
	}
	else if(Key2==0)
	{
		delay_ms(2);
		if(Key2==0)
		{
			key= 2;
			while(Key2==0);
		}
	}
	 else if(Key3==0)
	{
		delay_ms(2);
		if(Key3==0)
		{
			key= 3;
			while(Key3==0);
		}
	}
	else if(Key4==0)
	{
		delay_ms(2);
		if(Key4==0)
		{
			key= 4;
			while(Key4==0);
		}
	}
	return key;
}
