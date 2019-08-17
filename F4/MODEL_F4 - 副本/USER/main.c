#include "sys.h"
#include "delay.h"
#include "usart.h"


void GPIO_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10
   
}

int read_cmd()
{
	int cmd;
	cmd = GPIO_ReadInputData(GPIOA) & 0x0f;
	if(cmd == 12 || cmd == 10)
	{
		return cmd;
	}
	return cmd;
}

//动作表：
//0 ：
//1 ：前走
//2 ：后走
//3 ：左走
//4 ：右走
//5 ：左转
//6 ：右转
//7 ：向后起身
//8 ：向前起身
//9 ：左手左侧出拳
//10：
//11：
//12：
//13：
//14：
//15：

int main(void)
{ 
	int cmd;
	long i;
	
	delay_init(168);                 //延时初始化
	uart_init(115200);   //用到了定时器3的50ms中断
//	GPIO_init();

	while(1)
	{
		cmd = 1;
		cmd = read_cmd();
		if(cmd == 1 || cmd == 2)
		{
			delay_ms(50);
			cmd = read_cmd();
		}
		if(cmd != 0)
		{
			printf("#%dGC1\r\n",cmd);
			delay_ms(100);
			
			i = 0;
			while( (USART_ReceiveData(USART1) & 0xff) != 'F' )
			{
				i++;
				delay_us(1);
				if(i>2000000)
				{
					break;
				}
			}
		}
	}
	
}


