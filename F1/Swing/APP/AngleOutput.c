#include "AngleOutput.h"
#include <math.h>

float Angle_Coin(float angle_top,float angle_now)  		//angle_top�ǵ�������Ƕȣ�angle_now�ǵ�ǰ�Ƕ�, ��Ϊƫ��Ǧ���ߵĽǶ�
{
	angle_top/=57.3;
	angle_now/=57.3;
	return (atan((2*(cos(angle_now)-cos(angle_top))*sin(angle_now))/(2*(cos(angle_now)-cos(angle_top))*cos(angle_now)+1)))*57.3;
}

float Swing_Tmie(float angle_top)
{
	angle_top/=57.3;
	return 2*3.1416*sqrt(L/(9.8*cos(angle_top/2)));
}

float Angle_Laser(float angle_now) //angle_now�ǵ�ǰ�Ƕ�, ��Ϊ����ϵ�½Ƕȣ�����x������y������
{
	angle_now/=57.3;
	return atan(L*(1-sin(angle_now))/(D-L*cos(angle_now)))*57.3;	
}
