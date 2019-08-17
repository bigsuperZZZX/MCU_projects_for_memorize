#include "control.h"	
#include "usart.h"

int Balance_Pwm,Velocity_Pwm,Turn_Pwm;    
u8 Flag_Times;                            //进入中断的次数计数
float balance_kp=600,balance_kd=1.2, velocity_kp=130 ,turn_Kp=80,turn_Kd=0;                          
u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right; 
u8 Flag_Stop=0;                
int Encoder_Left,Encoder_Right;           
int Moto1,Moto2;                             
float Angle_Balance,Gyro_Balance,Gyro_Turn;       
float Acceleration_Z;
extern uint8_t Uart_Rx[UART_RX_LEN];        //串口

int EXTI9_5_IRQHandler(void) 
{
	 if(PBin(5)==0 && PAin(12)==0)	
	 {   
		  EXTI->PR=1<<5;                                                      //清除LINE5上的中断标志位   
		  Flag_Times=!Flag_Times;
		  if(Flag_Times==1)                                                 
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_11);
				show_state();
				Get_Angle();
				GPIO_ResetBits(GPIOA,GPIO_Pin_11);
				return 0;	                                               
			}
		  get_order();                                                                   
			Encoder_Left=-Read_Encoder(2);                                     
			Encoder_Right=Read_Encoder(4);                                 
	  	Get_Angle();
 			Balance_Pwm =balance(Angle_Balance,Gyro_Balance);  //平衡PID计算
		  Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);    //速度环PID计算
  	  Turn_Pwm    =turn(Encoder_Left,Encoder_Right,Gyro_Turn);   //转向环PID计算     
 		  Moto1=Balance_Pwm-Velocity_Pwm-Turn_Pwm;                        
 	  	Moto2=Balance_Pwm-Velocity_Pwm+Turn_Pwm;                         
   		Xianfu_Pwm();                                         
      if(Turn_Off(Angle_Balance)==0){}       //如果存在异常，PWM直接赋零                
			Set_Pwm(Moto1,Moto2);
			
	  }
		else Set_Pwm(0,0); 
		
	 return 0;	  
} 

void get_order(void)    //解算发来的信号
{
	if(Uart_Rx[0]&0x01) Flag_Qian=0, Flag_Hou=0;
	if(Uart_Rx[0]&0x04) Flag_Qian=0, Flag_Hou=1;
	if(Uart_Rx[0]&0x02) Flag_Qian=1, Flag_Hou=0;
	if(Uart_Rx[0]&0x10) Flag_Left=0, Flag_Right=0;
	if(Uart_Rx[0]&0x20) Flag_Left=0, Flag_Right=1;
	if(Uart_Rx[0]&0x40) Flag_Left=1, Flag_Right=0;
}

void show_state(void)
{
	static signed char times=0;
	if(times==0 && Flag_Qian==1)
	{
		OLED_ShowCHinese(16,4,1);
		OLED_ShowCHinese(16,0,5);
		OLED_ShowCHinese(16,2,5);
	}
	if(times==1 && Flag_Hou==1)
	{
		OLED_ShowCHinese(16,0,0);
		OLED_ShowCHinese(16,4,5);
		OLED_ShowCHinese(16,2,5);
	}
	if(times==2 && Flag_Left==1)
	{
		OLED_ShowCHinese(0,2,2);
		OLED_ShowCHinese(32,2,5);
		OLED_ShowCHinese(16,2,5);
	}
	if(times==3 && Flag_Right==1)
	{
		OLED_ShowCHinese(32,2,3);
		OLED_ShowCHinese(0,2,5);
		OLED_ShowCHinese(16,2,5);
	}
	if(times==4 && Flag_Qian==0 && Flag_Hou==0 && Flag_Left==0 && Flag_Right==0)
	{
		OLED_ShowCHinese(16,4,5);
		OLED_ShowCHinese(16,0,5);
	}
	if(times==5 && Flag_Qian==0 && Flag_Hou==0 && Flag_Left==0 && Flag_Right==0)
	{
		OLED_ShowCHinese(32,2,5);
		OLED_ShowCHinese(0,2,5);
	}
	if(times==6 && Flag_Qian==0 && Flag_Hou==0 && Flag_Left==0 && Flag_Right==0)
	{
		OLED_ShowCHinese(16,2,4);
	}
	if(times==7)
	{
		OLED_ShowFloat(48,6,Angle_Balance,6,16);
	}
	times++;
	if(times==8) times=0;
}

int balance(float Angle,float Gyro)
{  
   float Bias;
	 int balance;
	 Bias=Angle-ZHONGZHI;      
	 balance=balance_kp*Bias+Gyro*balance_kd; 
	 return balance;
}

int velocity(int encoder_left,int encoder_right)
{  
    static float Velocity,Encoder_Least,Encoder,Movement;
	  static float Encoder_Integral,Target_Velocity=100;
	  float ki=velocity_kp/200;

		if(1==Flag_Qian)    	Movement=-Target_Velocity;	      
		else if(1==Flag_Hou)	Movement=Target_Velocity;     
	  else  Movement=0;	
  
		Encoder_Least =(Encoder_Left+Encoder_Right)-0;   //滤波                 
		Encoder *= 0.7;		                                                    
		Encoder += Encoder_Least*0.3;	                                   
		Encoder_Integral +=Encoder;                                     
		Encoder_Integral=Encoder_Integral-Movement;                     
		if(Encoder_Integral>10000)  	Encoder_Integral=10000;            
		if(Encoder_Integral<-10000)	Encoder_Integral=-10000; 
		Velocity=Encoder*velocity_kp+Encoder_Integral*ki;           //PID计算           
		if(Movement == 0) 
		{
			if(Velocity > 4000) Velocity = 4000 ;
			if(Velocity <-4000) Velocity =-4000 ;
		}
		if(Turn_Off(Angle_Balance)==1||Flag_Stop==1)   Encoder_Integral=0;      
	  return Velocity;
}


int turn(int encoder_left,int encoder_right,float gyro)
{
    static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.7,Turn_Count;
	  float Turn_Amplitude=10;    
	 
  	if(1==Flag_Left||1==Flag_Right)                     
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
		
		if(1==Flag_Left)	           Turn_Target-=Turn_Convert;       
		else if(1==Flag_Right)	     Turn_Target+=Turn_Convert;    
		else Turn_Target= 80/turn_Kp*(encoder_left- encoder_right);	
    if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;   
	  if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;  
		if(Flag_Qian==1||Flag_Hou==1)  turn_Kd=0.6;                       
		else turn_Kd=0;                                   
  	
		Turn=Turn_Target*turn_Kp+gyro*turn_Kd;  
	  return Turn;
}

void Set_Pwm(int moto1,int moto2)
{
	    int siqu=400;
			if(moto1>0)
			{
				TIM_SetCompare1(TIM3,moto1+siqu);
				TIM_SetCompare2(TIM3,0);
			}
			else
			{
				TIM_SetCompare1(TIM3,0);
				TIM_SetCompare2(TIM3,-moto1+siqu);
			}
			if(moto2>0)
			{
				TIM_SetCompare3(TIM3,moto2+siqu);
				TIM_SetCompare4(TIM3,0);
			}
			else
			{
				TIM_SetCompare3(TIM3,0);
				TIM_SetCompare4(TIM3,-moto2+siqu);
			}
}



void Xianfu_Pwm(void)
{	
	  int Amplitude=6900;    //===PWM满幅是7200 限制在6900
    if(Moto1<-Amplitude) Moto1=-Amplitude;	
		if(Moto1>Amplitude)  Moto1=Amplitude;	
	  if(Moto2<-Amplitude) Moto2=-Amplitude;	
		if(Moto2>Amplitude)  Moto2=Amplitude;		
	
}


u8 Turn_Off(float angle)
{
			if(angle<-30||angle>30||Flag_Stop!=0)
			{	                                   
				Moto1 = Moto2 =0;
				return 1;
      }
			else
      return 0;			
}
	
void Get_Angle()
{ 
	Read_DMP();                      //===读取加速度、角速度、倾角
	Angle_Balance=Pitch;             //===更新平衡倾角
	Gyro_Balance=gyro[1];            //===更新平衡角速度
	Gyro_Turn=gyro[2];               //===更新转向角速度
	Acceleration_Z=accel[2];         //===更新Z轴加速度计
}

int myabs(int a)
{ 		   
		if(a<0)  return -a;  
	  else return a;
}


