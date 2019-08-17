#include "sys.h"
#include "delay.h"
#include "usart.h"

int main(void)
{ 
	Clock_Config();
	delay_init(168);		//延时初始化 
	uart_init(115200);	//串口初始化波特率为115200
	while(1)
	{
		;
	}
}

