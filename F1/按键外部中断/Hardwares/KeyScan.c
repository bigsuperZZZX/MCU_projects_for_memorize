#include "stm32f10x.h"
#include "KeyScan.h"
#include "delay.h"

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);
	
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
		GPIO_InitStructure.GPIO_Pin=K_UP;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
		GPIO_InitStructure.GPIO_Pin=K_DOWN|K_LEFT|K_RIGHT;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA,K_UP);

}


void key_pros(void)
{
	if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))==1)
	{
		delay(80000);
		if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))==1)
		{
			GPIO_ResetBits(GPIOC,GPIO_Pin_0);
		}
		while((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))==1);
		GPIO_SetBits(GPIOC,GPIO_Pin_0);
	}
	
	if(READ_KeyDown==0)
	{
		delay(80000);
		if(READ_KeyDown==0)
		{
			GPIO_ResetBits(GPIOC,GPIO_Pin_1);
		}
		while(READ_KeyDown==0);
		GPIO_SetBits(GPIOC,GPIO_Pin_1);
	}
	
	if(READ_KeyLeft==0)
	{
		delay(80000);
		if(READ_KeyLeft==0)
		{
			GPIO_ResetBits(GPIOC,GPIO_Pin_2);
		}
		while(READ_KeyLeft==0);
		GPIO_SetBits(GPIOC,GPIO_Pin_2);
	}
	
	if(READ_KeyRight==0)
	{
		delay(80000);
		if(READ_KeyRight==0)
		{
			GPIO_ResetBits(GPIOC,GPIO_Pin_3);
		}
		while(READ_KeyRight==0);
		GPIO_SetBits(GPIOC,GPIO_Pin_3);
	}
}
