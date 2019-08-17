#ifndef __FILTER_H
#define __FILTER_H
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
extern float angle, angle_dot ,angle_x,angle_y,angle_z; 	
void Kalman_Filter(float Accel,float Gyro);		
void Yijielvbo(float angle_x_t,float angle_y_t,float angle_z_t, float gyro_x_t,float gyro_y_t,float gyro_z_t);
void Erjielvbo(float angle_m, float gyro_m);
#endif
