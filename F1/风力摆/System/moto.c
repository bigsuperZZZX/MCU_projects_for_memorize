#include "moto.h"
#include "stm32f10x.h"
#include "math.h"
#include "control.h"


extern float set_x,set_y,cur_x,cur_y;
extern unsigned char set_quadrant;


MotoTypeDef M1;
MotoTypeDef M2;
MotoTypeDef M3;
MotoTypeDef M4;

PidTypeDef PID_X;
PidTypeDef PID_Y;

void moto_init(void)
{
	M1.CurPos=M2.CurPos=M3.CurPos=M4.CurPos=0;
	M1.PrevPos=M2.PrevPos=M3.PrevPos=M4.PrevPos=0;
	M1.PWM=M2.PWM=M3.PWM=M4.PWM=0;
}

void pid_config(void)
{
	PID_X.SumError=0;
	PID_X.LastError=0;
	PID_X.Proportion=0;
	PID_X.Integral=1.5;
	PID_X.Derivative=2250;
	
	PID_Y.SumError=0;
	PID_Y.LastError=0;
	PID_Y.Proportion=0;
	PID_Y.Integral=1.5;
	PID_Y.Derivative=2250;
}

/********************************************************/


/*经典PID计算
u16 pid_cal_x(void)
{
    register float  iError,dError;

	iError = set_x - fabs(cur_x);        // 偏差
	PID_X.SumError += iError;				    // 积分
	if(PID_X.SumError > 10000.0)					//积分限幅2300
		PID_X.SumError = 10000.0;
	else if(PID_X.SumError < -10000.0)
		PID_X.SumError = -10000.0;	
	dError = iError - PID_X.LastError; 			// 当前微分
	PID_X.LastError = iError;
	
	return(u16)(  PID_X.Proportion * iError           	// 比例项
          		    + PID_X.Integral   * PID_X.SumError 		// 积分项
          		    + PID_X.Derivative * dError);
}

u16 pid_cal_y(void)
{
    register float  iError,dError;

	iError = set_y - fabs(cur_y);        // 偏差
	PID_Y.SumError += iError;				    // 积分
	if(PID_Y.SumError > 10000.0)					//积分限幅2300
		PID_Y.SumError = 10000.0;
	else if(PID_Y.SumError < -10000.0)
		PID_Y.SumError = -10000.0;	
	dError = iError - PID_Y.LastError; 			// 当前微分
	PID_Y.LastError = iError;
	
	return(u16)(  PID_Y.Proportion * iError           	// 比例项
          		    + PID_Y.Integral   * PID_Y.SumError 		// 积分项
          		    + PID_Y.Derivative * dError);
}
*************************************/

/*计算方式2*/
u16 cal_x(void)
{
	float  iError,dError;

	iError = set_x - cur_x;        // 偏差
	if(set_x<0)                   //二三象限
		iError=(-iError);	
	if(fabs(iError)<1.5)
	{
		PID_X.SumError += iError;				    // 积分
		/*if(PID_X.SumError > 10000.0)					//积分限幅1000
			PID_X.SumError = 10000.0;
		else if(PID_X.SumError < -10000.0)
			PID_X.SumError = -10000.0;	*/
	}
	dError = iError - PID_X.LastError; 			// 当前微分
	PID_X.LastError = iError;
	
	return(u16)(  base(set_x)
          		    + PID_X.Integral   * PID_X.SumError 		// 积分项
          		    + PID_X.Derivative * dError);
}

u16 cal_y(void)
{
	float  iError,dError;

	iError = set_y - cur_y;        // 偏差
	if(set_y<0)											//三四象限
		iError=(-iError);
	if(fabs(iError)<2)
	{
		PID_Y.SumError += iError;				    // 积分
		/*if(PID_Y.SumError > 10000.0)					//积分限幅1000
			PID_Y.SumError = 10000.0;
		else if(PID_Y.SumError < -10000.0)
			PID_Y.SumError = -10000.0;	*/
	}
	dError = iError - PID_Y.LastError; 			// 当前微分
	PID_Y.LastError = iError;
	
	return(u16)(  base(set_y)
          		    + PID_Y.Integral   * PID_Y.SumError 		// 积分项
          		    + PID_Y.Derivative * dError);
}


/*********************************************************/

