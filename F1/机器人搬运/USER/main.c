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
#include "motor.h"
#include "sys.h"

/*已经封装到不能再简单了。
 *完全不用考虑时序问题。
 *一圈的五个电机，只要函数输入对应的电机号和上下方向就行。
 *顶上的电机只需要输入角度，控制方法像舵机那样。正前方为0度，顺时针为正，每个角度转向一个对应点（-180——+180）
 *下方舵机只要输入速度值就行，0-50，25是静止不动，越靠近0，向前越快，越靠近50，向后越快
 *移植到你的工程去的话，只要移植usart.c/h和motor.c/h这四个文件就行，前一个实现无线串口传输和DMA方式读取，后一个是所有电机的驱动
 *如果要动硬件，一定要跟我说，所有设备都是没有保护的，很容易烧！！！用热熔胶加固之后一定要把丝去掉！！！
 *发热特别特别严重，每次使用时间在三分钟内！！！
 */


extern uint8_t Uart_Rx[UART_RX_LEN];  //DMA读取到的串口接收值
signed int angle;

extern u8 key_clicked;  //按一次，这个变1，再按一次，这个变零

int main(void)
{
	signed int i=0;
	u8 dir=0;
	JTAG_Set(SWD_ENABLE);  //一定要要，要不然PB3不能用
	delay_init();          //就原子那个
	uart_init(115200);     //为了使用无线串口，需要拉高PA4（无线模块上的AUX）
	motor_Init();   			 //TIM1和TIM2都在这里被用掉了
	
	while(1)
	{
		set_paw_act(1,DOWN);  //使用范例
		set_paw_act(2,UP);
		set_paw_act(3,DOWN);
		set_paw_act(4,UP);
		set_paw_act(5,DOWN);
		if(dir == 0)
		{
			i+=59;
			if(i>180)
			{
				i=177;
				dir=1;
			}
		}
		else
		{
			i-=59;
			if(i<-180)
			{
				i=-177;
				dir=0;
			}
		}
		set_top_angle(i);
		set_speed_L(50-(i+180)/8);
		set_speed_R(50-(i+180)/8);
		delay_ms(1800);
		delay_ms(1800);
		set_paw_act(1,UP);
		set_paw_act(2,DOWN);
		set_paw_act(3,UP);
		set_paw_act(4,DOWN);
		set_paw_act(5,UP);
		if(dir == 0)
		{
			i+=59;
			if(i>180)
			{
				i=177;
				dir=1;
			}
		}
		else
		{
			i-=59;
			if(i<-180)
			{
				i=-177;
				dir=0;
			}
		}
		set_top_angle(i);
		set_speed_L(50-(i+180)/8);
		set_speed_R(50-(i+180)/8);
		delay_ms(1800);
		delay_ms(1800);
		printf("%d,\r\n",i);
	}
}

