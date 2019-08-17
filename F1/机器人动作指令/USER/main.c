#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "DataScope_DP.h"
#include "usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"

void GPIO_init()
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
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

//������
//0 ��
//1 ��ǰ��
//2 ������
//3 ������
//4 ������
//5 ����ת
//6 ����ת
//7 ���������
//8 ����ǰ����
//9 ����������ȭ
//10��
//11��
//12��
//13��
//14��
//15��

int main(void)
{ 
	int cmd;
	long i;
	
	delay_init();                 //��ʱ��ʼ��
	uart_init(115200);   //�õ��˶�ʱ��3��50ms�ж�
	GPIO_init();

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
			delay_ms(10);
			
			i = 0;
			while( (USART_ReceiveData(USART1) & 0xff) != 'F' )
			{
				i++;
				delay_us(1);
				if(i>1000000)
				{
					break;
				}
			}
			i++;
		}
	}
	
}

