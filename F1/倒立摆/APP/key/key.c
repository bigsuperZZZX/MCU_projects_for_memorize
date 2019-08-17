#include "key.h"
#include "sys.h"
#include "systick.h"

/*******************************************************************************
* �� �� ��         : key_init
* ��������		   : �����˿ڳ�ʼ������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void key_init()	   //
{
	GPIO_InitTypeDef GPIO_InitStructure;	  //����һ���ṹ�������������ʼ��GPIO

	SystemInit();
	/* ����GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	   //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//��������  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	   //���ô�������
	GPIO_Init(GPIOB,&GPIO_InitStructure);		  /* ��ʼ��GPIO */
	
	/*  ����GPIO��ģʽ��IO�� */
}

u8 Key_Scan()
{
	u8 key;
	if(Key1==0)
	{
		delay_ms(2);
		if(Key1==0)
		{
			key=1;
			while(Key1==0);
		}
	}
	else if(Key2==0)
	{
		delay_ms(2);
		if(Key2==0)
		{
			key= 2;
			while(Key2==0);
		}
	}
	 else if(Key3==0)
	{
		delay_ms(2);
		if(Key3==0)
		{
			key= 3;
			while(Key3==0);
		}
	}
	else if(Key4==0)
	{
		delay_ms(2);
		if(Key4==0)
		{
			key= 4;
			while(Key4==0);
		}
	}
	return key;
}
