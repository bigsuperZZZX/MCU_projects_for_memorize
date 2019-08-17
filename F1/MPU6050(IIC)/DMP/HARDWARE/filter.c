#include "filter.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
float K1 =0.1;   //�����˲����ٶȼ�Ȩ��
float angle, angle_dot; 	//��
float Q_angle=0.01;// ����������Э����   //��
float Q_gyro=0.0003;//0.003 ����������Э���� ����������Э����Ϊһ��һ�����о���  //��
float R_angle=0.01;// ����������Э���� �Ȳ���ƫ��   //��
float dt=0.005;//        ����ʱ��       ���õ�   
char  C_0 = 1;//��
float Q_bias, Angle_err;//��
float PCt_0, PCt_1, E;//��
float K_0, K_1, t_0, t_1;//��
float Pdot[4] ={0,0,0,0};//��
float PP[2][2] = { { 1, 0 },{ 0, 1 } }; //��
float x1,x2,y1;  //���׻����˲�
float angle_x,angle_y,angle_z;

/**************************************************************************
�������ܣ����׿������˲�
��ڲ��������ٶȡ����ٶ�
����  ֵ����
**************************************************************************/
void Kalman_Filter(float Accel,float Gyro)		
{
	angle+=(Gyro - Q_bias) * dt; //�������
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-����������Э�����΢��

	Pdot[1]=-PP[1][1];
	Pdot[2]=-PP[1][1];
	Pdot[3]=Q_gyro;
	PP[0][0] += Pdot[0] * dt;   // Pk-����������Э����΢�ֵĻ���
	PP[0][1] += Pdot[1] * dt;   // =����������Э����
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
		
	Angle_err = Accel - angle;	//zk-�������
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //����������Э����
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	angle	+= K_0 * Angle_err;	 //�������
	Q_bias	+= K_1 * Angle_err;	 //�������
	angle_dot   = Gyro - Q_bias;	 //���ֵ(�������)��΢��=���ٶ�
}

/**************************************************************************
�������ܣ�һ�׻����˲�
��ڲ��������ٶȡ����ٶ�
����  ֵ����
**************************************************************************/
void Yijielvbo(float angle_x_t,float angle_y_t,float angle_z_t, float gyro_x_t,float gyro_y_t,float gyro_z_t)
{
  angle_x = K1 * angle_x_t+ (1-K1) * (angle_x + gyro_x_t * dt);
	angle_y = K1 * angle_y_t+ (1-K1) * (angle_y + gyro_y_t * dt);
	angle_z = K1 * angle_z_t+ (1-K1) * (angle_z + gyro_z_t * dt);
}


//���׻����˲�

void Erjielvbo(float angle_m,float gyro_m)//�ɼ������ĽǶȺͽǼ��ٶ�
{
    x1=(angle_m-angle_x)*(1-K1)*(1-K1);
    y1=y1+x1*dt;
    x2=y1+2*(1-K1)*(angle_m-angle_x)+gyro_m;
    angle_x=angle_x+ x2*dt;
}
	

