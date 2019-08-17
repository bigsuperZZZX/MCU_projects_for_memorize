#include "filter.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
float K1 =0.1;   //互补滤波加速度计权重
float angle, angle_dot; 	//卡
float Q_angle=0.01;// 过程噪声的协方差   //卡
float Q_gyro=0.0003;//0.003 过程噪声的协方差 过程噪声的协方差为一个一行两列矩阵  //卡
float R_angle=0.01;// 测量噪声的协方差 既测量偏差   //卡
float dt=0.005;//        采样时间       均用到   
char  C_0 = 1;//卡
float Q_bias, Angle_err;//卡
float PCt_0, PCt_1, E;//卡
float K_0, K_1, t_0, t_1;//卡
float Pdot[4] ={0,0,0,0};//卡
float PP[2][2] = { { 1, 0 },{ 0, 1 } }; //卡
float x1,x2,y1;  //二阶互补滤波
float angle_x,angle_y,angle_z;

/**************************************************************************
函数功能：简易卡尔曼滤波
入口参数：加速度、角速度
返回  值：无
**************************************************************************/
void Kalman_Filter(float Accel,float Gyro)		
{
	angle+=(Gyro - Q_bias) * dt; //先验估计
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-先验估计误差协方差的微分

	Pdot[1]=-PP[1][1];
	Pdot[2]=-PP[1][1];
	Pdot[3]=Q_gyro;
	PP[0][0] += Pdot[0] * dt;   // Pk-先验估计误差协方差微分的积分
	PP[0][1] += Pdot[1] * dt;   // =先验估计误差协方差
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
		
	Angle_err = Accel - angle;	//zk-先验估计
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //后验估计误差协方差
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	angle	+= K_0 * Angle_err;	 //后验估计
	Q_bias	+= K_1 * Angle_err;	 //后验估计
	angle_dot   = Gyro - Q_bias;	 //输出值(后验估计)的微分=角速度
}

/**************************************************************************
函数功能：一阶互补滤波
入口参数：加速度、角速度
返回  值：无
**************************************************************************/
void Yijielvbo(float angle_x_t,float angle_y_t,float angle_z_t, float gyro_x_t,float gyro_y_t,float gyro_z_t)
{
  angle_x = K1 * angle_x_t+ (1-K1) * (angle_x + gyro_x_t * dt);
	angle_y = K1 * angle_y_t+ (1-K1) * (angle_y + gyro_y_t * dt);
	angle_z = K1 * angle_z_t+ (1-K1) * (angle_z + gyro_z_t * dt);
}


//二阶互补滤波

void Erjielvbo(float angle_m,float gyro_m)//采集后计算的角度和角加速度
{
    x1=(angle_m-angle_x)*(1-K1)*(1-K1);
    y1=y1+x1*dt;
    x2=y1+2*(1-K1)*(angle_m-angle_x)+gyro_m;
    angle_x=angle_x+ x2*dt;
}
	

