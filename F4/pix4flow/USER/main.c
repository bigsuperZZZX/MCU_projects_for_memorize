#include "sys.h"
#include "delay.h"
#include "usart.h"

int main(void)
{ 
	Clock_Config();
	delay_init(168);		//��ʱ��ʼ�� 
	uart_init(115200);	//���ڳ�ʼ��������Ϊ115200
	while(1)
	{
		;
	}
}

