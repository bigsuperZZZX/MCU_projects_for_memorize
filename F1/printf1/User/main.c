/*******************************************************************************
*                 
*                 		       普中科技
--------------------------------------------------------------------------------
* 实 验 名		 : printf重定向实验
* 实验说明       : 通过串口打印数据，学习printf重定向 与C语言printf功能一样
* 连接方式       : 
* 注    意		 : 	所用函数在头文件.c文件内
*******************************************************************************/


#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "printf.h"

void delay(u32 i)
{
	while(i--);
}

/****************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
int main()
{	
	u32 d1=123;
	float d2=12.34;
	printf_init();	//printf串口初始化
	while(1)
	{
		printf("中文\r\n");
		printf("www.prechin.com\r\n");
		printf("to d1 a int number %d\r\n",d1); //整型数
		printf("to d2 a float number %f\r\n",d2);//浮点数
		printf("change hexfile is %X\r\n",d1);//16进制
		printf("change 0file is %o\r\n",d1); //八进制
		delay(4000000);	
	}
}



