#ifndef _motor_control_h
#define _motor_control_h

/*struct pid
{ 
	float SetSpeed;//定义设定值
	float ActualSpeed;//定义实际值
	float err; //定义偏差值 
	float err_last;//定义上一个偏差值 
	float Kp,Ki,Kd;//定义比例、积分、微分系数
	float voltage;//定义电压值（控制执行器的变量）
	float integral;//定义积分值 
}moto_1,moto_2;*/

void moto_1_pid(void);
void moto_2_pid(void);

void PID_init(void);

void pwm_init(void);


#endif

