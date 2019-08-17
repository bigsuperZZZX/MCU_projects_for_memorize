#include "restorecontrol.h"
#include "delay.h"  

#define	DELAY_LONG	0
#define	DELAY_SHORT	1

#define	ARM_ANGLE		1
#define	ARM_FOBA 		2

#define	SERVOCENTER	(int8_t)130
#define CW					(int8_t)80
#define	AW					(int8_t)-80
#define FORWARD			(int8_t)-30
#define BACK				(int8_t)30
#define CENTER			(int8_t)0

extern short  MoFang[7][10];

extern short Restore_step[200];


/*========================================

魔方是人的视角
舵机是 上下左右安装，分别是F\B\L\R

=========================================*/


u8 ov_sta;	

static void CubeLeft(int8_t dir);
static void CubeRight(int8_t dir);
static void ServoDelay(u8	mode);
static void ServoTimerInit(void);

static u32				Servo_Timer=0;
static int8_t			AngleBuff[4] = {0xFF,0,0,0xFE};
//====================================================

static short Abs(short x)
{
    if(x > 0)
        return x;
    else
        return -x;
}
/*=================================================
与舵机驱动板通讯
发送数据帧格式
每帧长度 4
帧头	0XFF
第一个数据	舵机号
第二个数据	高电平时间	t x 0.01ms;	如 130 = 1.3ms
帧尾	0XFE

===================================================*/
static void SendServoAngle(int8_t *tx,u8 size)
{
	u8 i;
	for(i=0;i<size;i++)
	{
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
			USART1->DR = (u8)tx[i];
	}
}


static void FrontServo(int8_t dir,int8_t adj)
{	
	switch(dir)
	{	
		case	ARM_ANGLE:																	//发送的角度数据，给舵机0
			AngleBuff[1] = 0;
			AngleBuff[2] = SERVOCENTER+adj;
			break;
		case	ARM_FOBA:
			AngleBuff[1] = 1;																//发送的手臂伸缩数据,给舵机1
			AngleBuff[2] = SERVOCENTER+adj;
			break;
	}
	SendServoAngle(AngleBuff,4);
}

static void BackServo(int8_t dir,int8_t adj)
{
	switch(dir)
	{	
		case	ARM_ANGLE:
			AngleBuff[1] = 2;
			AngleBuff[2] = SERVOCENTER+adj;
			break;
		case	ARM_FOBA:
			AngleBuff[1] = 3;
			AngleBuff[2] = SERVOCENTER+adj;
			break;
	}
	SendServoAngle(AngleBuff,4);
}

static void LeftServo(int8_t dir,int8_t adj)
{
	switch(dir)
	{	
		case	ARM_ANGLE:
			AngleBuff[1] = 4;
			AngleBuff[2] = SERVOCENTER+adj;
			break;
		case	ARM_FOBA:
			AngleBuff[1] = 5;
			AngleBuff[2] = SERVOCENTER+adj;
			break;
	}
	SendServoAngle(AngleBuff,4);
}

static void RightServo(int8_t dir,int8_t adj)
{
	switch(dir)
	{	
		case	ARM_ANGLE:
			AngleBuff[1] = 6;
			AngleBuff[2] = SERVOCENTER+adj;
			break;
		case	ARM_FOBA:
			AngleBuff[1] = 7;
			AngleBuff[2] = SERVOCENTER+adj;
			break;
	}
	SendServoAngle(AngleBuff,4);
}

//===============================================

static void CubeFront(int8_t dir)  //
{	
	if(dir > 0)
	{
		LeftServo(ARM_FOBA,BACK);
		RightServo(ARM_FOBA,BACK);																		
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_ANGLE,CW);   //有用
		BackServo(ARM_ANGLE,AW+3);											
		ServoDelay(DELAY_LONG);
		
		RightServo(ARM_FOBA,FORWARD);
		LeftServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_ANGLE,CENTER);
		FrontServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_FOBA,FORWARD);
		BackServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		if(dir==2)
		{
			LeftServo(ARM_ANGLE,CW);
			ServoDelay(DELAY_LONG);
			
			LeftServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
			
			LeftServo(ARM_ANGLE,CENTER);
			ServoDelay(DELAY_SHORT);
			
			LeftServo(ARM_FOBA,FORWARD);
			ServoDelay(DELAY_LONG);
			
			LeftServo(ARM_ANGLE,CW);
			ServoDelay(DELAY_LONG);
			
			LeftServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
			
			LeftServo(ARM_ANGLE,CENTER);
			ServoDelay(DELAY_SHORT);
		}
		else
		{
			LeftServo(ARM_ANGLE,CW);
			ServoDelay(DELAY_LONG);
			
			LeftServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
			
			LeftServo(ARM_ANGLE,CENTER);
			ServoDelay(DELAY_SHORT);
		}
		
		RightServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_ANGLE,AW+3);
		BackServo(ARM_ANGLE,CW);
		ServoDelay(DELAY_LONG);
		
		RightServo(ARM_FOBA,FORWARD);
		LeftServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_ANGLE,CENTER);
		FrontServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_FOBA,FORWARD);
		BackServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
	}
	else
	{
		LeftServo(ARM_FOBA,BACK);
		RightServo(ARM_FOBA,BACK);																		
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_ANGLE,CW);
		BackServo(ARM_ANGLE,AW+3);											
		ServoDelay(DELAY_LONG);
		
		RightServo(ARM_FOBA,FORWARD);
		LeftServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_ANGLE,CENTER);
		FrontServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_FOBA,FORWARD);
		BackServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		if(dir==2)
		{
			LeftServo(ARM_ANGLE,AW+2);
			ServoDelay(DELAY_LONG);
			
			LeftServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
			
			LeftServo(ARM_ANGLE,CENTER);
			ServoDelay(DELAY_SHORT);
			
			LeftServo(ARM_FOBA,FORWARD);
			ServoDelay(DELAY_LONG);
			
			LeftServo(ARM_ANGLE,AW+2);
			ServoDelay(DELAY_LONG);
			
			LeftServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
			
			LeftServo(ARM_ANGLE,CENTER);
			ServoDelay(DELAY_SHORT);
		}
		else
		{
			LeftServo(ARM_ANGLE,AW+2);
			ServoDelay(DELAY_LONG);
			
			LeftServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
			
			LeftServo(ARM_ANGLE,CENTER);
			ServoDelay(DELAY_SHORT);
		}
		
		RightServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_ANGLE,AW+3);
		BackServo(ARM_ANGLE,CW);
		ServoDelay(DELAY_LONG);
		
		RightServo(ARM_FOBA,FORWARD);
		LeftServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_ANGLE,CENTER);
		FrontServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_FOBA,FORWARD);
		BackServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
	}
}

static void CubeBack(int8_t dir) //
{	
	if(dir > 0)
	{
		LeftServo(ARM_FOBA,BACK);
		RightServo(ARM_FOBA,BACK);																		
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_ANGLE,CW);
		BackServo(ARM_ANGLE,AW+3);											
		ServoDelay(DELAY_LONG);
		
		RightServo(ARM_FOBA,FORWARD);
		LeftServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_ANGLE,CENTER);
		FrontServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_FOBA,FORWARD);
		BackServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		if(dir==2)
		{
			RightServo(ARM_ANGLE,CW+2);
			ServoDelay(DELAY_LONG);
			
			RightServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
			
			RightServo(ARM_ANGLE,CENTER-2);
			ServoDelay(DELAY_SHORT);
			
			RightServo(ARM_FOBA,FORWARD);
			ServoDelay(DELAY_LONG);
			
			RightServo(ARM_ANGLE,CW+2);
			ServoDelay(DELAY_LONG);
			
			RightServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
			
			RightServo(ARM_ANGLE,CENTER-2);
			ServoDelay(DELAY_SHORT);
		}
		else
		{
			RightServo(ARM_ANGLE,CW+2);
			ServoDelay(DELAY_LONG);
			
			RightServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
			
			RightServo(ARM_ANGLE,CENTER-2);
			ServoDelay(DELAY_SHORT);
		}
		
		LeftServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_ANGLE,AW+3);
		BackServo(ARM_ANGLE,CW);
		ServoDelay(DELAY_LONG);
		
		RightServo(ARM_FOBA,FORWARD);
		LeftServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_ANGLE,CENTER);
		FrontServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_FOBA,FORWARD);
		BackServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
	}
	else
	{
		LeftServo(ARM_FOBA,BACK);
		RightServo(ARM_FOBA,BACK);																		
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_ANGLE,CW);
		BackServo(ARM_ANGLE,AW+3);											
		ServoDelay(DELAY_LONG);
		
		RightServo(ARM_FOBA,FORWARD);
		LeftServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_ANGLE,CENTER);
		FrontServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_FOBA,FORWARD);
		BackServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		if(dir==2)
		{
			RightServo(ARM_ANGLE,AW+2);
			ServoDelay(DELAY_LONG);
			
			RightServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
			
			RightServo(ARM_ANGLE,CENTER-2);
			ServoDelay(DELAY_SHORT);
			
			RightServo(ARM_FOBA,FORWARD);
			ServoDelay(DELAY_LONG);
			
			RightServo(ARM_ANGLE,AW+2);
			ServoDelay(DELAY_LONG);
			
			RightServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
			
			RightServo(ARM_ANGLE,CENTER-2);
			ServoDelay(DELAY_SHORT);
		}
		else
		{
			RightServo(ARM_ANGLE,AW+2);
			ServoDelay(DELAY_LONG);
			
			RightServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
			
			RightServo(ARM_ANGLE,CENTER-2);
			ServoDelay(DELAY_SHORT);
		}
		
		LeftServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_ANGLE,AW+3);
		BackServo(ARM_ANGLE,CW);
		ServoDelay(DELAY_LONG);
		
		RightServo(ARM_FOBA,FORWARD);
		LeftServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_ANGLE,CENTER);
		FrontServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_FOBA,FORWARD);
		BackServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
	}
}

static void CubeLeft(int8_t dir)  //
{
	int8_t i = Abs(dir);
	for(;i--;)
	if(dir > 0)
	{
			LeftServo(ARM_ANGLE,CW+12);
			ServoDelay(DELAY_LONG);
		
			LeftServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
		
			LeftServo(ARM_ANGLE,CENTER);
			ServoDelay(DELAY_SHORT);
		
			LeftServo(ARM_FOBA,FORWARD);
			ServoDelay(DELAY_LONG);
	}
	else
	{
			LeftServo(ARM_ANGLE,AW+2);
			ServoDelay(DELAY_LONG);
		
			LeftServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_SHORT);
		
			LeftServo(ARM_ANGLE,CENTER);
			ServoDelay(DELAY_SHORT);
		
			LeftServo(ARM_FOBA,FORWARD);
			ServoDelay(DELAY_LONG);
	}
}

static void CubeRight(int8_t dir)  //
{
	int8_t i = Abs(dir);
	for(;i--;)
	if(dir > 0)
	{
		RightServo(ARM_ANGLE,CW+8);
		ServoDelay(DELAY_LONG);
		
		RightServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		RightServo(ARM_ANGLE,CENTER-2);
		ServoDelay(DELAY_SHORT);
		
		RightServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
	}
	else
	{
		RightServo(ARM_ANGLE,AW+2);
		ServoDelay(DELAY_LONG);
		
		RightServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		RightServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		RightServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
	}
}

static void CubeUp(int8_t dir)      //
{
	int8_t i = Abs(dir);
	for(;i--;)
	if(dir > 0)
	{
		FrontServo(ARM_ANGLE,CW+10);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
	}
	else
	{
		FrontServo(ARM_ANGLE,AW);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
	}
}

static void CubeDown(int8_t dir)   //
{
	u8 i = Abs(dir);
	for(;i--;)
	if(dir > 0)
	{
		BackServo(ARM_ANGLE,CW+4);
		ServoDelay(DELAY_LONG);
		
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_ANGLE,CENTER-13);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
	}
	else
	{
		BackServo(ARM_ANGLE,AW-2);
		ServoDelay(DELAY_LONG);
		
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
	}
}

static void CubeZ(int8_t dir)   //整个魔方+U1
{
	int8_t i = Abs(dir);
	for(;i--;)
	if(dir > 0)
	{
		LeftServo(ARM_FOBA,BACK);
		RightServo(ARM_FOBA,BACK);																		
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_ANGLE,CW);
		BackServo(ARM_ANGLE,AW+3);											
		ServoDelay(DELAY_LONG);
		
		RightServo(ARM_FOBA,FORWARD);
		LeftServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_ANGLE,CENTER);
		FrontServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_FOBA,FORWARD);
		BackServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
	}
	else
	{
		LeftServo(ARM_FOBA,BACK);
		RightServo(ARM_FOBA,BACK);																		
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_ANGLE,AW+3);
		BackServo(ARM_ANGLE,CW);											
		ServoDelay(DELAY_LONG);
		
		RightServo(ARM_FOBA,FORWARD);
		LeftServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_SHORT);
		
		BackServo(ARM_ANGLE,CENTER);
		FrontServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_SHORT);
		
		FrontServo(ARM_FOBA,FORWARD);
		BackServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
	}
}

void CubePrepare(void)
{
	LeftServo(ARM_ANGLE,CENTER);
	RightServo(ARM_ANGLE,CENTER);
	ServoDelay(DELAY_LONG);
	
	FrontServo(ARM_ANGLE,CENTER);
	BackServo(ARM_ANGLE,CENTER);
	
	LeftServo(ARM_FOBA,BACK);
	RightServo(ARM_FOBA,BACK);
	ServoDelay(DELAY_LONG);
	
	FrontServo(ARM_FOBA,BACK);
	BackServo(ARM_FOBA,BACK);
	ServoDelay(DELAY_LONG);
}

static void CubeF_R_B_L(void)   //
{	
	LeftServo(ARM_FOBA,FORWARD);
	RightServo(ARM_FOBA,FORWARD);
	ServoDelay(DELAY_LONG);
	
	FrontServo(ARM_FOBA,BACK);
	BackServo(ARM_FOBA,BACK);
	ServoDelay(DELAY_LONG);
	
	FrontServo(ARM_ANGLE,CENTER);
	BackServo(ARM_ANGLE,CENTER);
	ServoDelay(DELAY_LONG);
	
	FrontServo(ARM_FOBA,FORWARD);
	BackServo(ARM_FOBA,FORWARD);
	ServoDelay(DELAY_LONG);
	
	LeftServo(ARM_FOBA,BACK);
	RightServo(ARM_FOBA,BACK);
	ServoDelay(DELAY_LONG);
	
	FrontServo(ARM_ANGLE,CW);     //有用
	BackServo(ARM_ANGLE,AW+3);    //有用     
	ServoDelay(DELAY_LONG);
	
}

static void CubeD_U(void)     //
{
	static u8	cunt=0;
	cunt++;
	
	if(cunt == 1)
	{
		LeftServo(ARM_FOBA,FORWARD);
		RightServo(ARM_FOBA,FORWARD);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_FOBA,BACK);
		BackServo(ARM_FOBA,BACK);
		ServoDelay(DELAY_LONG);
		
		FrontServo(ARM_ANGLE,CENTER);
		BackServo(ARM_ANGLE,CENTER);
		ServoDelay(DELAY_LONG);
		
		LeftServo(ARM_ANGLE,AW+5);
		RightServo(ARM_ANGLE,CW);  
		ServoDelay(DELAY_LONG);
	}
	else
	{
		if(cunt == 2)
		{
			LeftServo(ARM_ANGLE,CENTER);
			RightServo(ARM_ANGLE,CENTER);
			ServoDelay(DELAY_LONG);
			
			FrontServo(ARM_FOBA,FORWARD);
			BackServo(ARM_FOBA,FORWARD);
			ServoDelay(DELAY_LONG);
			
			FrontServo(ARM_FOBA,BACK);
			BackServo(ARM_FOBA,BACK);
			ServoDelay(DELAY_LONG);
			
			LeftServo(ARM_ANGLE,CW);
			RightServo(ARM_ANGLE,AW+6);
			ServoDelay(DELAY_LONG);
				
		}
		else
		{		
			LeftServo(ARM_ANGLE,CENTER);
			RightServo(ARM_ANGLE,CENTER);
			ServoDelay(DELAY_LONG);
			
			FrontServo(ARM_FOBA,FORWARD);
			BackServo(ARM_FOBA,FORWARD);
			ServoDelay(DELAY_LONG);
		}
	}	
}

static void OV7670Read_LCDRefresh(void)
{
	
}

static void ServoDelay(u8	mode)
{
	Servo_Timer = 0;
	if(mode == DELAY_LONG)
	{
		while(Servo_Timer < 600)
		{
			if(ov_sta >= 2)
				OV7670Read_LCDRefresh();
		}
	}
	else	if(mode == DELAY_SHORT)
	{
		while(Servo_Timer < 400)
		{
			if(ov_sta >= 2)
				OV7670Read_LCDRefresh();
		}
	}
}


//=============================================

static void RestoreStepResolve(int16_t *step)
{
	u8 	i;
	
	for(i=0;step[i]!='#';i++)
	{
		switch(Abs(step[i]/10))
		{
		case	1:	CubeFront(step[i]%10)	;	break;    //
		case	2:	CubeBack(step[i]%10)	;	break;    //
		case	3:	CubeLeft(step[i]%10)	;	break;		//
		case	4:	CubeRight(step[i]%10)	;	break;		//
		case	5:	CubeUp(step[i]%10)		;	break;    //
		case	6:	CubeDown(step[i]%10)	;	break;		//
		case	7:	CubeZ(step[i]%10)			;	break;
		case	8:	CubeF_R_B_L()					;	break;
		case	9:	CubeD_U();						;	break;
		}	
	}
}

void ColorRecogniton(void)
{
	u8 	i=0,m,n;
	int8_t	color[7][10]={0};
														 	//1    2    3     4   5    6
	       //enum  {Resever = 0,Front,Back,Left,Right,Up,Down};      //枚举类型 表示魔方的六个面
	                            //F        L        B         R         D          F
	int16_t	sz[8][3]={{'#','#'},{81,'#'},{81,'#'},{81,'#'},{81,'#'},{81,91,'#'},{91,'#'},{91,'#'}};

//	TargetProperty_TypeDef	Framecondition={{{0,0,0},{100,20,46}},{0,0,240,320},4};
/*	TargetProperty_TypeDef	Condition[COLOR_NUM-1]={
	{{{340,40,40},{25,88,88}},{0,0,240,320},4},											//红
	{{{29,40,40},{45,88,88}},{0,0,240,320},4},											//橙
	{{{41,50,50},{75,80,80}},{0,0,240,320},4},											//黄
	{{{80,16,40},{125,80,80}},{0,0,240,320},4},										//绿
	{{{0,0,45},{320,10,80}},{0,0,240,320},4},											//蓝
	{{{0,25,75},{360,75,100}},{0,0,240,320},4}												//白
	};*/
	
	ServoTimerInit();
	
Retry:
	CubePrepare();  //舵机位置归中
	while(1)
	{
		while(ov_sta < 2);
		OV7670Read_LCDRefresh();
		if(/*Trace(Condition,COLOR_NUM-1,&Framecondition,color[1]) == 1*/ 1)
		{
			if(i < 1)
				i++;
			else
			{
				for(m=1;m<10;m++)	
					MoFang[1][m]=(short)color[1][m];
				break;
			}
		}
	}
	
	for(n=2;n<7;n++)
	{
 		RestoreStepResolve(sz[n]);
		for(;;)
		{
			while(ov_sta < 2);
			OV7670Read_LCDRefresh();
			if(/*Trace(Condition,COLOR_NUM-1,&Framecondition,color[1]) == 1*/ 1)
			{
				break;
			}
		}
													//1    2    3     4   5    6
	  //enum  {Resever = 0,Front,Back,Left,Right,Up,Down};      //枚举类型 表示魔方的六个面
												//绿    蓝    橙   红   白  黄
		switch(n)
		{																																//F
		case 2:for(m=1;m<10;m++)	MoFang[4][m]=(short)color[n][m];break;//R
		case 3:for(m=1;m<10;m++)	MoFang[2][m]=(short)color[n][m];break;//B
		case 4:for(m=1;m<10;m++)	MoFang[3][m]=(short)color[n][m];break;//L
		case 5:for(m=1;m<10;m++)	MoFang[6][m]=(short)color[n][m];break;//D
		case 6:for(m=1;m<10;m++)	MoFang[5][m]=(short)color[n][m];break;//U
		}		
	}
	RestoreStepResolve(sz[n]);
	//至此读取完全部颜色
	
	if(RestoreCube() == 0)   //步骤求解
 		goto Retry;
	else
	{
		RestoreStepResolve(Restore_step);
		LeftServo(ARM_FOBA,CENTER);
		RightServo(ARM_FOBA,CENTER);
		FrontServo(ARM_FOBA,CENTER);
		BackServo(ARM_FOBA,CENTER);
		ServoDelay(DELAY_LONG);
		while(1)
		{
			if(ov_sta >= 2)
				OV7670Read_LCDRefresh();
		}
	}
}

static void ServoTimerInit(void)
{
	NVIC_InitTypeDef TIM_InitStruct;
	
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	
	TIM_InitStruct.NVIC_IRQChannel = TIM7_IRQn;
	TIM_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	TIM_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	TIM_InitStruct.NVIC_IRQChannelSubPriority = 3;
	
	NVIC_Init(&TIM_InitStruct);
	
	TIM7->CR1 	= 0x0084;
	TIM7->DIER 	= 0x0001;
	TIM7->PSC		= 0x02D0;		//72M / 720 = 100KHz	
	TIM7->ARR		= 0x0064;		//重载值为100 = 1ms
	TIM7->CR1		|= 0x0001;
 	TIM7->EGR		= 0x0001;
}

void TIM7_IRQHandler(void)
{
	TIM7->SR = 0;
	Servo_Timer++;
}

