#include "delay.h"
#include "math.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "music.h"
#include "stm32f10x_tim.h"
#include "music.h"
#include "oled.h"



#define uchar unsigned char

signed short FLAG;
signed char SIN_TAB[128] = { 0, 6, 12, 18, 24, 30, 36, 42, 48, 54, 59, 65, 70, 75, 80, 85, 89, 94, 98, 102, 

105, 108, 112, 114, 117, 119, 121, 123, 124, 125, 126, 126, 126, 126, 126, 125, 124, 123, 121, 119, 117, 114, 112, 

108, 105, 102, 98, 94, 89, 85, 80, 75, 70, 65, 59, 54, 48, 42, 36, 30, 24, 18, 12, 6, 0, -6, -12, -18, -24, -30, 

-36, -42, -48, -54, -59, -65, -70, -75, -80, -85, -89, -94, -98, -102, -105, -108, -112, -114, -117, -119, -121, 

-123, -124, -125, -126, -126, -126, -126, -126, -125, -124, -123, -121, -119, -117, -114, -112, -108, -105, -102, 

-98, -94, -89, -85, -80, -75, -70, -65, -59, -54, -48, -42, -36, -30, -24, -18, -12, -6 };

signed char COS_TAB[128] = { 127, 126, 126, 125, 124, 123, 121, 119, 117, 114, 112, 108, 105, 102, 98, 94, 

89, 85, 80, 75, 70, 65, 59, 54, 48, 42, 36, 30, 24, 18, 12, 6, 0, -6, -12, -18, -24, -30, -36, -42, -48, -54, -59, 

-65, -70, -75, -80, -85, -89, -94, -98, -102, -105, -108, -112, -114, -117, -119, -121, -123, -124, -125, -126, -

126, -126, -126, -126, -125, -124, -123, -121, -119, -117, -114, -112, -108, -105, -102, -98, -94, -89, -85, -80, 

-75, -70, -65, -59, -54, -48, -42, -36, -30, -24, -18, -12, -6, 0, 6, 12, 18, 24, 30, 36, 42, 48, 54, 59, 65, 70, 

75, 80, 85, 89, 94, 98, 102, 105, 108, 112, 114, 117, 119, 121, 123, 124, 125, 126, 126 };

signed char LIST_TAB[128] = { 0, 64, 32, 96, 16, 80, 48, 112,
8, 72, 40, 104, 24, 88, 56, 120,
4, 68, 36, 100, 20, 84, 52, 116,
12, 76, 44, 108, 28, 92, 60, 124,
2, 66, 34, 98, 18, 82, 50, 114,
10, 74, 42, 106, 26, 90, 58, 122,
6, 70, 38, 102, 22, 86, 54, 118,
14, 78, 46, 110, 30, 94, 62, 126,
1, 65, 33, 97, 17, 81, 49, 113,
9, 73, 41, 105, 25, 89, 57, 121,
5, 69, 37, 101, 21, 85, 53, 117,
13, 77, 45, 109, 29, 93, 61, 125,
3, 67, 35, 99, 19, 83, 51, 115,
11, 75, 43, 107, 27, 91, 59, 123,
7, 71, 39, 103, 23, 87, 55, 119,
15, 79, 47, 111, 31, 95, 63, 127
};

unsigned char i,j,k,b,p;
int Fft_Real[128]={0XFF}; 
int Fft_Image[128]={0XFF};               
uchar LED_TAB2[64]={0XFF}; 				
uchar LED_TAB[64]={0XFF}; 			
uchar LED_TAB1[64]={0XFF}; 				
unsigned int TEMP1;
uchar G_value=0;
uchar j_value=0;
uchar fractional_frequency=16;
int Temp_Real,Temp_Imag,temp; 
int Height[16];


void TIM3_IRQHandler()	 //��ʱ��3�жϺ���
{
	u16 i;
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	for(i=0;i<128;i++)
	{
		PAout(0)=0;
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//ת��������־λ
		Fft_Real[LIST_TAB[i]] = (int)( ADC_GetConversionValue(ADC1)*10 );//�������һ�� ADCx �������ת�����
		PAout(0)=1;
	}
	for(i=0;i<128;i++)
  {
    Fft_Image[i] = 0;   //ת���������
  }
	FFT();
	for(i=1;i<19;i++)
	{
		light_show(i,0);
	}
	for(i=0;i<7;i++)
  {
    Height[i] = ( Fft_Image[i*8+61]+Fft_Image[i*8+62]+Fft_Image[i*8+63]+Fft_Image[i*8+64]+Fft_Image[i*8+65]+Fft_Image[i*8+66]+Fft_Image[i*8+67] )/3;
		if(Height[i]>63)
			Height[i]=63;
		if(Height[i]<0)
			Height[i]=0;
		if(i!=0)
		{
			if(Height[i]<4)
			{
				light_show(i*3-2,1);
			}
			else if(Height[i]<11)
			{
				light_show(i*3-2,1);
				light_show(i*3-1,1);
			}
			else if(Height[i]<21)
			{
				light_show(i*3-1,1);
			}
			else if(Height[i]<32)
			{
				light_show(i*3-1,1);
				light_show(i*3,1);
			}
			else if(Height[i]<42)
			{
				light_show(i*3,1);
			}
			else if(Height[i]<55)
			{
				light_show(i*3-2,1);
				light_show(i*3,1);
			}
			else
			{
				light_show(i*3-2,1);
				light_show(i*3-1,1);
				light_show(i*3,1);
			}
		}
		
//		OLED_pillars(i,Height[i]);
  }
}

//�����Ŷ��壬����������
//	A0 A2 A3 | A6 A8 A9 | A10 A11 A12 | A13 A14 A15 | B2 B3 B4 | B5 B6 B7

void light_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��A�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_6|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��A�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void light_show(char led,char act)
{
	if(act==1)
	{
		switch(led)
		{
			case 1:L11_ON();break;
			case 2:L12_ON();break;
			case 3:L13_ON();break;
			case 4:L21_ON();break;
			case 5:L22_ON();break;
			case 6:L23_ON();break;
			case 7:L31_ON();break;
			case 8:L32_ON();break;
			case 9:L33_ON();break;
			case 10:L41_ON();break;
			case 11:L42_ON();break;
			case 12:L43_ON();break;
			case 13:L51_ON();break;
			case 14:L52_ON();break;
			case 15:L53_ON();break;
			case 16:L61_ON();break;
			case 17:L62_ON();break;
			case 18:L63_ON();break;
		}
	}
	if(act==0)
	{
		switch(led)
		{
			case 1:L11_OFF();break;
			case 2:L12_OFF();break;
			case 3:L13_OFF();break;
			case 4:L21_OFF();break;
			case 5:L22_OFF();break;
			case 6:L23_OFF();break;
			case 7:L31_OFF();break;
			case 8:L32_OFF();break;
			case 9:L33_OFF();break;
			case 10:L41_OFF();break;
			case 11:L42_OFF();break;
			case 12:L43_OFF();break;
			case 13:L51_OFF();break;
			case 14:L52_OFF();break;
			case 15:L53_OFF();break;
			case 16:L61_OFF();break;
			case 17:L62_OFF();break;
			case 18:L63_OFF();break;
		}
	}
}

void FFT(void)
{               
    for( i=1; i<=7; i++)                            /* for(1) */
    { 
        b=1;
        b <<=(i-1);                                     
        for( j=0; j<=b-1; j++)                              /* for (2) */
        { 
            p=1;
            p <<= (7-i);            
            p = p*j;
            for( k=j; k<128; k=k+2*b)                /* for (3) ?��?tfft */
            { 
                Temp_Real = Fft_Real[k]; 
				Temp_Imag = Fft_Image[k]; 
				temp = Fft_Real[k+b];
                Fft_Real[k] = Fft_Real[k] + ((Fft_Real[k+b]*COS_TAB[p])>>7) + ((Fft_Image[k+b]*SIN_TAB[p])>>7);
                Fft_Image[k] = Fft_Image[k] - ((Fft_Real[k+b]*SIN_TAB[p])>>7) + ((Fft_Image[k+b]*COS_TAB[p])>>7);
                Fft_Real[k+b] = Temp_Real - ((Fft_Real[k+b]*COS_TAB[p])>>7) - ((Fft_Image[k+b]*SIN_TAB[p])>>7);
                Fft_Image[k+b] = Temp_Imag + ((temp*SIN_TAB[p])>>7) - ((Fft_Image[k+b]*COS_TAB[p])>>7);     
                              
              	Fft_Real[k] >>= 1;             
                Fft_Image[k] >>= 1; 
               	Fft_Real[k+b]  >>= 1;                 
                Fft_Image[k+b]  >>= 1;                                                              
            }     
        } 
    } 
	Fft_Real[0]=Fft_Image[0]=0;		 
}


void adc_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO|RCC_APB2Periph_ADC1,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//12M ��� 14M ���� ADC ʱ�ӣ� ADCCLK��
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;//ADC
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN; //ģ������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	//����ָ�� ADC �Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_71Cycles5);
	ADC_Cmd(ADC1,ENABLE);
	ADC_ResetCalibration(ADC1);//����ָ���� ADC ��У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//��ȡ ADC ����У׼�Ĵ�����״̬
		ADC_StartCalibration(ADC1);//��ʼָ�� ADC ��У׼״̬
	while(ADC_GetCalibrationStatus(ADC1));//��ȡָ�� ADC ��У׼����
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ�ܻ���ʧ��ָ���� ADC �����ת����������
}

void time_init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	 //����һ���ṹ�������������ʼ��GPIO

	NVIC_InitTypeDef NVIC_InitStructure;

	/* ������ʱ��3ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//���TIMx���жϴ�����λ:TIM �ж�Դ
	TIM_TimeBaseInitStructure.TIM_Period = 2000;//�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 3599;//����������ΪTIMxʱ��Ƶ��Ԥ��Ƶֵ��100Khz����Ƶ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);	
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܻ���ʧ��TIMx����
	/* �����жϲ����������ж� */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );	//ʹ�ܻ���ʧ��ָ����TIM�ж�
	
	/* ����NVIC���� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��TIM3_IRQn��ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;	//��ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;  //��Ӧ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ��
	NVIC_Init(&NVIC_InitStructure);	
}

