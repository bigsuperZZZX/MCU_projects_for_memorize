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
	printf_init();	//printf���ڳ�ʼ��
	while(1)
	{
		printf("�������пƼ����޹�˾\r\n");
		printf("www.prechin.com\r\n");
		printf("to d1 a int number %d\r\n",d1); //������
		printf("to d2 a float number %f\r\n",d2);//������
		printf("change hexfile is %X\r\n",d1);//16����
		printf("change 0file is %o\r\n",d1); //�˽���
		delay(8000000);
	}
}


