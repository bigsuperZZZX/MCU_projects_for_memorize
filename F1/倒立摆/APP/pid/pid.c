#include "pid.h"

u16 pid_flag=0;
struct Pid
{
	float sumerror;
	float kp;
	float ki;
	float kd;
	float e[3]; 
}pid;
void PID_INIT()
{
	pid.sumerror=0;
	pid.kp=80;
	pid.ki=0;
	pid.kd=250;
	pid.e[0]=0;
	pid.e[1]=0;
	pid.e[2]=0;
}

extern int Encoder_Left; 

float  Incremental_PID(float nowpoint,float Setpoint)
{
	
	float incpid;
	pid.e[0]=Setpoint-nowpoint;		
	
	incpid=pid.kp*pid.e[0]+pid.ki*pid.e[1]+pid.kd*pid.e[2];

	pid.e[2]=pid.e[1];
	pid.e[1]=pid.e[0];
	return incpid;
	
}

float  Posi_PID(float nowpoint,float Setpoint)
{
	 float pidout;
	 pid.e[0] = Setpoint - nowpoint;
	 pid.sumerror += pid.e[0];
	 if(Encoder_Left>30) Encoder_Left = 30;
   if(Encoder_Left<-30) Encoder_Left = -30;
	 pidout = pid.kp * pid.e[0] + pid.ki * pid.sumerror + pid.kd * (pid.e[0] - pid.e[1])  + (float)Encoder_Left * 15;
	 pid.e[1] = pid.e[0];
	 if(pidout>0) pidout += 80;  //加上死区
	 if(pidout<0) pidout -= 80;
	 if(pidout>400 ) pidout = 850; //限幅
	 if(pidout<-400) pidout = -850;
	 return pidout; 
}


int Moto_Reflash(int nowpoint)
{
	int pwm;
	pwm=nowpoint;
	if(pwm>500||pwm<-500)
		pwm=500;
	if(pwm<=0)
		pwm=-pwm;
	return pwm;
}

