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
#include "encoder.h"

extern int sys_pulsecount;

int main(void)
{ 
	signed long pulse_cnt,pulse_cnt_last,pulse_now;  //脉冲数
	char dirction;          //方向
	
	NVIC_Configuration();
	delay_init();                 //延时初始化
	uart_init(115200);   //用到了定时器3的50ms中断
	sys_pulse();
	encoder_ExtiInit();

	while(1)
	{
		if(sys_pulsecount%1000 == 0)
		{
			pulse_cnt = read_encoder_pulse();
			dirction = read_encoder_dirction();
			pulse_now = pulse_cnt - pulse_cnt_last;
			pulse_cnt_last = pulse_cnt;
			printf("%d %d\r\n",pulse_now,dirction);
			delay_ms(1);
		}
	}
}

