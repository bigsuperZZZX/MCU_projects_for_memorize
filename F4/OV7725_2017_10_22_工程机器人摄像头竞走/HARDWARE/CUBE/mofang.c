#include "mofang.h"
#include "delay.h"  

//循环左移和循环右移
#define ROL(x,s,n) (((x)<<(n)) | ((x)>>((s)-(n))))       //x被操作数   s该数位数 n要循环移位的位数
#define ROR(x,s,n) (((x)>>(n)) | ((x)<<((s)-(n))))

#define CW  1           //顺时针
#define AW  -1           //逆时针


extern short Restore_step[];                            //复原步骤记录
extern int  Step_sum ;                                  //步骤统计
extern short  MoFang[][10];                             //魔方颜色存储

enum  {Resever = 0,Front,Back,Left,Right,Up,Down};      //枚举类型 表示魔方的六个面

short RestoreCube(void);

/****************internal function statement***************/

static void F(short degree,short dir);                  //前转        degree 控制角度 1 =90° 2=180°  dir控制方向 顺时针和逆时针
static void B(short degree,short dir);                  //后转
static void L(short degree,short dir);                  //左
static void R(short degree,short dir);                  //右
static void U(short degree,short dir);                  //上
static void D(short degree,short dir);                  //下
static void Z(short degree,short dir);                  //绕Z轴旋转

static void DoStep(const char *step_str);               //执行所给步骤
static void FinishDownCross(void);                      //完成底面十字
static void FinishDownAngle(void);                      //完成底面四角
static void SecondFloor(void);                          //完成第二层
static void UpCross(void);                              //完成顶层十字
static void FinishUpColor(void);                        //完成顶层一色
static void FinishThirdFloor(void);                     //完成三层
static void simpleStep(void);                           //简化重复步骤
static void clean_71(void);															//把71指令（整个魔方+-U1/2的指令）去除掉
//short ColorCheck(void);													//检查颜色

/**********************************************************

//w=1,r=2,g=3,y=4,b=5,o=6

 F =  11  F2 =  12
-F = -11 -F2 = -12
 B =  21
-B = -21..........
 Z =  71
-Z = -71

***************extern variable statement*****************/
short Restore_step[200]={0};
short *pRestore = Restore_step;
int  Step_sum = 0;

/*short  MoFang[7][10] = {
0,0,0,0,0,0,0,0,0,0,
0,5,2,2,4,2,4,1,5,1,
0,6,2,4,5,6,5,3,3,3,
0,6,1,1,2,5,2,6,5,3,
0,4,3,3,6,3,4,5,3,4,
0,5,3,4,6,4,4,6,1,5,
0,2,1,2,6,1,6,1,1,2
};*/


													//1    2    3     4   5    6
	  //enum  {Resever = 0,Front,Back,Left,Right,Up,Down};      //枚举类型 表示魔方的六个面
												//绿    蓝    橙   红   白  黄
//    顺序1 4 2 3 6 5
			// 绿红 蓝橙黄白

short  MoFang[7][10] = {
0,0,0,0,0,0,0,0,0,0,      //
0,1,1,1,1,1,1,3,4,4,  		//F
0,6,6,6,6,2,6,6,6,6,   		//B
0,1,1,1,1,3,1,1,1,1,			//L
0,2,2,2,2,4,2,2,2,2,			//R
0,3,3,3,3,5,3,3,3,3,			//U
0,4,4,4,4,6,4,4,4,4			  //D
};


/*********************************************************/

/*********************************************************/
short RestoreCube(void)
{
	if(ColorCheck()==0)														//检查是否有某种颜色超过9个
		return 0;
	FinishDownCross();
	FinishDownAngle();
	SecondFloor();
	UpCross();
	FinishUpColor();
	FinishThirdFloor();
	simpleStep();
	clean_71();
	return 1;
}


/*********************************************************/
short ColorCheck(void)
{
	short temp[2][7]={0};
	short i,j,m;
	
	for(i=Front;i<=Down;i++)
	{
		for(j=1;j<10;j++)
		{
			for(m=Front;m<=Down;m++)
			{
				if(temp[0][m] == MoFang[i][j])
				{
					temp[1][m]++;
					break;
				}
				if(temp[0][m]==0)
				{
					temp[0][m]=MoFang[i][j];
					temp[1][m]++;
					break;
				}
			}
		}
	}
	for(i=Front;i<=Down;i++)
	{
		if(temp[1][i] != 9)
			return 0;
	}
	return 1;
}

/******step_str的组成：每三个字节为一个动作   如  “+F2”  表示前面顺时针旋转180°***********/

static void DoStep(const char *step_str)
{
    short dir;

    while(*step_str)
    {
        if(*(step_str++) == '+')
            dir = CW;
        else
            dir = AW;
        switch(*(step_str++))
        {
        case 'F':
            F(*step_str-0x30,dir);                     //此时*step_str指向的为字符型的数字 将其转为数字  如  ‘2’ - 0x30 = 2
            break;
        case 'B':
            B(*step_str-0x30,dir);
            break;
        case 'L':
            L(*step_str-0x30,dir);
            break;
        case 'R':
            R(*step_str-0x30,dir);
            break;
        case 'U':
            U(*step_str-0x30,dir);
            break;
        case 'D':
            D(*step_str-0x30,dir);
            break;
        case 'Z':
            Z(*step_str-0x30,dir);
            break;
        }
		step_str++;
    }
}

/*********************************************************/

/***************main function****************************/

static void FinishDownCross(void)
{
	short DownColor ;
	short FrontColor;
	short i,m;
	//与地面中心块对应的色块只可能在这些坐标出现,12与58对应  
	//可能12是白色58是红色，也可能58是白色12是红色所以又两个表
	//第三个是对应步骤，如找到的符合条件的色块在14,36，则对应‘+F1’
	const short SearchTable[12]={12,14,16,18,22,24,26,28,32,42,48,38};                          //12 表示前面第二块
	const short ComTable[12]   ={58,36,44,62,52,46,34,68,54,56,66,64};
	const char  *stepTable[12]  = {"","+F1","-F1","+F2","+U2","+B1+U2-B1",
																 "-B1+U2+B1","+B2+U2",
																 "-U1","+U1","+R2+U1","+L2-U1"};                            	//将符合规定的方块移动到 前面第2个位置

	DownColor =MoFang[Down][5];

	for(i=0;i<4;i++)
	{
		FrontColor=MoFang[Front][5];										//在+Z1后前面颜色更新

		if(MoFang[Down][2]!=DownColor || MoFang[Front][8]!=FrontColor)
		{
			for(m=0;m<12;m++)
			{
				if(MoFang[SearchTable[m]/10][SearchTable[m]%10] == DownColor)
				{
					if(MoFang[ComTable[m]/10][ComTable[m]%10] == FrontColor)
					{
						DoStep(stepTable[m]);
						if(MoFang[Front][2] != FrontColor)
								DoStep("-U1-R1+F1+R1");
						else
							DoStep("+F2");
						break;
					}
					continue;
				}
				if(MoFang[SearchTable[m]/10][SearchTable[m]%10] == FrontColor)
				{
					if(MoFang[ComTable[m]/10][ComTable[m]%10] == DownColor)
					{
						DoStep(stepTable[m]);
						if(MoFang[Front][2] != FrontColor)
								DoStep("-U1-R1+F1+R1");
						else
							DoStep("+F2");
						break;
					}
				}
			}
		}
		DoStep("+Z1");
	}
}


static void FinishDownAngle(void)
{
	short DownColor ;
	short FrontColor;
	short RightColor;
	short i,m;

	//脚块颜色有三种可能，所以有三个表
	
	const short SearchTable [8] = {11,13,19,21,23,27,29,17};
	const short SearchTable1[8] = {33,41,47,43,31,49,37,39};
	const short SearchTable2[8] = {57,59,63,53,51,69,67,61};
	const char  *stepTable  [8] = {"-U1","","+R1+U1-R1-U1",
																 "+U1","+U2","-R1+U1+R1+U1","+L1+U2-L1","-L1-U1+L1"};

	DownColor =MoFang[Down][5];

	for(i=0;i<4;i++)
	{
		FrontColor=MoFang[Front][5];
		RightColor=MoFang[Right][5];

		if(MoFang[Down][3] != DownColor || MoFang[Front][9] != FrontColor|| MoFang[Right][7] != RightColor)
		{
			for(m=0;m<8;m++)
			{
				if(MoFang[SearchTable[m]/10][SearchTable[m]%10] == DownColor ||
					 MoFang[SearchTable[m]/10][SearchTable[m]%10] == FrontColor||
					 MoFang[SearchTable[m]/10][SearchTable[m]%10] == RightColor   )
				{
					if(MoFang[SearchTable1[m]/10][SearchTable1[m]%10] == DownColor ||
						 MoFang[SearchTable1[m]/10][SearchTable1[m]%10] == FrontColor||
						 MoFang[SearchTable1[m]/10][SearchTable1[m]%10] == RightColor   )
					{
							if(MoFang[SearchTable2[m]/10][SearchTable2[m]%10] == DownColor ||
								 MoFang[SearchTable2[m]/10][SearchTable2[m]%10] == FrontColor||
								 MoFang[SearchTable2[m]/10][SearchTable2[m]%10] == RightColor   )
						 {
							 DoStep(stepTable[m]);
							 if(MoFang[Up][9] == DownColor)
								DoStep("-F1+U1+F1+R1+U2-R1");
							 else
							 {
								 if(MoFang[Front][3] == DownColor)
										DoStep("+U1+R1-U1-R1");
								 else
										DoStep("-U1-F1+U1+F1");
							 }
								break;
						 }
					}
				}
			}
		}
		DoStep("+Z1");                                      //换向
	}
}

static void SecondFloor(void)
{
	short FrontColor;
	short RightColor;
	short i,m;

	const short SearchTable1[8]={12,22,32,42,16,24,26,14};
	const short SearchTable2[8]={58,52,54,56,44,46,34,36};
	const char *stepTable[8]  ={"","+U2","-U1","+U1",
															 "+R1-U1-R1-U1-F1+U1+F1+U2",          //16
															 "-R1+U1+R1+U1+B1-U1-B1",             //24
															 "+L1-U1-L1-U1-B1+U1+B1",             //26
															 "-L1+U1+L1+U1+F1-U1-F1-U2"};         //14

	for(i=0;i<4;i++)
	{
		FrontColor=MoFang[Front][5];
		RightColor=MoFang[Right][5];

		if(MoFang[Front][6]!=FrontColor || MoFang[Right][4]!=RightColor)
		{
			for(m=0;m<8;m++)
			{
				if(MoFang[SearchTable1[m]/10][SearchTable1[m]%10] == FrontColor ||
					 MoFang[SearchTable1[m]/10][SearchTable1[m]%10] == RightColor   )
				{
					if(MoFang[SearchTable2[m]/10][SearchTable2[m]%10] == FrontColor ||
						 MoFang[SearchTable2[m]/10][SearchTable2[m]%10] == RightColor   )
					{
						 DoStep(stepTable[m]);
						 if(MoFang[Front][2]==FrontColor)
									DoStep("+U1+R1-U1-R1-U1-F1+U1+F1");
						 else
									DoStep("+U2-F1+U1+F1+U1+R1-U1-R1");
						 break;
					}
				}
			}
		}
		DoStep("+Z1");
	}
}

/*******************************************
        _____________
        | 0 | 1 | 2 |
        -------------
        | 3 | 8 | 4 |
        -------------
        | 5 | 6 | 7 |
        -------------

    当顶层为十字时 1、3、4、6与中心颜色一样
    用1表示与顶层颜色一样的色块 用0表示不一样的色块
    则顶层为十字时 1、3、4、6位必定为 1  则判断掩码值为0x5a = 01011010b;

********************************************/


static void UpCross(void)
{
	short UpColor = MoFang[Up][5];
	short UpValue ;
	short EigenValue = 0;
	short i;
	short temp;

	const short CrossMask = 0x5a;
	const short SearchTable[4] = {12,42,22,32};

	for(i=1,UpValue=0;i<10;i++)
	{
			if(i==5)
					continue;
			UpValue<<=1;
			if(MoFang[Up][i] == UpColor)
					UpValue|=1;
	}
	while((UpValue&CrossMask) != CrossMask)							//如果十字未完成
	{
		for(i=0,EigenValue=0;i<4;i++)
		{
			EigenValue<<=1;
			if(MoFang[SearchTable[i]/10][SearchTable[i]%10] == UpColor)
				EigenValue|=1;
		}

		for(i=0,temp=0;i<=3;i++)
		{
			temp = (ROL(EigenValue,4,i)&0x000f);
			if(temp == 0x0c || temp == 0x0a || temp == 0x0f)
				break;
		}
		switch(i)
		{
		case 1:DoStep("+U1");break;
		case 2:DoStep("+U2");break;
		case 3:DoStep("+U3");break;
		}

		DoStep("+F1+R1+U1-R1-U1-F1");
		for(i=1,UpValue=0;i<10;i++)
		{
				if(i==5)
						continue;
				UpValue<<=1;
				if(MoFang[Up][i] == UpColor)
						UpValue|=1;
		}
	}
}


static void FinishUpColor(void)
{
	short   UpColor = MoFang[Up][5];
	short   EigenValue = 0;
	short   i,m,index;
	short   NotFinish = 1;

	const short SearchTable[26] = {0x0049,0x0248,0x0241,0x0209,
																 0x0824,0x0124,0x0920,0x0904,
																 0x0108,0x0840,0x0204,0x0021,
																 0x0808,0x0044,0x0220,0x0101,
																 0x0028,0x0140,0x0a00,0x0005,
																 0x0a44,0x0225,0x0129,0x0948,
																 0x0a28,0x0145};


	while(NotFinish)
	{
		for(i=0,EigenValue=0;i<4;i++)
		{
			switch(i)
			{
					case 0:index=Front;break;
					case 1:index=Right;break;
					case 2:index=Back;break;
					case 3:index=Left;break;
			}
			for(m=1;m<4;m++)
			{
					EigenValue<<=1;
					if(MoFang[index][m]==UpColor)
							EigenValue|=1;
			}
		}
		if(!EigenValue)
		{
			NotFinish = 0;
			break;
		}
		for(i=0;i<26;i++)
		{
			if(EigenValue == SearchTable[i])
					break;
		}
		switch(i%4)
		{
		case 0:break;
		case 1:
				DoStep("-U1");
				break;
		case 2:
				DoStep("+U2");
				break;
		case 3:
				DoStep("+U1");
				break;
		}

		switch(i/4)
		{
		case 0:
				DoStep("-R1+U2+R1+U1-R1+U1+R1");
				NotFinish = 0;
				break;
		case 1:
				DoStep("-U1+R1+U2-R1-U1+R1-U1-R1");
				NotFinish = 0;
				break;
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
				DoStep("-R1+U2+R1+U1-R1+U1+R1");
				break;
		}
	}
}

static void FinishThirdFloor(void)
{
	short i,re_i;
	short EqualSide = 0;

	while(!EqualSide)
	{
		for(i=Front;i<=Right;i++)
		{
				if(MoFang[i][1] == MoFang[i][3])                    //判断有无同一边两边色块相等的情况
				{
						EqualSide++;
						re_i = i;                                       //记录相同的一边方向
						if(EqualSide >= 2)
						{
								EqualSide = 4;
								break;
						}
				}
		}
		if(EqualSide == 0)
				DoStep("+R1-B1+R1+F2-R1+B1+R1+F2+R2");
	}
	if(EqualSide == 1)
	{
		switch(re_i)
		{
		case Front:
				DoStep("+R1-B1+R1+F2-R1+B1+R1+F2+R2");
				break;
		case Back:
				DoStep("+U2+R1-B1+R1+F2-R1+B1+R1+F2+R2");
				break;
		case Left:
				DoStep("-U1+R1-B1+R1+F2-R1+B1+R1+F2+R2");
				break;
		case Right:
				DoStep("+U1+R1-B1+R1+F2-R1+B1+R1+F2+R2");
				break;
		}
	}

	EqualSide = 0;                                                              //标志清零
	for(i=Front,re_i=0;i<=Right;i++)
	{
		if(MoFang[i][1] == MoFang[i][3] && MoFang[i][3] == MoFang[i][2])                    //判断有无同一边有三块颜色相等的情况
		{
				EqualSide++;
				re_i = i;
				if(EqualSide >= 2)
				{
						EqualSide = 4;
						break;
				}
		}
	}
	if(EqualSide == 1)
	{
		switch(re_i)
		{
		case Front:
				DoStep("+U2");
				break;
		case Back:
				break;
		case Left:
				DoStep("+U1");
				break;
		case Right:
				DoStep("-U1");
				break;
		}
		if(MoFang[Front][2] == MoFang[Left][1])
		{
				DoStep("+R1-U1+R1+U1+R1+U1+R1-U1-R1-U1+R2");
				DoStep("+R1-U1+R1+U1+R1+U1+R1-U1-R1-U1+R2");
		}
		else
				DoStep("+R1-U1+R1+U1+R1+U1+R1-U1-R1-U1+R2");

	}
	else
	{
		if(EqualSide==0)
		{
//			if(MoFang[Front][2] == MoFang[Back][2])
				if(MoFang[Front][1] == MoFang[Back][2])
				{
						DoStep("+R1-U1+R1+U1+R1+U1+R1-U1-R1-U1+R2+U1");
						DoStep("+R1-U1+R1+U1+R1+U1+R1-U1-R1-U1+R2");
				}
				else
				{
						DoStep("+R1-U1+R1+U1+R1+U1+R1-U1-R1-U1+R2-U1");
						DoStep("+R1-U1+R1+U1+R1+U1+R1-U1-R1-U1+R2");
				}
		}
	}
	for(i=Front;i<=Right;i++)                                   //色块对齐，完成解算
	{
		if(MoFang[i][1] == MoFang[Front][5])											//搜索与前面中心色块颜色一样的面，然后将该面旋转到前面
				break;
	}
	switch(i)
	{
			case Front:
					break;
			case Back:
					DoStep("+U2");
					break;
			case Left:
					DoStep("-U1");
					break;
			case Right:
					DoStep("+U1");
					break;
	}
}


/*******************************************************/




/****************change direction function****************/

static void F(short degree,short dir)
{
    short i;
    short temp[21];

     *(pRestore++) = (10+degree)*dir;
     *pRestore = '#';

    if(dir == AW && degree == 1)
        degree = 3;                                     //-90  <<==>>   +270	即正90°旋转3次

    for(i=0;i<degree;i++)
    {
        temp[0] =  MoFang[Up][7];                        //  u7
        temp[1] =  MoFang[Up][8];                        //  u8
        temp[2] =  MoFang[Up][9];                        //  u9
        temp[3] =  MoFang[Right][1];                     //  r1
        temp[4] =  MoFang[Right][4];                     //  r4
        temp[5] =  MoFang[Right][7];                     //  r7
        temp[6] =  MoFang[Down][3];                      //  d3
        temp[7] =  MoFang[Down][2];                      //  d2
        temp[8] =  MoFang[Down][1];                      //  d1
        temp[9] =  MoFang[Left][9];                      //  l9
        temp[10]=  MoFang[Left][6];                      //  l6
        temp[11]=  MoFang[Left][3];                      //  l3

        temp[12]=  MoFang[Front][1];
        temp[13]=  MoFang[Front][2];
        temp[14]=  MoFang[Front][3];
        temp[15]=  MoFang[Front][4];
 //       temp[16]=  MoFang[Front][5];
        temp[17]=  MoFang[Front][6];
        temp[18]=  MoFang[Front][7];
        temp[19]=  MoFang[Front][8];
        temp[20]=  MoFang[Front][9];

        MoFang[Up][7]    =   temp[9];                     //  u7 = l9
        MoFang[Up][8]    =   temp[10];                     // u8 = l6
        MoFang[Up][9]    =   temp[11];                     // u9 = l3
        MoFang[Right][1] =   temp[0];                     //  r1 = u7
        MoFang[Right][4] =   temp[1];                     //  r4 = u8
        MoFang[Right][7] =   temp[2];                     //  r7 = u9
        MoFang[Down][3]  =   temp[3];                     //  d3 = r1
        MoFang[Down][2]  =   temp[4];                    //   d2 = r4
        MoFang[Down][1]  =   temp[5];                    //   d1 = r7
        MoFang[Left][9]  =   temp[6];                     //  l9 = d3
        MoFang[Left][6]  =   temp[7];                     //  l6 = d2
        MoFang[Left][3]  =   temp[8];                     //  l3 = d1

        MoFang[Front][1] =   temp[18];
        MoFang[Front][2] =   temp[15];
        MoFang[Front][3] =   temp[12];
        MoFang[Front][4] =   temp[19];
//        MoFang[Front][5] =   temp[18];
        MoFang[Front][6] =   temp[13];
        MoFang[Front][7] =   temp[20];
        MoFang[Front][8] =   temp[17];
        MoFang[Front][9] =   temp[14];


    }
}

static void B(short degree,short dir)
{
     short i;
    short temp[21];

    *(pRestore++) = (20+degree)*dir;
    *pRestore = '#';

    if(dir == AW && degree == 1)
        degree = 3;                                     //-90  <<==>>   +270	

    for(i=0;i<degree;i++)
    {
        temp[0] =  MoFang[Up][1];                        //  u1
        temp[1] =  MoFang[Up][2];                        //  u2
        temp[2] =  MoFang[Up][3];                        //  u3
        temp[3] =  MoFang[Right][3];                     //  r3
        temp[4] =  MoFang[Right][6];                     //  r6
        temp[5] =  MoFang[Right][9];                     //  r9
        temp[6] =  MoFang[Down][7];                      //  d7
        temp[7] =  MoFang[Down][8];                      //  d8
        temp[8] =  MoFang[Down][9];                      //  d9
        temp[9] =  MoFang[Left][1];                      //  l1
        temp[10]=  MoFang[Left][4];                      //  l4
        temp[11]=  MoFang[Left][7];                      //  l7
        temp[12]=  MoFang[Back][1];                     //旋转面
        temp[13]=  MoFang[Back][2];
        temp[14]=  MoFang[Back][3];
        temp[15]=  MoFang[Back][4];
 //       temp[16]=  MoFang[Front][5];
        temp[17]=  MoFang[Back][6];
        temp[18]=  MoFang[Back][7];
        temp[19]=  MoFang[Back][8];
        temp[20]=  MoFang[Back][9];

        MoFang[Up][1]    =   temp[3];                     // u1 = r3
        MoFang[Up][2]    =   temp[4];                     // u2 = r6
        MoFang[Up][3]    =   temp[5];                      // u3 = r9
        MoFang[Right][3] =   temp[8];                     //  r3 = d9
        MoFang[Right][6] =   temp[7];                     //  r6 = d8
        MoFang[Right][9] =   temp[6];                     //  r9 = d7
        MoFang[Down][7]  =   temp[9];                     //  d7 = l1
        MoFang[Down][8]  =   temp[10];                    //   d8 = l4
        MoFang[Down][9]  =   temp[11];                    //   d9 = l7
        MoFang[Left][1]  =   temp[2];                     //  l1 = u3
        MoFang[Left][4]  =   temp[1];                     //  l4 = u2
        MoFang[Left][7]  =   temp[0];                     //  l7 = u1
        MoFang[Back][1] =   temp[18];
        MoFang[Back][2] =   temp[15];
        MoFang[Back][3] =   temp[12];
        MoFang[Back][4] =   temp[19];
//        MoFang[Front][5] =   temp[18];
        MoFang[Back][6] =   temp[13];
        MoFang[Back][7] =   temp[20];
        MoFang[Back][8] =   temp[17];
        MoFang[Back][9] =   temp[14];
    }
}

static void L(short degree,short dir)
{
    short i;
    short temp[21];

    *(pRestore++) = (30+degree)*dir;
    *pRestore = '#';

    if(dir == AW && degree == 1)
        degree = 3;                                     //-90  <<==>>   +270

    for(i=0;i<degree;i++)
    {
        temp[0] =  MoFang[Up][1];                        //  u1
        temp[1] =  MoFang[Up][4];                        //  u4
        temp[2] =  MoFang[Up][7];                        //  u7
        temp[3] =  MoFang[Front][1];                     //  f1
        temp[4] =  MoFang[Front][4];                     //  f4
        temp[5] =  MoFang[Front][7];                     //  f7
        temp[6] =  MoFang[Down][1];                      //  d1
        temp[7] =  MoFang[Down][4];                      //  d4
        temp[8] =  MoFang[Down][7];                      //  d7
        temp[9] =  MoFang[Back][3];                      //  b3
        temp[10]=  MoFang[Back][6];                      //  b6
        temp[11]=  MoFang[Back][9];                      //  b9
        temp[12]=  MoFang[Left][1];                     //旋转面
        temp[13]=  MoFang[Left][2];
        temp[14]=  MoFang[Left][3];
        temp[15]=  MoFang[Left][4];
 //       temp[16]=  MoFang[Front][5];
        temp[17]=  MoFang[Left][6];
        temp[18]=  MoFang[Left][7];
        temp[19]=  MoFang[Left][8];
        temp[20]=  MoFang[Left][9];

        MoFang[Up][1]    =   temp[11];                     //  u1 = b9
        MoFang[Up][4]    =   temp[10];                     //  u4 = b6
        MoFang[Up][7]    =   temp[9];                      // u7 = b3
        MoFang[Front][1] =   temp[0];                     //  f1 = u1
        MoFang[Front][4] =   temp[1];                     //  f4 = u4
        MoFang[Front][7] =   temp[2];                     //  f7 = u7
        MoFang[Down][1]  =   temp[3];                     //  d1 = f1
        MoFang[Down][4]  =   temp[4];                    //  d4 = f4
        MoFang[Down][7]  =   temp[5];                    //  d7 = f7
        MoFang[Back][3]  =   temp[8];                     //  b3 = d7
        MoFang[Back][6]  =   temp[7];                     //  b6 = d4
        MoFang[Back][9]  =   temp[6];                     //  b9 = d1
        MoFang[Left][1] =   temp[18];
        MoFang[Left][2] =   temp[15];
        MoFang[Left][3] =   temp[12];
        MoFang[Left][4] =   temp[19];
//        MoFang[Front][5] =   temp[18];
        MoFang[Left][6] =   temp[13];
        MoFang[Left][7] =   temp[20];
        MoFang[Left][8] =   temp[17];
        MoFang[Left][9] =   temp[14];
    }
}

static void R(short degree,short dir)
{
    short i;
    short temp[21];

    *(pRestore++) = (40+degree)*dir;
    *pRestore = '#';

    if(dir == AW && degree == 1)
        degree = 3;                                     //-90  <<==>>   +270

    for(i=0;i<degree;i++)
    {
        temp[0] =  MoFang[Up][3];                        //  u3
        temp[1] =  MoFang[Up][6];                        //  u6
        temp[2] =  MoFang[Up][9];                        //  u9
        temp[3] =  MoFang[Front][3];                     //  f3
        temp[4] =  MoFang[Front][6];                     //  f6
        temp[5] =  MoFang[Front][9];                     //  f9
        temp[6] =  MoFang[Down][3];                      //  d3
        temp[7] =  MoFang[Down][6];                      //  d6
        temp[8] =  MoFang[Down][9];                      //  d9
        temp[9] =  MoFang[Back][1];                      //  b1
        temp[10]=  MoFang[Back][4];                      //  b4
        temp[11]=  MoFang[Back][7];                      //  b7

        temp[12]=  MoFang[Right][1];                     //旋转面
        temp[13]=  MoFang[Right][2];
        temp[14]=  MoFang[Right][3];
        temp[15]=  MoFang[Right][4];
 //       temp[16]=  MoFang[Front][5];
        temp[17]=  MoFang[Right][6];
        temp[18]=  MoFang[Right][7];
        temp[19]=  MoFang[Right][8];
        temp[20]=  MoFang[Right][9];

        MoFang[Up][3]    =   temp[3];                     //  u3 = f3
        MoFang[Up][6]    =   temp[4];                     //  u6 = f6
        MoFang[Up][9]    =   temp[5];                      // u9 = f9
        MoFang[Front][3] =   temp[6];                     //  f3 = d3
        MoFang[Front][6] =   temp[7];                     //  f6 = d6
        MoFang[Front][9] =   temp[8];                     //  f9 = d9
        MoFang[Down][3]  =   temp[11];                     //  d3 = b7
        MoFang[Down][6]  =   temp[10];                    //  d6 = b4
        MoFang[Down][9]  =   temp[9];                    //  d9 = b1
        MoFang[Back][1]  =   temp[2];                     //  b1 = u9
        MoFang[Back][4]  =   temp[1];                     //  b4 = u6
        MoFang[Back][7]  =   temp[0];                     //  b7 = u3

        MoFang[Right][1] =   temp[18];
        MoFang[Right][2] =   temp[15];
        MoFang[Right][3] =   temp[12];
        MoFang[Right][4] =   temp[19];
//        MoFang[Front][5] =   temp[18];
        MoFang[Right][6] =   temp[13];
        MoFang[Right][7] =   temp[20];
        MoFang[Right][8] =   temp[17];
        MoFang[Right][9] =   temp[14];
    }
}

static void U(short degree,short dir)
{
    short i;
    short temp[21];

    *(pRestore++) = (50+degree)*dir;
    *pRestore = '#';

    if(dir == AW && degree == 1)
        degree = 3;                                     //-90  <<==>>   +270

    for(i=0;i<degree;i++)
    {
        temp[0] =  MoFang[Right][1];                     //  r1
        temp[1] =  MoFang[Right][2];                     //  r2
        temp[2] =  MoFang[Right][3];                     //  r3
        temp[3] =  MoFang[Front][1];                     //  f1
        temp[4] =  MoFang[Front][2];                     //  f2
        temp[5] =  MoFang[Front][3];                     //  f3
        temp[6] =  MoFang[Left][1];                      //  l1
        temp[7] =  MoFang[Left][2];                      //  l2
        temp[8] =  MoFang[Left][3];                      //  l3
        temp[9] =  MoFang[Back][1];                      //  b1
        temp[10]=  MoFang[Back][2];                      //  b2
        temp[11]=  MoFang[Back][3];                      //  b3
        temp[12]=  MoFang[Up][1];                        //旋转面 u1
        temp[13]=  MoFang[Up][2];                        //u2....
        temp[14]=  MoFang[Up][3];
        temp[15]=  MoFang[Up][4];
 //       temp[16]=  MoFang[Front][5];
        temp[17]=  MoFang[Up][6];
        temp[18]=  MoFang[Up][7];
        temp[19]=  MoFang[Up][8];
        temp[20]=  MoFang[Up][9];                        //u9

        MoFang[Right][1] =   temp[9];                     //  r1 = b1
        MoFang[Right][2] =   temp[10];                     //  r2 = b2
        MoFang[Right][3] =   temp[11];                      // r3 = b3
        MoFang[Front][1] =   temp[0];                     //  f1 = r1
        MoFang[Front][2] =   temp[1];                     //  f2 = r2
        MoFang[Front][3] =   temp[2];                     //  f3 = r3
        MoFang[Left][1]  =   temp[3];                     //  l1 = f1
        MoFang[Left][2]  =   temp[4];                    //  l2 = f2
        MoFang[Left][3]  =   temp[5];                    //  l3 = f3
        MoFang[Back][1]  =   temp[6];                     //  b1 = l1
        MoFang[Back][2]  =   temp[7];                     //  b2 = l2
        MoFang[Back][3]  =   temp[8];                     //  b3 = l3
        MoFang[Up][1] =   temp[18];
        MoFang[Up][2] =   temp[15];
        MoFang[Up][3] =   temp[12];
        MoFang[Up][4] =   temp[19];
//        MoFang[Front][5] =   temp[18];
        MoFang[Up][6] =   temp[13];
        MoFang[Up][7] =   temp[20];
        MoFang[Up][8] =   temp[17];
        MoFang[Up][9] =   temp[14];
    }
}

static void D(short degree,short dir)
{
    short i;
    short temp[21];

    *(pRestore++) = (60+degree)*dir;
    *pRestore = '#';

    if(dir == AW && degree == 1)
        degree = 3;                                     //-90  <<==>>   +270

    for(i=0;i<degree;i++)
    {
        temp[0] =  MoFang[Right][7];                     //  r7
        temp[1] =  MoFang[Right][8];                     //  r8
        temp[2] =  MoFang[Right][9];                     //  r9
        temp[3] =  MoFang[Front][7];                     //  f7
        temp[4] =  MoFang[Front][8];                     //  f8
        temp[5] =  MoFang[Front][9];                     //  f9
        temp[6] =  MoFang[Left][7];                      //  l7
        temp[7] =  MoFang[Left][8];                      //  l8
        temp[8] =  MoFang[Left][9];                      //  l9
        temp[9] =  MoFang[Back][7];                      //  b7
        temp[10]=  MoFang[Back][8];                      //  b8
        temp[11]=  MoFang[Back][9];                      //  b9
        temp[12]=  MoFang[Down][1];                     //旋转面
        temp[13]=  MoFang[Down][2];
        temp[14]=  MoFang[Down][3];
        temp[15]=  MoFang[Down][4];
 //       temp[16]=  MoFang[Front][5];
        temp[17]=  MoFang[Down][6];
        temp[18]=  MoFang[Down][7];
        temp[19]=  MoFang[Down][8];
        temp[20]=  MoFang[Down][9];

        MoFang[Right][7] =   temp[3];                     //  r7 = f7
        MoFang[Right][8] =   temp[4];                     //  r8 = f8
        MoFang[Right][9] =   temp[5];                      // r9 = f9
        MoFang[Front][7] =   temp[6];                     //  f7 = l7
        MoFang[Front][8] =   temp[7];                     //  f8 = l8
        MoFang[Front][9] =   temp[8];                     //  f9 = l9
        MoFang[Left][7]  =   temp[9];                     //  l7 = b7
        MoFang[Left][8]  =   temp[10];                    //  l8 = b8
        MoFang[Left][9]  =   temp[11];                    //  l9 = b9
        MoFang[Back][7]  =   temp[0];                     //  b7 = r7
        MoFang[Back][8]  =   temp[1];                     //  b8 = r8
        MoFang[Back][9]  =   temp[2];                     //  b9 = r9
        MoFang[Down][1] =   temp[18];
        MoFang[Down][2] =   temp[15];
        MoFang[Down][3] =   temp[12];
        MoFang[Down][4] =   temp[19];
//        MoFang[Front][5] =   temp[18];
        MoFang[Down][6] =   temp[13];
        MoFang[Down][7] =   temp[20];
        MoFang[Down][8] =   temp[17];
        MoFang[Down][9] =   temp[14];
    }
}

static void Z(short degree,short dir)
{
    short i;
    short temp[16]={0};

    *(pRestore++) = (70+degree)*dir;
    *pRestore = '#';

    if(dir == AW && degree == 1)
        degree = 3;                                     //-90  <<==>>   +270

    for(;degree;degree--)
    {
        for(i=1;i<10;i++)
        {
            MoFang[Resever][i] = MoFang[Front][i];
        }
        for(i=1;i<10;i++)
        {
            MoFang[Front][i] = MoFang[Right][i];
        }
        for(i=1;i<10;i++)
        {
            MoFang[Right][i] = MoFang[Back][i];
        }
        for(i=1;i<10;i++)
        {
            MoFang[Back][i] = MoFang[Left][i];
        }
        for(i=1;i<10;i++)
        {
            MoFang[Left][i] = MoFang[Resever][i];
        }
        temp[0]=  MoFang[Down][1];                     //D旋转面
        temp[1]=  MoFang[Down][2];
        temp[2]=  MoFang[Down][3];
        temp[3]=  MoFang[Down][4];
        temp[4]=  MoFang[Down][6];
        temp[5]=  MoFang[Down][7];
        temp[6]=  MoFang[Down][8];
        temp[7]=  MoFang[Down][9];

        temp[8]=  MoFang[Up][1];                     //U旋转面
        temp[9]=  MoFang[Up][2];
        temp[10]=  MoFang[Up][3];
        temp[11]=  MoFang[Up][4];
        temp[12]=  MoFang[Up][6];
        temp[13]=  MoFang[Up][7];
        temp[14]=  MoFang[Up][8];
        temp[15]=  MoFang[Up][9];

        MoFang[Down][1] =   temp[2];                 //D旋转后的结果
        MoFang[Down][2] =   temp[4];
        MoFang[Down][3] =   temp[7];
        MoFang[Down][4] =   temp[1];
        MoFang[Down][6] =   temp[6];
        MoFang[Down][7] =   temp[0];
        MoFang[Down][8] =   temp[3];
        MoFang[Down][9] =   temp[5];

        MoFang[Up][1] =   temp[13];                  //U旋转后的结果
        MoFang[Up][2] =   temp[11];
        MoFang[Up][3] =   temp[8];
        MoFang[Up][4] =   temp[14];
        MoFang[Up][6] =   temp[9];
        MoFang[Up][7] =   temp[15];
        MoFang[Up][8] =   temp[12];
        MoFang[Up][9] =   temp[10];
    }

}

/**************other function****************/

static short Abs(short x)
{
    if(x > 0)
        return x;
    else
        return -x;
}

void simpleStep(void)
{
  short i,m;
  short flag=1;
	short temp[200]={0};

	while(flag)
	{
		flag = 0;
		for(i=0;Restore_step[i]!='#';i++)
		{
			if(Restore_step[i]+Restore_step[i+1] == 0)
			{
					Restore_step[i] = Restore_step[i+1] = 0;
					flag = 1;
					i++;
					continue;
			}
			if(Restore_step[i] == Restore_step[i+1])
			{
					flag = 1;
					if(Abs(Restore_step[i]%10) == 2)
					 {
							 Restore_step[i] = Restore_step[i+1] = 0;
							 i++;
							 continue;
					 }
					else
					{
							if(Restore_step[i]%10 == 1)
									Restore_step[i]=Restore_step[i]/10*10+2;
							if(Restore_step[i]%10 == -1)
									Restore_step[i]=Restore_step[i]/10*10-2;
							Restore_step[i+1] = 0;
							i++;
							continue;
					}
			}
			if(Restore_step[i]/10 == Restore_step[i+1]/10)//如果前后两个步骤为同向旋转，但角度不同
			{
					if(Abs(Restore_step[i]) > Abs(Restore_step[i+1]))   //前面步骤绝对值大于后，则符号由前值决定
							Restore_step[i] = ((Restore_step[i]/10)*10)+(Restore_step[i]%4 + Restore_step[i+1]%4);
					else
							Restore_step[i] = ((Restore_step[i+1]/10)*10)+(Restore_step[i]%4 + Restore_step[i+1]%4);
					Restore_step[i+1] = 0;
					i++;
					flag = 1;
					continue;
			}
			else
			{
				if(Restore_step[i]/10 == -Restore_step[i+1]/10)
				{
					if(Abs(Restore_step[i]) > Abs(Restore_step[i+1]))   //前面步骤绝对值大于后，则符号由前值决定
						Restore_step[i] = ((Restore_step[i]/10)*10)+(Restore_step[i]%10 + Restore_step[i+1]%10);
					else
						Restore_step[i] = ((Restore_step[i+1]/10)*10)+(Restore_step[i]%10 + Restore_step[i+1]%10);
					Restore_step[i+1] = 0;
					i++;
					flag = 1;
					continue;
				}
			}
		}
		for(i=0,m=0;Restore_step[i]!='#';i++)
		{
			if(Restore_step[i]!=0)
				temp[m++] = Restore_step[i];
		}
		temp[m]='#';
		for(i=0,Step_sum=0;temp[i]!='#';i++)
		{
			Step_sum++;
			Restore_step[i] = temp[i] ;
		}
		Restore_step[i]='#';
	}
	i = 0;
	while(Restore_step[i] != '#')
	{
		if( Restore_step[i] % 10 == 3)
		{
			Restore_step[i] = -((Restore_step[i]/10)*10+1);
		}
		if( Restore_step[i] % 10 == -3)
		{
			Restore_step[i] = -((Restore_step[i]/10)*10-1);
		}
		i++;
	}
}

void clean_71(void)
{
	int i = 0 , j;
	while(Restore_step[i] != '#')
	{
		if(Restore_step[i] == 71)
		{
			j = i;
			while(Restore_step[j+1] != '#')
			{
				switch(Restore_step[j+1] / 10)
				{
					case 1 : Restore_step[j] = 40+Restore_step[j+1]%10; break;
					case 2 : Restore_step[j] = 30+Restore_step[j+1]%10; break;
					case 3 : Restore_step[j] = 10+Restore_step[j+1]%10; break;
					case 4 : Restore_step[j] = 20+Restore_step[j+1]%10; break;
					case -1 : Restore_step[j] = -40+Restore_step[j+1]%10; break;
					case -2 : Restore_step[j] = -30+Restore_step[j+1]%10; break;
					case -3 : Restore_step[j] = -10+Restore_step[j+1]%10; break;
					case -4 : Restore_step[j] = -20+Restore_step[j+1]%10; break;
					default : Restore_step[j] = Restore_step[j+1]; break;
				}
				j++;
			}
			Restore_step[j] = '#';
		}
		if(Restore_step[i] == -71)
		{
			j = i;
			while(Restore_step[j+1] != '#')
			{
				switch(Restore_step[j+1] / 10)
				{
					case 1 : Restore_step[j] = 30+Restore_step[j+1]%10; break;
					case 2 : Restore_step[j] = 40+Restore_step[j+1]%10; break;
					case 3 : Restore_step[j] = 20+Restore_step[j+1]%10; break;
					case 4 : Restore_step[j] = 10+Restore_step[j+1]%10; break;
					case -1 : Restore_step[j] = -30+Restore_step[j+1]%10; break;
					case -2 : Restore_step[j] = -40+Restore_step[j+1]%10; break;
					case -3 : Restore_step[j] = -20+Restore_step[j+1]%10; break;
					case -4 : Restore_step[j] = -10+Restore_step[j+1]%10; break;
					default : Restore_step[j] = Restore_step[j+1]; break;
				}
				j++;
			}
			Restore_step[j] = '#';
		}
		if((Restore_step[i] == 72) || (Restore_step[i] == -72))
		{
			j = i;
			while(Restore_step[j+1] != '#')
			{
				switch(Restore_step[j+1] / 10)
				{
					case 1 : Restore_step[j] = 20+Restore_step[j+1]%10; break;
					case 2 : Restore_step[j] = 10+Restore_step[j+1]%10; break;
					case 3 : Restore_step[j] = 40+Restore_step[j+1]%10; break;
					case 4 : Restore_step[j] = 30+Restore_step[j+1]%10; break;
					case -1 : Restore_step[j] = -20+Restore_step[j+1]%10; break;
					case -2 : Restore_step[j] = -10+Restore_step[j+1]%10; break;
					case -3 : Restore_step[j] = -40+Restore_step[j+1]%10; break;
					case -4 : Restore_step[j] = -30+Restore_step[j+1]%10; break;
					default : Restore_step[j] = Restore_step[j+1]; break;
				}
				j++;
			}
			Restore_step[j] = '#';
		}
		i++;
	}
}
