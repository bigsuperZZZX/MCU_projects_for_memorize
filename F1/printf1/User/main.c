/*******************************************************************************
*                 
*                 		       ���пƼ�
--------------------------------------------------------------------------------
* ʵ �� ��		 : printf�ض���ʵ��
* ʵ��˵��       : ͨ�����ڴ�ӡ���ݣ�ѧϰprintf�ض��� ��C����printf����һ��
* ���ӷ�ʽ       : 
* ע    ��		 : 	���ú�����ͷ�ļ�.c�ļ���
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
	printf_init();	//printf���ڳ�ʼ��
	while(1)
	{
		printf("����\r\n");
		printf("www.prechin.com\r\n");
		printf("to d1 a int number %d\r\n",d1); //������
		printf("to d2 a float number %f\r\n",d2);//������
		printf("change hexfile is %X\r\n",d1);//16����
		printf("change 0file is %o\r\n",d1); //�˽���
		delay(4000000);	
	}
}



