#include "sys.h"
#include "delay.h"
#include "OV7670.h"
#include "sccb.h"
#include "usart.h"
#include "dcmi.h" 
#include "imgprcs.h"

u8 rows_get;    //存储到哪一行
u8 row[640];
u8 R[120][120];
u8 G[120][120];
u8 B[120][120];
u8 Gray[120][120];

StdColor_TypeDef std_color;     //五个颜色的标准色

struct 
{
	u8 x;
	u8 y;
}grow_stack[1000];   //区域生长法的栈

struct 
{
	u8 x;
	u8 y;
}class_posi[2000];   


/*错误返回0，正确返回1*/
/*开始时为每种颜色寻找最优三颜色分量*/
/*并在起开始时候的识别和中心黑圆的坐标输出*/
u8 get_stdcolor(void)   //寻找几种颜色的标准值和输出位置信息
{
	u16 i,j,k;             	 //i:行号，j：列号，k区域号(从1开始)
	u8 area_num2;          	 //区域数量  
	
	ImgPrcs_TypeDef area_info[20];  //定义各区域信息结构体变量
	
	for(k=0;k<20;k++)
	{
		area_info[k].color=0;
		area_info[k].acre=0;
	}
	
	roberts(Gray,120,120);   //进行一次边缘检测
	
	area_num2=area_grow();   //图像分块并存入Gray , 存入格式：0x80+area_num，0x80代表有效边缘，area_num为分区编号
	if(area_num2==0 || area_num2>19) 
		return 0;
	
	/*统计各区域面积，计算各区域中心坐标*/
	for(i=0;i<area_num2;i++)  
	{
		area_info[i].acre=0;
		area_info[i].area_x=0;
		area_info[i].area_y=0;
	}
	for(i=0;i<120;i++)       //统计各区域数量，计算x、y总和
		for(j=0;j<120;j++)
		{
			if(Gray[i][j] >0x80 ) 
			{
				u8 temp = Gray[i][j]&0x7f;
				area_info[temp].acre++;
				area_info[temp].area_x += j;
				area_info[temp].area_y += i;
			}
		}
	for(i=0;i<area_num2;i++)  //得各区域x，y坐标
	{
		area_info[i].area_x = area_info[i].area_x/area_info[i].acre;
		area_info[i].area_y = area_info[i].area_y/area_info[i].acre;
	}
	
	/*找小圆和中心圆 */
	u8 temp_y,temp_max;				//找最大半径时用    
	u8 max_num=0;             //找出的最大半径区域号
	for(k=1;k<area_num2;k++)     //修改：最后一列全置为0，k从1开始
	{
		area_info[k].Y_N=1;   //假定是圆
		for(i=area_info[k].area_y ;i>1;i--)           //向上穿线
			if(Gray[i][area_info[k].area_x] == 0x80+k)
			{
				area_info[k].get=area_info[k].area_y-i;
				break;
			}
		if(i<=1) {area_info[k].Y_N=0;goto END;}
		for( ;i>1;i--)
			if(Gray[i][area_info[k].area_x] != 0x80+k)
			{
				area_info[k].leave=area_info[k].area_y-i;
				break;
			}
		
		for(i=area_info[k].area_y ,j=area_info[k].area_x ; i>1&&j<117 ; i--,j++)           //右上穿线
			if(Gray[i][j] == 0x80+k)
			{
				temp_y=area_info[k].area_y-i;
				if( temp_y>area_info[k].get || 2*temp_y<area_info[k].get)
				{
					area_info[k].Y_N=0;
					goto END;                 //完全跳出
				}
				area_info[k].get=temp_y;
				break;
			}
		if(i<=1) {area_info[k].Y_N=0;goto END;}
		for( ; i>1&&j<117 ; i--,j++)
			if(Gray[i][j] != 0x80+k)
			{
				temp_y=area_info[k].area_y-i;
				if( temp_y>area_info[k].leave || 2*temp_y<area_info[k].leave)
				{
					area_info[k].Y_N=0;
					goto END;                 //完全跳出
				}
				area_info[k].leave=temp_y;
				break;
			}
		
		for(i=area_info[k].area_y ;i<118;i++)           //向xia穿线
			if(Gray[i][area_info[k].area_x] == 0x80+k)
			{
				temp_y=i-area_info[k].area_y;
				if( temp_y<area_info[k].get || temp_y>area_info[k].get*2)
				{
					area_info[k].Y_N=0;
					goto END;                 //完全跳出
				}
				area_info[k].get=temp_y;
				break;
			}
		if(i>=118) {area_info[k].Y_N=0;goto END;}
		for( ;i<118;i++)
			if(Gray[i][area_info[k].area_x] != 0x80+k)
			{
				temp_y=i-area_info[k].area_y;
				if( temp_y<area_info[k].leave || temp_y>area_info[k].leave*2)
				{
					area_info[k].Y_N=0;
					goto END;                 //完全跳出
				}
				area_info[k].leave=temp_y;
				break;
			}
			
		for(i=area_info[k].area_y,j=area_info[k].area_x ; i<118&&j>1 ;i++,j--)           //左下穿线
			if(Gray[i][j] == 0x80+k)
			{
				temp_y=i-area_info[k].area_y;
				if( temp_y>area_info[k].get || 2*temp_y<area_info[k].get)
				{
					area_info[k].Y_N=0;
					goto END;                 //完全跳出
				}
				area_info[k].get=temp_y;
				break;
			}
		if(i>=118) {area_info[k].Y_N=0;goto END;}
		for( ; i<118&&j>1 ; i++,j--)
			if(Gray[i][j] != 0x80+k)
			{
				temp_y=i-area_info[k].area_y;
				if( temp_y>area_info[k].leave || 2*temp_y<area_info[k].leave)
				{
					area_info[k].Y_N=0;
					goto END;                 //完全跳出
				}
				area_info[k].leave=temp_y;
				break;
			}
		END:;                    //当前区域不是圆，提前跳出循环
	}
	
	/*丢弃远离中心圆的部分*/
	temp_max=area_info[1].get;
	for(k=2;k<area_num2;k++)
		if(temp_max<area_info[k].get)
			temp_max=area_info[k].get;
	for(k=1;k<area_num2;k++)
		if(temp_max==area_info[k].get)
			max_num=k;
	for(k=1;k<area_num2;k++)
	{
		if( (area_info[k].area_x-area_info[max_num].area_x)*(area_info[k].area_x-area_info[max_num].area_x)+
					(area_info[k].area_y-area_info[max_num].area_y)*(area_info[k].area_y-area_info[max_num].area_y) >
				area_info[max_num].leave*area_info[max_num].leave*9)
			area_info[k].acre=0;
	}
	
	/*给各颜色找一个最符合的区域*/
	int temp1=0 ;        	 
	u8 temp_num=0; 					 
	for(k=1;k<area_num2;k++)
		if(area_info[k].acre!=0)
			area_info[k].color=0;  //0代表没有颜色归属
	
	temp_num=1;								//找黑色
	temp1=100;
	for(k=1;k<area_num2;k++)       
	{
		if(area_info[k].acre != 0)
		{
			int temp2=abs(R[area_info[k].area_y][area_info[k].area_x]-G[area_info[k].area_y][area_info[k].area_x])+
							  abs(G[area_info[k].area_y][area_info[k].area_x]-B[area_info[k].area_y][area_info[k].area_x])+
							  abs(B[area_info[k].area_y][area_info[k].area_x]-R[area_info[k].area_y][area_info[k].area_x]);
			if(temp1>temp2) {temp_num=k;  temp1=temp2;}    //加和越小越好
		}
	}
	area_info[temp_num].color |= 0x02;    //00000010/0x02代表黑色
	
	temp_num=1;								//找红色
	temp1=-100;
	for(k=1;k<area_num2;k++)                   
	{
		if(area_info[k].acre != 0 /*&& area_info[k].color != 0*/)   //找过的跳掉
		{
			int temp2=	(R[area_info[k].area_y][area_info[k].area_x]-G[area_info[k].area_y][area_info[k].area_x])+
									(R[area_info[k].area_y][area_info[k].area_x]-B[area_info[k].area_y][area_info[k].area_x])-
							 abs(B[area_info[k].area_y][area_info[k].area_x]-G[area_info[k].area_y][area_info[k].area_x]);
			if(temp1<temp2){temp_num=k;  temp1=temp2;}        //加和越大越好
		}
	}
	area_info[temp_num].color |= 0x04;    //0x04代表红色
	
	temp_num=1;								//找绿色
	temp1=-100;
	for(k=1;k<area_num2;k++)       
	{
		if(area_info[k].acre != 0 /*&& area_info[k].color != 0*/)   //找过的跳掉
		{
			int temp2=	(G[area_info[k].area_y][area_info[k].area_x]-R[area_info[k].area_y][area_info[k].area_x])+
									(G[area_info[k].area_y][area_info[k].area_x]-B[area_info[k].area_y][area_info[k].area_x])-
							 abs(B[area_info[k].area_y][area_info[k].area_x]-R[area_info[k].area_y][area_info[k].area_x]);
			if(temp1<temp2){temp_num=k;  temp1=temp2;}         //加和越大越好
		}
	}
	area_info[temp_num].color |= 0x08;    //0x08代表绿色
	
	temp_num=1;								//找蓝色
	temp1=-100;
	for(k=1;k<area_num2;k++)       
	{
		if(area_info[k].acre != 0 /*&& area_info[k].color != 0*/)   //找过的跳掉
		{
			int temp2=	(B[area_info[k].area_y][area_info[k].area_x]-R[area_info[k].area_y][area_info[k].area_x])+
									(B[area_info[k].area_y][area_info[k].area_x]-G[area_info[k].area_y][area_info[k].area_x])-
							 abs(R[area_info[k].area_y][area_info[k].area_x]-G[area_info[k].area_y][area_info[k].area_x]);
			if(temp1<temp2){temp_num=k;  temp1=temp2;}           //加和越大越好
		}
	}
	area_info[temp_num].color |= 0x10;    //0x10代表蓝色
	
	temp_num=1;								//找黄色
	temp1=-100;
	for(k=1;k<area_num2;k++)       
	{
		if(area_info[k].acre != 0 /*&& area_info[k].color != 0*/)   //找过的跳掉
		{
			int temp2=	(R[area_info[k].area_y][area_info[k].area_x]-B[area_info[k].area_y][area_info[k].area_x])+
									(G[area_info[k].area_y][area_info[k].area_x]-B[area_info[k].area_y][area_info[k].area_x])-
							 abs(R[area_info[k].area_y][area_info[k].area_x]-G[area_info[k].area_y][area_info[k].area_x]);
			if(temp1<temp2){temp_num=k;  temp1=temp2;}                 //加和越大越好
		}
	}
	area_info[temp_num].color |= 0x20;    //0x20代表黄色
	
	/*对被重复给色的区域确定其最合适颜色*/
	u8 max_R=0, max_G=0, max_B=0, min_R=100, min_G=100, min_B=100;
	u8 color_count;
	int distR,distG,distB,distY,distK,dist_min=999;
	
	for(k=1;k<area_num2;k++)
		if(area_info[k].acre!=0)
		{
			if(max_R<R[area_info[k].area_y][area_info[k].area_x])
				max_R=R[area_info[k].area_y][area_info[k].area_x];
			if(max_G<G[area_info[k].area_y][area_info[k].area_x])
				max_G=G[area_info[k].area_y][area_info[k].area_x];
			if(max_B<B[area_info[k].area_y][area_info[k].area_x])
				max_B=B[area_info[k].area_y][area_info[k].area_x];
			if(min_R>R[area_info[k].area_y][area_info[k].area_x])
				min_R=R[area_info[k].area_y][area_info[k].area_x];
			if(min_G>G[area_info[k].area_y][area_info[k].area_x])
				min_G=G[area_info[k].area_y][area_info[k].area_x];
			if(min_B>B[area_info[k].area_y][area_info[k].area_x])
				min_B=B[area_info[k].area_y][area_info[k].area_x];
		}
	for(k=1;k<area_num2;k++)
		if(area_info[k].acre!=0)
		{
			color_count=0;
			for(i=1;i<=5;i++)     //这里i单纯计数变量
				if( (area_info[k].color & (0x01<<i)) !=0 )
					color_count++;
			if(color_count>1)		//说明某一区域被赋予了多种颜色
			{
				distK=distR=distG=distB=distY=1000;
				if(area_info[k].color & 0x02)    //K
				{
					distK=R[area_info[k].area_y][area_info[k].area_x]-min_R + 
								G[area_info[k].area_y][area_info[k].area_x]-min_G +
								B[area_info[k].area_y][area_info[k].area_x]-min_B ;
					if(dist_min>distK)
						dist_min=distK;
				}
					
				if(area_info[k].color & 0x04)    //R
				{
					distR=max_R-R[area_info[k].area_y][area_info[k].area_x] + 
								G[area_info[k].area_y][area_info[k].area_x]-min_G +
								B[area_info[k].area_y][area_info[k].area_x]-min_B ;
					if(dist_min>distR)
						dist_min=distR;
				}
				if(area_info[k].color & 0x08)    //G
				{
					distG=max_G-G[area_info[k].area_y][area_info[k].area_x] + 
								R[area_info[k].area_y][area_info[k].area_x]-min_R +
								B[area_info[k].area_y][area_info[k].area_x]-min_B ;
					if(dist_min>distG)
						dist_min=distG;
				}
				if(area_info[k].color & 0x10)    //B
				{
					distB=max_B-B[area_info[k].area_y][area_info[k].area_x] + 
								G[area_info[k].area_y][area_info[k].area_x]-min_G +
								R[area_info[k].area_y][area_info[k].area_x]-min_R ;
					if(dist_min>distB)
						dist_min=distB;
				}
				if(area_info[k].color & 0x20)    //Y
				{
					distY=max_R-R[area_info[k].area_y][area_info[k].area_x] + 
								max_G-G[area_info[k].area_y][area_info[k].area_x] +
								B[area_info[k].area_y][area_info[k].area_x]-min_B ;
					if(dist_min>distY)
						dist_min=distY;
				}
				if(dist_min==distK) area_info[k].color = 0x02;
				if(dist_min==distR) area_info[k].color = 0x04;
				if(dist_min==distG) area_info[k].color = 0x08;
				if(dist_min==distB) area_info[k].color = 0x10;
				if(dist_min==distY) area_info[k].color = 0x20;
			}
		}
		
		u8 color_absent=0x3e,b;  //给没有归属的颜色赋值
		for(k=1;k<area_num2;k++)
			color_absent ^= area_info[k].color;
		b=color_absent>>1<<7;        //黑
		if(b)
		{
			for(k=1;k<area_num2;k++)
				if(area_info[k].color != 0 && R[area_info[k].area_y][area_info[k].area_x]<15 
																	 && G[area_info[k].area_y][area_info[k].area_x]<15 
																	 && B[area_info[k].area_y][area_info[k].area_x]<15 
					 &&area_info[k].acre != 0)
				
					area_info[k].color=0x02;
		}
		b=color_absent>>2<<7;         //红
		if(b)
		{
			for(k=1;k<area_num2;k++)
				if(area_info[k].color != 0 && R[area_info[k].area_y][area_info[k].area_x]>12 
																	 && G[area_info[k].area_y][area_info[k].area_x]<12 
																	 && B[area_info[k].area_y][area_info[k].area_x]<12 
					 &&area_info[k].acre != 0)
				
					area_info[k].color=0x04;
		}
		b=color_absent>>3<<7;         //G
		if(b)
		{
			for(k=1;k<area_num2;k++)
				if(area_info[k].color != 0 && R[area_info[k].area_y][area_info[k].area_x]<12 
																	 && G[area_info[k].area_y][area_info[k].area_x]>12 
																	 && B[area_info[k].area_y][area_info[k].area_x]<12 
					 &&area_info[k].acre != 0)
				
					area_info[k].color=0x08;
		}
		b=color_absent>>4<<7;      //B
		if(b)
		{
			for(k=1;k<area_num2;k++)
				if(area_info[k].color != 0 && R[area_info[k].area_y][area_info[k].area_x]<12 
																	 && G[area_info[k].area_y][area_info[k].area_x]<12 
																	 && B[area_info[k].area_y][area_info[k].area_x]>12 
					 &&area_info[k].acre != 0)
				
					area_info[k].color=0x10;
		}
		b=color_absent>>5<<7;      //Y
		if(b)
		{
			for(k=1;k<area_num2;k++)
				if(area_info[k].color != 0 && R[area_info[k].area_y][area_info[k].area_x]>=15 
																	 && G[area_info[k].area_y][area_info[k].area_x]>=15 
																	 && B[area_info[k].area_y][area_info[k].area_x]<=15 
					 &&area_info[k].acre != 0)
				
					area_info[k].color=0x20;
		}
	
	/*中心黑色圆是主要区域，检查其是否符合 1存在该区域，2该区域面积最大，3该区域是圆形*/
	u8 data_valid=1,K_num;       //1是有效
	for(k=1;k<area_num2;k++)
		if(area_info[k].color == 0x02)   
			break;
	if(k==area_num2)           //没找到黑色
		data_valid=0;
	else
	{
		K_num=k;
		for(k=1;k<area_num2 && data_valid!=0;k++)
			if(area_info[k].acre != 0 && area_info[k].color != 0 )
				if( area_info[k].acre > area_info[K_num].acre)    //黑色不是面积最大的
					data_valid=0;
	}
	if(area_info[K_num].Y_N==0)    //黑色不是圆
		data_valid=0;
	
	/*输出颜色和坐标信息*/
	if(data_valid!=0)
		
		data_output(k,area_num2,area_info,data_valid);  //串口输出至电脑显示
//	img_display(Gray);
	
	return 1;
}


/***出错返回0，正确返回区域数量+1（因为k是从1开始的）*/
u8 area_grow(void)     //区域生长函数    
{ 
	u8 i,j,i_last,j_last;
	u16 n;
	int top=0;             //栈顶
	u8 area_num=1;         //各白色区域区域号 编号1开始
	u16 area_acre=0;       //单色区域面积
	for(i=4;i<120;i+=10)
		for(j=1;j<117;j++)
			if(Gray[i][j] > 9 && Gray[i-1][j] < 0x80)    //具体大于多少阈值还需计算
			{
				i_last=i; j_last=j;
				top=0;
				Gray[i][j] = 0;    //种子点
	//			data_process1();
				while(top!=-1)
				{
					/*遍历四周8个元素*/
					if(Gray[i-1][j] > 9 && Gray[i-1][j] < 0x80)
					{
						Gray[i-1][j] = 0;
						class_posi[area_acre].x = grow_stack[top].x = j;
						class_posi[area_acre].y = grow_stack[top].y=i-1;
						top++;
						area_acre++;
					}
					if(Gray[i-1][j+1] > 9 && Gray[i-1][j+1] < 0x80)
					{
						Gray[i-1][j+1] = 0;
						class_posi[area_acre].x = grow_stack[top].x=j+1;
						class_posi[area_acre].y = grow_stack[top].y=i-1;
						top++;
						area_acre++;
					}
					if(Gray[i][j+1] > 9 && Gray[i][j+1] < 0x80)
					{
						Gray[i][j+1] = 0;
						class_posi[area_acre].x = grow_stack[top].x=j+1;
						class_posi[area_acre].y = grow_stack[top].y=i;
						top++;
						area_acre++;
					}
					if(Gray[i+1][j+1] > 9 && Gray[i+1][j+1] < 0x80)
					{
						Gray[i+1][j+1] = 0;
						class_posi[area_acre].x = grow_stack[top].x=j+1;
						class_posi[area_acre].y = grow_stack[top].y=i+1;
						top++;
						area_acre++;
					}
					if(Gray[i+1][j] > 9 && Gray[i+1][j] < 0x80)
					{
						Gray[i+1][j] = 0;
						class_posi[area_acre].x = grow_stack[top].x=j;
						class_posi[area_acre].y = grow_stack[top].y=i+1;
						top++;
						area_acre++;
					}
					if(Gray[i+1][j-1] > 9 && Gray[i+1][j-1] < 0x80)
					{
						Gray[i+1][j-1] = 0;
						class_posi[area_acre].x = grow_stack[top].x=j-1;
						class_posi[area_acre].y = grow_stack[top].y=i+1;
						top++;
						area_acre++;
					}
					if(Gray[i][j-1] > 9 && Gray[i][j-1] < 0x80)
					{
						Gray[i][j-1] = 0;
						class_posi[area_acre].x = grow_stack[top].x=j-1;
						class_posi[area_acre].y = grow_stack[top].y=i;
						top++;
						area_acre++;
					}
					if(Gray[i-1][j-1] > 9 && Gray[i-1][j-1] < 0x80)
					{
						Gray[i-1][j-1] = 0;
						class_posi[area_acre].x = grow_stack[top].x=j-1;
						class_posi[area_acre].y = grow_stack[top].y=i-1;
						top++;
						area_acre++;
					}
					/*四周8个元素遍历完*/
					if(area_acre>1996 || top>998){    //太大了，丢弃
						return 0;}
					/*取出栈顶元素*/
					if(top>0)
					{
						i=grow_stack[top-1].y;
						j=grow_stack[top-1].x;
					}
					top--;
				}
				if(area_acre>20)             //区域面积足够大，有效
				{
					for(n=0;n<area_acre;n++)
					{
						Gray[ class_posi[n].y ][ class_posi[n].x ] = area_num+0x80;
					}
					area_num++;
				}
				i=i_last; j=j_last;
				area_acre=0;
			}
//	printf("%d ",area_num);
	return (area_num);
}
 
/*roberts算子边缘检测*/
void roberts(u8 in[120][120],int rows,int cols)   //边缘检测函数
{
	int h,q,v,i,j;
	for(i=0;i<rows-2;i++)
	{
		for(j=0;j<cols-2;j++)
		{
			h=in[i][j]-in[i+2][j+2];
			v=in[i][j+2]-in[i+2][j];
			q=abs(h)+abs(v);
			if(q<=2) q=0;
			if(q>127) q=127;
			in[i][j]=q;
		}
		in[i][118]=in[i][119]=0;
	}
	for(j=0;j<120;j++)
		in[118][j]=in[119][j]=0;
}

int abs(int i)
{
	if(i<0) i=-i;
	return i;
}

void img_display(u8 img[120][120])  //串口输出图像
{
	int i,j;
	DCMI_Stop();
	USART_SendData(USART1,0);
	delay_us(40);
	USART_SendData(USART1,255);
	delay_us(40);
	USART_SendData(USART1,1);
	delay_us(40);
	USART_SendData(USART1,0);
	delay_us(40);
	for(i=0;i<120;i++)
	{
		 for(j=0;j<120;j++)
		 {
			 delay_us(40);
			 USART_SendData(USART1, img[i][j]);
		 }
  }
	DCMI_Start();
}


void data_output(u16 k , u8 area_num2 , ImgPrcs_TypeDef area_info[20] , u8 data_valid)   //串口输出至电脑显示
{
	if(data_valid!=0)
	{
		for(k=1;k<area_num2;k++)    //输出各区域信息
		{
			if(area_info[k].acre != 0 && area_info[k].color != 0 )
			{
				if(area_info[k].color == 0x02)
					printf("黑     :%3d 坐标X:%3d Y%3d\r\n",area_info[k].acre,area_info[k].area_x,area_info[k].area_y);
			}
		}
		for(k=1;k<area_num2;k++)    //输出各区域信息
		{
			if(area_info[k].acre != 0 && area_info[k].color != 0 )
			{
				if(area_info[k].color == 0x04)
					printf("红     :%3d 坐标X:%3d Y%3d\r\n",area_info[k].acre,area_info[k].area_x,area_info[k].area_y);
			}
		}
		for(k=1;k<area_num2;k++)    //输出各区域信息
		{
			if(area_info[k].acre != 0 && area_info[k].color != 0 )
			{
				if(area_info[k].color == 0x08)
					printf("绿 面积:%3d 坐标X:%3d Y%3d\r\n",area_info[k].acre,area_info[k].area_x,area_info[k].area_y);
			}
		}
		for(k=1;k<area_num2;k++)    //输出各区域信息
		{
			if(area_info[k].acre != 0 && area_info[k].color != 0 )
			{
				if(area_info[k].color == 0x10)
					printf("蓝     :%3d 坐标X:%3d Y%3d\r\n",area_info[k].acre,area_info[k].area_x,area_info[k].area_y);
			}
		}
		for(k=1;k<area_num2;k++)    //输出各区域信息
		{
			if(area_info[k].acre != 0 && area_info[k].color != 0 )
			{
				if(area_info[k].color == 0x20)
					printf("黄     :%3d 坐标X:%3d Y%3d\r\n",area_info[k].acre,area_info[k].area_x,area_info[k].area_y);
			}
		}
		printf("\r\n");	
	}
	else
			printf("无效\r\n\r\n");
}


