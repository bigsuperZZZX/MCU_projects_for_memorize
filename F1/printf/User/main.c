#include "stm32f10x.h"
#include "printf.h"
#include "stdio.h"
#include "delay.h"

/*int main(void)
{	
	u32 d1=123;
	float d2=12.34;
	printf_init();
	while(1)
	{	printf("abcdefg\r\n");
		delay(8000000);
		delay(8000000);
	}
}*/


int main()
{	
	u32 d1=123;
	float d2=12.34;
	printf_init();	//printf串口初始化
	while(1)
	{
		printf("深圳普中科技有限公司\r\n");
		printf("www.prechin.com\r\n");
		printf("to d1 a int number %d\r\n",d1); //整型数
		printf("to d2 a float number %f\r\n",d2);//浮点数
		printf("change hexfile is %X\r\n",d1);//16进制
		printf("change 0file is %o\r\n",d1); //八进制
		delay(8000000);
	}
}


