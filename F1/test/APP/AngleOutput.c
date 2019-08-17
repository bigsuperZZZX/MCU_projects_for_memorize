#include "AngleOutput.h"
#include <math.h>

float Angle_Coin(float angle_top,float angle_now)  		//angle_top是到达的最大角度，angle_now是当前角度, 均为偏离铅垂线的角度
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

float Angle_Laser(float angle_now) //angle_now是当前角度, 均为坐标系下角度，其中x轴向左，y轴向下
{
	angle_now/=57.3;
	return atan(L*(1-sin(angle_now))/(D-L*cos(angle_now)))*57.3;	
}
