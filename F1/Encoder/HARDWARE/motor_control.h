#ifndef _motor_control_h
#define _motor_control_h

/*struct pid
{ 
	float SetSpeed;//�����趨ֵ
	float ActualSpeed;//����ʵ��ֵ
	float err; //����ƫ��ֵ 
	float err_last;//������һ��ƫ��ֵ 
	float Kp,Ki,Kd;//������������֡�΢��ϵ��
	float voltage;//�����ѹֵ������ִ�����ı�����
	float integral;//�������ֵ 
}moto_1,moto_2;*/

void moto_1_pid(void);
void moto_2_pid(void);

void PID_init(void);

void pwm_init(void);


#endif

