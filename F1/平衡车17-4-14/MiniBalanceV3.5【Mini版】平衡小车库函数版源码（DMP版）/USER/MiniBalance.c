#include "stm32f10x.h"
#include "sys.h"
#include "control.h"	
#include "filter.h"	
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
extern long pulse_r,pulse_l;
extern char dir_r,dir_l;   //0��ǰ��1�Ǻ�  �ҵ�������ж��ߣ��½��ؽ��룬����Ϊ����Ϊǰ��Ϊ����Ϊ�� // �� �� �� �� �� ǰ �� ��
extern int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
extern u8 Flag_Target;
extern signed char count;
extern signed int l_pulse,r_pulse;
extern long pulse_r,pulse_l;
extern char dir_r,dir_l;   //0��ǰ��1�Ǻ�  �ҵ�������ж��ߣ��½��ؽ��룬����Ϊ����Ϊǰ��Ϊ����Ϊ�� // �� �� �� �� �� ǰ �� ��
extern	int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
extern float Pitch,Roll,zhongzhi; 
signed char count2=0;

void symbol_key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

int main(void)
  { 
		Flag_Stop =0 ;
		delay_init();	    	            //=====��ʱ������ʼ��	
		uart_init(128000);	            //=====���ڳ�ʼ��Ϊ
		delay_ms(1800);
    MiniBalance_Motor_Init();   //=====��ʼ��PWM 10KHZ������������� �����ʼ������ӿ� 
    encoder_ExtiInit();
    IIC_Init();                     //=====IIC��ʼ��
		delay_ms(200);    
		MPU6050_initialize();           //=====MPU6050��ʼ��	
    DMP_Init();                     //=====��ʼ��DMP 
    OLED_Init();                    //=====OLED��ʼ��	   
	  MiniBalance_EXTI_Init();        //=====MPU6050 5ms��ʱ�жϳ�ʼ��
		symbol_key_init();
    while(1)
	   {
			  if(count%6 == 5)
				{
					count = 0;
					GPIO_SetBits(GPIOB,GPIO_Pin_2);
//					zhongzhi += (float)(pulse_r+pulse_l)/50;
//					if(zhongzhi > 2)  zhongzhi=2;
//					if(zhongzhi < -2)  zhongzhi=-2;
					Balance_Pwm =balance(Angle_Balance,Gyro_Balance);                   //===ƽ��PID����	
	  		  Velocity_Pwm=velocity(pulse_l,pulse_r);                  //===�ٶȻ�PID����	 ��ס���ٶȷ�����������������С�����ʱ��Ҫ����������Ҫ���ܿ�һ��
	//  	  Turn_Pwm    =turn(Encoder_Left,Encoder_Right,Gyro_Turn);            //===ת��PID����
					Moto1=Balance_Pwm+Velocity_Pwm-Turn_Pwm;                            //===�������ֵ������PWM
					Moto2=Balance_Pwm+Velocity_Pwm+Turn_Pwm;                            //===�������ֵ������PWM
					Xianfu_Pwm();                                                       //===PWM�޷�
	//				if(Pick_Up(Acceleration_Z,Angle_Balance,Encoder_Left,Encoder_Right))//===����Ƿ�С��������
	//				if(Put_Down(Angle_Balance,Encoder_Left,Encoder_Right))              //===����Ƿ�С��������
					Turn_Off(Angle_Balance,Voltage);                              //===����������쳣
					Set_Pwm(Moto1,Moto2);					//===��ֵ��PWM�Ĵ���
					
					count2++;
					if(count2==4)
					{
						count2=0;
						pulse_r = pulse_l =0;	
					}
						
					if(count%20 == 0)
					{
						OLED_ShowFloat(0,0,Pitch,6,16);
						OLED_ShowFloat(0,2,zhongzhi,6,16);
						OLED_ShowNum(80,0,pulse_r,6,16);
						OLED_ShowNum(80,2,pulse_l,6,16);
//						OLED_ShowNum(80,4,dir_r,6,16);
//						OLED_ShowNum(80,6,dir_l,6,16);
					}
					GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				}
	   } 
}

