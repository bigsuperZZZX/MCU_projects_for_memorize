#include "beep.h"

/*******************************************************************************
* �� �� ��         : BEEP_Init
* ��������		   : �������˿ڳ�ʼ������	   ͨ���ı�Ƶ�ʿ��������仯
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void BEEP_Init()	  //�˿ڳ�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;	//����һ���ṹ�������������ʼ��GPIO

	SystemInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   /* ����GPIOʱ�� */

	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;		//ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;		  //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(GPIOB,&GPIO_InitStructure); 	 /* ��ʼ��GPIO */
}

/*******************************************************************************
* �� �� ��         : sound1
* ��������		   : ��������������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/



