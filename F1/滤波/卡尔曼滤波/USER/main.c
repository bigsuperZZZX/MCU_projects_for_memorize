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
#include <time.h>
#include <stdio.h>

#define N 200

//ִ��һ�θó����еĿ������˲������Լ8΢��

float sig_filt[N]={1};
float sig_get[N];
float p[N]={1}; 
float all;

int main(void)
{ 
	
	int t,i;
	float Rvv=0.5;              //��������Э����  ������ֵ��������ֵ��������
  float Rww = 0.1;           //��ǰֵ����ǰֵ����ֵ�ĵ�������ֵ��������
	float p1;
	float Kg;
	
	delay_init();                 //��ʱ��ʼ��
	uart_init(115200);
	OLED_Init();
//	srand(time(0));
	
	for(t=0;t<N;t++)
		sig_get[t]=10*sin((float)t/40)+(float)(rand()%100)/50;
	
	delay_ms(1000);
	OLED_ShowChar(0,0,'A');
	
	for(i=0;i<10000;i++)
	{
		for(t=1;t<N;t++)
		{
			p1=p[t-1]+Rww;           //Э������� ��ǰʱ�̵Ĺ���ֵ��ƫ�aΪϵͳ������û�п�����������û�в���b��RwwΪ����   
			Kg=p1/(p1+Rvv);  	    //��Kg,KgΪKg����Kalman����   
			sig_filt[t]=sig_filt[t-1]+Kg*(sig_get[t]-sig_filt[t-1]);   //����� ��tʱ�̵�����ֵ������ǰʱ�̵�����ֵ   
			p[t]=p1-Kg*p1;              //��Э���� ��ǰ��״̬����ֵ��ƫ���ʽ��5:(1-c*Kg)*p1 
		}
		all+=sig_filt[i];
	}
	
	OLED_ShowChar(0,0,'Z');
  
	
  /*for(t=0;t<N;t++)
	{
		DataScope_Get_Channel_Data(sig_get[t], 1 );
		DataScope_Get_Channel_Data(sig_filt[t], 2 );
		for(i=0;i<DataScope_Data_Generate(2);i++) 
		{
			while((USART1->SR&0X40)==0);  
			USART1->DR = DataScope_OutPut_Buffer[i]; 
		}
		delay_ms(50); //20HZ
	} */

		while(1);
}

