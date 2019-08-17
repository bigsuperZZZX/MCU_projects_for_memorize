#include "sys.h"
#include "delay.h"
#include "usart.h"
//#include "arm_math.h"

void GPIO_START(void);

u8 R[120][120];
u8 G[120][120];
u8 B[120][120];

int abs(int i)
{
	if(i<0) i=-i;
	return i;
}

struct 
{
	u32 m;
	u16 n;
	u16 t;
}A[5];

int main(void)
{ 
	Clock_Config();
	
	u16 i,j,k;
	u16 a=12,b=5,c=17,d=18,e=16,f=7;
	u32 x=0,y=0,z=0;
	u8 *pR,*pG,*pB; 
	pR=&(R[0][0]);
	pG=&(G[0][0]);
	pB=&(B[0][0]);
	
	delay_init(168);		//延时初始化 
	uart_init(256000);	//串口初始化波特率为115200
	delay_ms(1500);
	GPIO_START();
	while(1)
	{
		PCout(4)=0;
		
		for(i=0;i<120;i++)
			for(j=0;j<120;j++)
				for(k=0;k<5;k++)
					if(abs(R[i][j]-A[k].t) <= A[k].n &&
						 abs(G[i][j]-A[k].t) <= A[k].n &&
						 abs(B[i][j]-A[k].t) <= A[k].n)
					{
						A[k].m++;
						A[k].m += j;
						A[k].m += i;
					}
		
		PCout(4)=1;
		delay_us(10);
		PCout(4)=0;
					
		for(i=0;i<120;i++)
			for(j=0;j<120;j++)
				for(k=0;k<5;k++)
					if(abs(*(pR+i*120+j)-A[k].t) <= A[k].n &&
						 abs(*(pG+i*120+j)-A[k].t) <= A[k].n &&
						 abs(*(pB+i*120+j)-A[k].t) <= A[k].n)
					{
						A[k].m++;
						A[k].m += j;
						A[k].m += i;
					}	
					
		PCout(4)=1;
		delay_us(10);			
	}
}

void GPIO_START(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOA时钟
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); 
}

