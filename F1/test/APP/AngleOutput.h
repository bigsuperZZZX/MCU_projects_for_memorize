#ifndef _AngleOutput_h
#define _AngleOutput_h

#define L 100
#define D 150

float Angle_Coin(float angle_top,float angle_now) ; 		//angle_top是到达的最大角度，angle_now是当前角度, 均为偏离铅垂线的角度
float Swing_Tmie(float angle_top) ; //angle_top 是偏离垂线的角度
float Angle_Laser(float angle_now); //angle_now是当前角度, 均为坐标系下角度，其中x轴向左，y轴向下

#endif
