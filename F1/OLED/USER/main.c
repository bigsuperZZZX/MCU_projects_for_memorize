//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//�о�԰����
//���̵�ַ��http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  �� �� ��   : main.c
//  �� �� ��   : v2.0
//  ��    ��   : HuangKai
//  ��������   : 2014-0101
//  ����޸�   : 
//  ��������   : OLED 4�ӿ���ʾ����(51ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND    ��Դ��
//              VCC  ��5V��3.3v��Դ
//              D0   ��PA5��SCL��
//              D1   ��PA7��SDA��
//              RES  ��PB0
//              DC   ��PB1
//              CS   ��PA4               
//              ----------------------------------------------------------------
// �޸���ʷ   :
// ��    ��   : 
// ��    ��   : HuangKai
// �޸�����   : �����ļ�
//��Ȩ���У�����ؾ���
//Copyright(C) �о�԰����2014/3/16
//All rights reserved
//******************************************************************************/


#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"
#include "stdlib.h"
 int main(void)
 {	
 u32 i,k=127;
	 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��A�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //��ʼ��GPIOD3,6
 	GPIO_ResetBits(GPIOB,GPIO_Pin_2);	
		delay_init();	    	 //��ʱ������ʼ��	  
		OLED_Init();			//��ʼ��OLED 
			 for(i=1;i<128;i++)
		 {
			 OLED_ShowPix(i,32-16*cos((float)i/8),'w');
			 delay_ms(10);
		 }
		 
	 while(k++)
	 {
		 OffSet_1();
		 OLED_ShowPix(127,rand()%128,'w');
		 delay_ms(10);
	 }
	/*for(i=0;i<10;i++)
	 {
	 OLED_ShowPix(i,5,1);
		delay_ms(500);
	 }
	 while(1);*/
}

