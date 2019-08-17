#include "control.h"	
#include "filter.h"	
#include "oled.h"	
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
u8 Flag_Target;
float sum_error=0;
float bal_kp=170, bal_ki=0, bal_kd=0.2, zhongzhi=0;
signed char count=0;
signed int l_pulse,r_pulse;
extern long pulse_r,pulse_l;
extern char dir_r,dir_l;   //0��ǰ��1�Ǻ�  �ҵ�������ж��ߣ��½��ؽ��룬����Ϊ����Ϊǰ��Ϊ����Ϊ�� // �� �� �� �� �� ǰ �� ��
extern	int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
extern float Pitch,Roll; 
/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
         5ms��ʱ�ж���MPU6050��INT���Ŵ���
         �ϸ�֤���������ݴ����ʱ��ͬ��				 
**************************************************************************/
/*int EXTI9_5_IRQHandler(void) 
{    
	if(PBin(5)==0)		
	{   
		  EXTI->PR=1<<5;                                                      //���LINE5�ϵ��жϱ�־λ   
			Get_Angle(Way_Angle);
		  if(count == 0)
			{
	//			OLED_ShowFloat(0,0,Pitch,6,16);
				OLED_ShowNum(80,0,pulse_r,6,16);
			}
			if(count == 1)
			{				//===������̬	
				Balance_Pwm =balance(Angle_Balance,Gyro_Balance);                   //===ƽ��PID����	
	//		  Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);                  //===�ٶȻ�PID����	 ��ס���ٶȷ�����������������С�����ʱ��Ҫ����������Ҫ���ܿ�һ��
	//  	  Turn_Pwm    =turn(Encoder_Left,Encoder_Right,Gyro_Turn);            //===ת��PID����     
				Moto1=Balance_Pwm+Velocity_Pwm-Turn_Pwm;                            //===�������ֵ������PWM
				Moto2=Balance_Pwm+Velocity_Pwm+Turn_Pwm;                            //===�������ֵ������PWM
				Xianfu_Pwm();                                                       //===PWM�޷�
//				if(Pick_Up(Acceleration_Z,Angle_Balance,Encoder_Left,Encoder_Right))//===����Ƿ�С��������
//				Flag_Stop=1;	                                                      //===���������͹رյ��
//				if(Put_Down(Angle_Balance,Encoder_Left,Encoder_Right))              //===����Ƿ�С��������
//				Flag_Stop=0;	                                                      //===��������¾��������
				Turn_Off(Angle_Balance,Voltage);                              //===����������쳣
//				Set_Pwm(Moto1,Moto2);                                               //===��ֵ��PWM�Ĵ���	
			}
		  if(count == 2)
			{
	//			OLED_ShowFloat(0,2,bal_kp,6,16);
				OLED_ShowNum(80,2,pulse_l,6,16);
			}
		  if(count == 3)
			{
	//			OLED_ShowFloat(0,4,bal_ki,6,16);
				OLED_ShowNum(80,4,dir_r,6,16);
			}
		  if(count == 4)
			{
	//			OLED_ShowFloat(0,6,bal_kd,6,16);
				OLED_ShowNum(80,6,dir_l,6,16);
				count=-1;
			}				
	}
  count++;	
	 return 0;	  
} */

int EXTI9_5_IRQHandler(void) 
{    
	if(PBin(5)==0)		
	{   
		  EXTI->PR=1<<5;                                                      //���LINE5�ϵ��жϱ�־λ   
			Get_Angle(Way_Angle);
		  count++;
	}
	 return 0;	  
}

void read_pulse(void)
{
	
}
	

/**************************************************************************
�������ܣ�ֱ��PD����
��ڲ������Ƕȡ����ٶ�
����  ֵ��ֱ������PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int balance(float Angle,float Gyro)
{  
   float Bias;
	 int balance;
	 Bias=Angle-zhongzhi;       //===���ƽ��ĽǶ���ֵ �ͻ�е���
	 sum_error += Bias;
	 balance=bal_kp*Bias+ Gyro*bal_kd+ bal_ki*sum_error;   //===����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
	 return balance;
}

/**************************************************************************
�������ܣ��ٶ�PI���� �޸�ǰ������ң���ٶȣ�����Target_Velocity�����磬�ĳ�60�ͱȽ�����
��ڲ��������ֱ����������ֱ�����
����  ֵ���ٶȿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int velocity(int encoder_left,int encoder_right)
{  
    static float Velocity,Encoder_Least,Encoder,Movement;
	  static float Encoder_Integral,Target_Velocity=130;
	  float kp=100,ki=kp/200;
	  //=============ң��ǰ�����˲���=======================// 
		if(1==Flag_Qian)    	Movement=-Target_Velocity;	         //===ǰ����־λ��1 
		else if(1==Flag_Hou)	Movement=Target_Velocity;         //===���˱�־λ��1
	  else  Movement=0;	
	  if(Bi_zhang==1&&Distance<500&&Flag_Left!=1&&Flag_Right!=1)        //���ϱ�־λ��1�ҷ�ң��ת���ʱ�򣬽������ģʽ
	  Movement=Target_Velocity;
   //=============�ٶ�PI������=======================//	
		Encoder_Least =(encoder_left+encoder_right)-0;                    //===��ȡ�����ٶ�ƫ��==�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ�㣩 
//		Encoder *= 0.7;		                                                //===һ�׵�ͨ�˲���       
//		Encoder += Encoder_Least*0.3;	                                    //===һ�׵�ͨ�˲���    
		Encoder_Integral +=Encoder_Least;                                       //===���ֳ�λ�� ����ʱ�䣺10ms
		Encoder_Integral=Encoder_Integral-Movement;                       //===����ң�������ݣ�����ǰ������
		if(Encoder_Integral>100)  	Encoder_Integral=100;             //===�����޷�
		if(Encoder_Integral<-100)	Encoder_Integral=-100;              //===�����޷�
//		OLED_ShowFloat(0,2,Encoder_Integral,8,16);
		Velocity=Encoder_Least*kp+Encoder_Integral*ki;                          //===�ٶȿ���	
		if(Turn_Off(Angle_Balance,Voltage)==1||Flag_Stop==1)   Encoder_Integral=0;      //===����رպ��������
	  return Velocity;
}

/**************************************************************************
�������ܣ�ת�����  �޸�ת���ٶȣ����޸�Turn_Amplitude����
��ڲ��������ֱ����������ֱ�������Z��������
����  ֵ��ת�����PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int turn(int encoder_left,int encoder_right,float gyro)//ת�����
{
    static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.7,Turn_Count,Kp=42,Kd=0;
	  float Turn_Amplitude=50;    
	  //=============ң��������ת����=======================//
  	if(1==Flag_Left||1==Flag_Right)                      //��һ������Ҫ�Ǹ�����תǰ���ٶȵ����ٶȵ���ʼ�ٶȣ�����С������Ӧ��
		{
			if(++Turn_Count==1)
			Encoder_temp=myabs(encoder_left+encoder_right);
			Turn_Convert=50/Encoder_temp;
			if(Turn_Convert<0.4)Turn_Convert=0.4;
			if(Turn_Convert>1)Turn_Convert=1;
		}	
	  else
		{
			Turn_Convert=0.7;
			Turn_Count=0;
			Encoder_temp=0;
		}		
		if(1==Flag_Left)	           Turn_Target-=Turn_Convert;        //===����ת��ң������
		else if(1==Flag_Right)	     Turn_Target+=Turn_Convert;        //===����ת��ң������
		else Turn_Target=0;                                            //===����ת��ң������
    if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===ת���ٶ��޷�
	  if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;   //===ת���ٶ��޷�
		if(Flag_Qian==1||Flag_Hou==1)  Kd=0.6;                         //===����ת��ң������ֱ�����ߵ�ʱ�����������Ǿ;���    
		else Kd=0;                                   
  	//=============ת��PD������=======================//
		Turn=Turn_Target*Kp+gyro*Kd;                 //===���Z�������ǽ���PD����
	  return Turn;
}
/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int moto1,int moto2)
{
	    int siqu=400;
			if(moto1<0)
			{
				TIM_SetCompare1(TIM2,-moto1+siqu);
				TIM_SetCompare2(TIM2,0);
			}	
			else
			{
				TIM_SetCompare1(TIM2,0);
				TIM_SetCompare2(TIM2,moto1+siqu);
			}			
		  if(moto2<0)
			{
				TIM_SetCompare3(TIM2,-moto2+siqu);
				TIM_SetCompare4(TIM2,0);
			}	
			else
			{
				TIM_SetCompare3(TIM2,0);
				TIM_SetCompare4(TIM2,moto2+siqu);
			}		
}


/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=2500;    //===PWM������7200 ������6900
    if(Moto1<-Amplitude) Moto1=-Amplitude;	
		if(Moto1>Amplitude)  Moto1=Amplitude;	
	  if(Moto2<-Amplitude) Moto2=-Amplitude;	
		if(Moto2>Amplitude)  Moto2=Amplitude;		
	
}
/**************************************************************************
�������ܣ������޸�С������״̬ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Key(void)
{	
	u8 tmp,tmp2;
	tmp=click_N_Double(50); 
	if(tmp==1)Flag_Stop=!Flag_Stop;//��������С������ͣ
	if(tmp==2)Flag_Show=!Flag_Show;//˫������С������ʾ״̬
	tmp2=Long_Press();                   
  if(tmp2==1) Bi_zhang=!Bi_zhang;		//��������С���Ƿ���볬��������ģʽ 
}

/**************************************************************************
�������ܣ��쳣�رյ��
��ڲ�������Ǻ͵�ѹ
����  ֵ��1���쳣  0������
**************************************************************************/
u8 Turn_Off(float angle, int voltage)
{
			if(angle<-25||angle>25||1==Flag_Stop)//��ص�ѹ����11.1V�رյ��
			{	                                                 //===��Ǵ���40�ȹرյ��
				Moto1 = Moto2 =0;
      }
      return 0;			
}
	
/**************************************************************************
�������ܣ���ȡ�Ƕ� �����㷨�������ǵĵ�У�����ǳ����� 
��ڲ�������ȡ�Ƕȵ��㷨 1��DMP  2�������� 3�������˲�
����  ֵ����
**************************************************************************/
void Get_Angle(u8 way)
{ 	
	    if(way==1)                           //===DMP�Ķ�ȡ�����ݲɼ��ж����ѵ�ʱ���ϸ���ѭʱ��Ҫ��
			{	
					Read_DMP();                      //===��ȡ���ٶȡ����ٶȡ����
					Angle_Balance=Pitch;             //===����ƽ�����
					Gyro_Balance=gyro[1];            //===����ƽ����ٶ�
					Gyro_Turn=gyro[2];               //===����ת����ٶ�
				  Acceleration_Z=accel[2];         //===����Z����ٶȼ�
			}		
}
/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}
/**************************************************************************
�������ܣ����С���Ƿ�����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int Pick_Up(float Acceleration,float Angle,int encoder_left,int encoder_right)
{ 		   
	 static u16 flag,count0,count1,count2;
	if(flag==0)                                                                   //��һ��
	 {
	      if(myabs(encoder_left)+myabs(encoder_right)<30)                         //����1��С���ӽ���ֹ
				count0++;
        else 
        count0=0;		
        if(count0>10)				
		    flag=1,count0=0; 
	 } 
	 if(flag==1)                                                                  //����ڶ���
	 {
		    if(++count1>200)       count1=0,flag=0;                                 //��ʱ���ٵȴ�2000ms
	      if(Acceleration>26000&&(Angle>(-20+ZHONGZHI))&&(Angle<(20+ZHONGZHI)))   //����2��С������0�ȸ���������
		    flag=2; 
	 } 
	 if(flag==2)                                                                  //������
	 {
		  if(++count2>100)       count2=0,flag=0;                                   //��ʱ���ٵȴ�1000ms
	    if(myabs(encoder_left+encoder_right)>200)                                 //����3��С������̥��Ϊ�������ﵽ����ת��   
      {
				flag=0;                                                                                     
				return 1;                                                               //��⵽С��������
			}
	 }
	return 0;
}
/**************************************************************************
�������ܣ����С���Ƿ񱻷���
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int Put_Down(float Angle,int encoder_left,int encoder_right)
{ 		   
	 static u16 flag,count;	 
	 if(Flag_Stop==0)                           //��ֹ���      
   return 0;	                 
	 if(flag==0)                                               
	 {
	      if(Angle>(-10+ZHONGZHI)&&Angle<(10+ZHONGZHI)&&encoder_left==0&&encoder_right==0)         //����1��С������0�ȸ�����
		    flag=1; 
	 } 
	 if(flag==1)                                               
	 {
		  if(++count>50)                                          //��ʱ���ٵȴ� 500ms
		  {
				count=0;flag=0;
		  }
	    if(encoder_left<-3&&encoder_right<-3&&encoder_left>-30&&encoder_right>-30)                //����2��С������̥��δ�ϵ��ʱ����Ϊת��  
      {
				flag=0;
				flag=0;
				return 1;                                             //��⵽С��������
			}
	 }
	return 0;
}


