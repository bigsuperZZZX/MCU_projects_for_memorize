#include "TTP229.h"
#include "stm32f10x.h"
#include "delay.h"

void TTP229_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11;	  //PA12 SCL PA11 SDO    
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOD3,6
}

u16 get_key(void)
{
	u16 KeyBuff=0;
	char i;
	SDO_OUT();
	SCLL();
	SDOL();
	delay_us(10);
	SDOH();
	delay_us(93);
	SDOL();
	SDO_IN();
	delay_us(15);
	for(i=0;i<16;i++)
	{
		SCLH();
		delay_us(6);
		SCLL();
		KeyBuff <<= 1;
		KeyBuff |= SDO();
		delay_us(4);
	}
	return KeyBuff;
}


//返回值：当次按键是否改变，是1否0
u8 get_key2(u8* Pressed2)
{
	u8 i, Pressed_now=0;
	u16 Key;
	Key = get_key();
	if(Key != 0)
	{
		for(i=0;i<16;i++)
		{
			if((Key&(0x8000>>i)) != 0)
			{
				Pressed_now = i+1;
			}
		}
	}
	if(*Pressed2 != Pressed_now) 
	{
		*Pressed2 = Pressed_now;
		if(Pressed_now != 0)
			return 1;
	}
	return 0;
}


