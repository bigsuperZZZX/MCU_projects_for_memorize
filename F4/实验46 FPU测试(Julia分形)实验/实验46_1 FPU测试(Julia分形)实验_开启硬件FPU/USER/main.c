#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "arm_math.h" 

#if __FPU_USED==1
#define SCORE_FPU_MODE                  "FPU On" 
#else
#define SCORE_FPU_MODE                  "FPU Off" 
#endif
int main(void)
{ 
	delay_init(168);  //��ʼ����ʱ����
	uart_init(256000);		//��ʼ�����ڲ�����Ϊ115200
	int i;
	int a=0;
	while(1)
	{	
		for(i=0;i<1000000;i++)
			a+=1+2;
		printf("%d",a);
	}
}
 
