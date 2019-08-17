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

/*�Ѿ���װ�������ټ��ˡ�
 *��ȫ���ÿ���ʱ�����⡣
 *һȦ����������ֻҪ���������Ӧ�ĵ���ź����·�����С�
 *���ϵĵ��ֻ��Ҫ����Ƕȣ����Ʒ���������������ǰ��Ϊ0�ȣ�˳ʱ��Ϊ����ÿ���Ƕ�ת��һ����Ӧ�㣨-180����+180��
 *�·����ֻҪ�����ٶ�ֵ���У�0-50��25�Ǿ�ֹ������Խ����0����ǰԽ�죬Խ����50�����Խ��
 *��ֲ����Ĺ���ȥ�Ļ���ֻҪ��ֲusart.c/h��motor.c/h���ĸ��ļ����У�ǰһ��ʵ�����ߴ��ڴ����DMA��ʽ��ȡ����һ�������е��������
 *���Ҫ��Ӳ����һ��Ҫ����˵�������豸����û�б����ģ��������գ����������۽��ӹ�֮��һ��Ҫ��˿ȥ��������
 *�����ر��ر����أ�ÿ��ʹ��ʱ�����������ڣ�����
 */


extern uint8_t Uart_Rx[UART_RX_LEN];  //DMA��ȡ���Ĵ��ڽ���ֵ
signed int angle;

extern u8 key_clicked;  //��һ�Σ������1���ٰ�һ�Σ��������

int main(void)
{
	signed int i=0;
	u8 dir=0;
	JTAG_Set(SWD_ENABLE);  //һ��ҪҪ��Ҫ��ȻPB3������
	delay_init();          //��ԭ���Ǹ�
	uart_init(115200);     //Ϊ��ʹ�����ߴ��ڣ���Ҫ����PA4������ģ���ϵ�AUX��
	motor_Init();   			 //TIM1��TIM2�������ﱻ�õ���
	
	while(1)
	{
		set_paw_act(1,DOWN);  //ʹ�÷���
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

