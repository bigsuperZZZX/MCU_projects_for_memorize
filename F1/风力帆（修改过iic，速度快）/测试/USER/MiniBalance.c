#include "stm32f10x.h"
#include "sys.h"
#include "oled.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
u8 Way_Angle=1;                             //��ȡ�Ƕȵ��㷨��1����Ԫ��  2��������  3�������˲� 
u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right; //����ң����صı���
u8 Flag_Stop=1,Flag_Show=0;                 //ֹͣ��־λ�� ��ʾ��־λ Ĭ��ֹͣ ��ʾ��
int Encoder_Left,Encoder_Right;             //���ұ��������������
int Moto1,Moto2;                            //���PWM���� Ӧ��Motor�� ��Moto�¾�	
int Temperature;                            //��ʾ�¶�
int Voltage;                                //��ص�ѹ������صı���
float Angle_Balance,Gyro_Balance,Gyro_Turn; //ƽ����� ƽ�������� ת��������
float Show_Data_Mb;                         //ȫ����ʾ������������ʾ��Ҫ�鿴������
u32 Distance;                               //���������
u8 delay_50,delay_flag,Bi_zhang=0;         //Ĭ������£����������Ϲ��ܣ������û�����2s���Ͽ��Խ������ģʽ
float Acceleration_Z;                      //Z����ٶȼ�  
int main(void)
  { 
		u8 temp[1]={0};
		u8 state;
		delay_init();	    	            //=====��ʱ������ʼ��	
		uart_init(128000);	            //=====���ڳ�ʼ��Ϊ
    OLED_Init();                    //=====OLED��ʼ��	
		delay_ms(10);
//		JTAG_Set(JTAG_SWD_DISABLE);     //=====�ر�JTAG�ӿ�
//		JTAG_Set(SWD_ENABLE);           //=====��SWD�ӿ� �������������SWD�ӿڵ���
//		LED_Init();                     //=====��ʼ���� LED ���ӵ�Ӳ���ӿ�
//	  KEY_Init();                     //=====������ʼ��
//    MiniBalance_PWM_Init(7199,0);   //=====��ʼ��PWM 10KHZ������������� �����ʼ������ӿ� 
//		uart3_init(9600);               //=====����3��ʼ��
//    Encoder_Init_TIM2();            //=====�������ӿ�
//    Encoder_Init_TIM4();            //=====��ʼ��������2
//		Adc_Init();                     //=====adc��ʼ��
    IIC_Init();                     //=====IIC��ʼ��
		delay_ms(500);
    MPU6050_initialize();           //=====MPU6050��ʼ��	
		
   state=i2cRead(0x68,0x75,1,temp);
    DMP_Init();                     //=====��ʼ��DMP     
//		TIM3_Cap_Init(0XFFFF,72-1);	    //=====��������ʼ��
	  MiniBalance_EXTI_Init();        //=====MPU6050 5ms��ʱ�жϳ�ʼ��
    while(1)
	   {
			 
	
	//	    if(Flag_Show==0)          //ʹ��MiniBalanceV3.5 APP��OLED��ʾ��
	//				{
	//						APP_Show();	
	//						oled_show();          //===��ʾ����
	//				}
	//				else                      //ʹ��MiniBalanceV3.5��λ�� ��λ��ʹ�õ�ʱ����Ҫ�ϸ��ʱ�򣬹ʴ�ʱ�ر�app��ز��ֺ�OLED��ʾ��
	//				{
	//			      DataScope();          //����MiniBalanceV3.5��λ��
	//				}	
	//			  delay_flag=1;	
	//				delay_50=0;
	//				while(delay_flag);	     //ͨ��MPU6050��INT�ж�ʵ�ֵ�50ms��׼��ʱ	
	   } 
}

