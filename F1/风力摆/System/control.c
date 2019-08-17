#include "control.h"
#include "stm32f10x.h"
#include "moto.h"
#include "delay.h"
#include "math.h"

extern float set_x,set_y,cur_x,cur_y,set_r,set_a;
extern u8 set_n;

extern struct
{
	float SumError;
	float LastError;
	float Proportion;
	float Integral;
	float Derivative;	
}PID_X,PID_Y;

const Number_Struct Number_Table[10]=
{
	{{-6,6,6,6,-6,-6,-6,},{12,12,1,-12,-12,1,12}},
	{{1,1,1,1,1,1,1},{12,8,4,1,-4,-8,-12}},
	{{-6,-6,6,6,-6,-6,6},{12,12,12,1,1,-12,-12}},
	{{-8,-1,7,-1,7,-1,-8},{8,11,8,1,-8,-11,-8}},
	{{3,-3,-9,6,1,1,1},{12,8,1,1,1,-6,-12}},
	{{6,6,-6,-6,6,6,-6},{11,11,11,1,1,-11,-11}},
	{{6,-6,-6,-6,6,6,-6},{11,11,-1,-11,-11,-1,-1}},
	{{-6,-1,5,2,1,-1,-3},{8,8,8,1,-1,-5,-10}},
};

extern u8 sigh_ModeChange;

/**¶¨µã **************************/
void FixPoint(void)
{
	u16 high;
	if(set_x>0)
	{	
    TIM_SetCompare1(TIM2,0);		
		TIM_SetCompare3(TIM2,((high=cal_x())>2500 ? 2500:high));
	}
	else
	{
    TIM_SetCompare3(TIM2,0);	
		TIM_SetCompare1(TIM2,((high=cal_x())>2500 ? 2500:high));
	}
	if(set_y>0)
	{
    TIM_SetCompare2(TIM2,0);												
		TIM_SetCompare4(TIM2,((high=cal_y())>2500 ? 2500:high));
	}
	else
	{
    TIM_SetCompare4(TIM2,0);	
		TIM_SetCompare2(TIM2,((high=cal_y())>2500 ? 2500:high));
	}
/*	switch (set_quadrant)
	{
		case 1:{											//34
			TIM_SetCompare3(TIM2,((high=pid_cal_x())>2500 ? 2500:high));
			TIM_SetCompare4(TIM2,((high=pid_cal_y())>2500 ? 2500:high));
			break;}
		case 2:{											//14
			TIM_SetCompare1(TIM2,((high=pid_cal_x())>2500 ? 2500:high));
			TIM_SetCompare4(TIM2,((high=pid_cal_y())>2500 ? 2500:high));			
			break;}
		case 3:{											//12
			TIM_SetCompare1(TIM2,((high=pid_cal_x())>2500 ? 2500:high));
			TIM_SetCompare2(TIM2,((high=pid_cal_y())>2500 ? 2500:high));			
			break;}
		case 4:{											//23
			TIM_SetCompare3(TIM2,((high=pid_cal_x())>2500 ? 2500:high));
			TIM_SetCompare2(TIM2,((high=pid_cal_y())>2500 ? 2500:high));			
			break;}
		default: break;*/
	
}

void circle_PointMove(float r)
{
	static char i=0;
		PID_X.LastError=0;
		PID_X.SumError=0;
		PID_Y.LastError=0;
		PID_Y.SumError=0;
		set_x=r*cos(RAD_10*i*2);
		set_y=r*sin(RAD_10*i*2);
		i++;
		if(i==36)
			i=0;
}

void line_PointMove(void)
{
	static char j=0;
	if(fabs(cur_x-set_x)<2||fabs(cur_y-set_y)<2)
	{
		j++;
	}
	if(j==20)
	{
		set_x=(-set_x);
		set_y=(-set_y);
		PID_X.SumError=0;
		PID_X.LastError=0;
		PID_Y.SumError=0;
		PID_Y.LastError=0;
		j=0;
	}
}

void number_PointMove(void)
{
	static char k=0;
	 if(sigh_ModeChange==1)
		{
				sigh_ModeChange=0;
				k=0;
		}
	PID_X.SumError=0;
	PID_X.LastError=0;
	PID_Y.SumError=0;
	PID_Y.LastError=0;
	if(k!=7)	
	{
		set_x=Number_Table[set_n].x[k];
		set_y=Number_Table[set_n].y[k];
		k++;
	}	
	else
	{
		k=0;
	}
}


u16 base(float a)
{
	if(a>-0.75&&a<0.75)
	{return 80;}
	else if(a>-1.25&&a<1.25)
	{return 120;}
	else if(a>-1.75&&a<1.75)
	{return 160;}
	else if(a>-2.25&&a<2.25)
	{return 200;}
	else if(a>-2.75&&a<2.75)
	{return 225;}
	else if(a>-3.25&&a<3.25)
	{return 265;}
	else if(a>-3.75&&a<3.75)
	{return 295;}
	else if(a>-4.25&&a<4.25)
	{return 340;}
	else if(a>-4.75&&a<4.75)
	{return 380;}
	else if(a>-5.25&&a<5.25)
	{return 420;}
	else if(a>-5.75&&a<5.75)
	{return 450;}
	else if(a>-6.25&&a<6.25)
	{return 500;}
	else if(a>-6.75&&a<6.75)
	{return 515;}
	else if(a>-7.25&&a<7.25)
	{return 575;}
	else if(a>-7.75&&a<7.75)
	{return 625;}
	else if(a>-8.25&&a<8.25)
	{return 665;}
	else if(a>-8.75&&a<8.75)
	{return 700;}
	else if(a>-9.25&&a<9.25)
	{return 760;}
	else if(a>-9.75&&a<9.75)
	{return 810;}
	else if(a>-10.25&&a<10.25)
	{return 850;}
	else if(a>-10.75&&a<10.75)
	{return 890;}
	else if(a>-11.25&&a<11.25)
	{return 930;}
	else if(a>-11.75&&a<11.75)
	{return 980;}
	else if(a>-12.25&&a<12.25)
	{return 1010;}
	else if(a>-12.75&&a<12.75)
	{return 1050;}
	else if(a>-13.5&&a<13.5)
	{return 1110;}
	else if(a>-14.5&&a<14.5)
	{return 1230;}
	else if(a>-15.5&&a<15.5)
	{return 1320;}
	else if(a>-16.5&&a<16.5)
	{return 1440;}
	else if(a>-17.5&&a<17.5)
	{return 1560;}
	else if(a>-18.5&&a<18.5)
	{return 1710;}
	else if(a>-19.5&&a<19.5)
	{return 1840;}
	else if(a>-20.5&&a<20.5)
	{return 2000;}
	else{return 2100;}
	
}
