#include "sys.h"
#include "delay.h"
#include "OV7670.h"
#include "sccb.h"
#include "usart.h"
#include "dcmi.h" 
#include "imgprcs.h"

u8 rows_get;    //�洢����һ��
u8 row[640];
u8 R[120][120];
u8 G[120][120];
u8 B[120][120];
u8 Gray[120][120];

StdColor_TypeDef std_color;     //�����ɫ�ı�׼ɫ

struct 
{
	u8 x;
	u8 y;
}grow_stack[1000];   //������������ջ

struct 
{
	u8 x;
	u8 y;
}class_posi[2000];   


/*���󷵻�0����ȷ����1*/
/*��ʼʱΪÿ����ɫѰ����������ɫ����*/
/*������ʼʱ���ʶ������ĺ�Բ���������*/
u8 get_stdcolor(void)   //Ѱ�Ҽ�����ɫ�ı�׼ֵ�����λ����Ϣ
{
	u16 i,j,k;             	 //i:�кţ�j���кţ�k�����(��1��ʼ)
	u8 area_num2;          	 //��������  
	
	ImgPrcs_TypeDef area_info[20];  //�����������Ϣ�ṹ�����
	
	for(k=0;k<20;k++)
	{
		area_info[k].color=0;
		area_info[k].acre=0;
	}
	
	roberts(Gray,120,120);   //����һ�α�Ե���
	
	area_num2=area_grow();   //ͼ��ֿ鲢����Gray , �����ʽ��0x80+area_num��0x80������Ч��Ե��area_numΪ�������
	if(area_num2==0 || area_num2>19) 
		return 0;
	
	/*ͳ�Ƹ���������������������������*/
	for(i=0;i<area_num2;i++)  
	{
		area_info[i].acre=0;
		area_info[i].area_x=0;
		area_info[i].area_y=0;
	}
	for(i=0;i<120;i++)       //ͳ�Ƹ���������������x��y�ܺ�
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
	for(i=0;i<area_num2;i++)  //�ø�����x��y����
	{
		area_info[i].area_x = area_info[i].area_x/area_info[i].acre;
		area_info[i].area_y = area_info[i].area_y/area_info[i].acre;
	}
	
	/*��СԲ������Բ */
	u8 temp_y,temp_max;				//�����뾶ʱ��    
	u8 max_num=0;             //�ҳ������뾶�����
	for(k=1;k<area_num2;k++)     //�޸ģ����һ��ȫ��Ϊ0��k��1��ʼ
	{
		area_info[k].Y_N=1;   //�ٶ���Բ
		for(i=area_info[k].area_y ;i>1;i--)           //���ϴ���
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
		
		for(i=area_info[k].area_y ,j=area_info[k].area_x ; i>1&&j<117 ; i--,j++)           //���ϴ���
			if(Gray[i][j] == 0x80+k)
			{
				temp_y=area_info[k].area_y-i;
				if( temp_y>area_info[k].get || 2*temp_y<area_info[k].get)
				{
					area_info[k].Y_N=0;
					goto END;                 //��ȫ����
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
					goto END;                 //��ȫ����
				}
				area_info[k].leave=temp_y;
				break;
			}
		
		for(i=area_info[k].area_y ;i<118;i++)           //��xia����
			if(Gray[i][area_info[k].area_x] == 0x80+k)
			{
				temp_y=i-area_info[k].area_y;
				if( temp_y<area_info[k].get || temp_y>area_info[k].get*2)
				{
					area_info[k].Y_N=0;
					goto END;                 //��ȫ����
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
					goto END;                 //��ȫ����
				}
				area_info[k].leave=temp_y;
				break;
			}
			
		for(i=area_info[k].area_y,j=area_info[k].area_x ; i<118&&j>1 ;i++,j--)           //���´���
			if(Gray[i][j] == 0x80+k)
			{
				temp_y=i-area_info[k].area_y;
				if( temp_y>area_info[k].get || 2*temp_y<area_info[k].get)
				{
					area_info[k].Y_N=0;
					goto END;                 //��ȫ����
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
					goto END;                 //��ȫ����
				}
				area_info[k].leave=temp_y;
				break;
			}
		END:;                    //��ǰ������Բ����ǰ����ѭ��
	}
	
	/*����Զ������Բ�Ĳ���*/
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
	
	/*������ɫ��һ������ϵ�����*/
	int temp1=0 ;        	 
	u8 temp_num=0; 					 
	for(k=1;k<area_num2;k++)
		if(area_info[k].acre!=0)
			area_info[k].color=0;  //0����û����ɫ����
	
	temp_num=1;								//�Һ�ɫ
	temp1=100;
	for(k=1;k<area_num2;k++)       
	{
		if(area_info[k].acre != 0)
		{
			int temp2=abs(R[area_info[k].area_y][area_info[k].area_x]-G[area_info[k].area_y][area_info[k].area_x])+
							  abs(G[area_info[k].area_y][area_info[k].area_x]-B[area_info[k].area_y][area_info[k].area_x])+
							  abs(B[area_info[k].area_y][area_info[k].area_x]-R[area_info[k].area_y][area_info[k].area_x]);
			if(temp1>temp2) {temp_num=k;  temp1=temp2;}    //�Ӻ�ԽСԽ��
		}
	}
	area_info[temp_num].color |= 0x02;    //00000010/0x02�����ɫ
	
	temp_num=1;								//�Һ�ɫ
	temp1=-100;
	for(k=1;k<area_num2;k++)                   
	{
		if(area_info[k].acre != 0 /*&& area_info[k].color != 0*/)   //�ҹ�������
		{
			int temp2=	(R[area_info[k].area_y][area_info[k].area_x]-G[area_info[k].area_y][area_info[k].area_x])+
									(R[area_info[k].area_y][area_info[k].area_x]-B[area_info[k].area_y][area_info[k].area_x])-
							 abs(B[area_info[k].area_y][area_info[k].area_x]-G[area_info[k].area_y][area_info[k].area_x]);
			if(temp1<temp2){temp_num=k;  temp1=temp2;}        //�Ӻ�Խ��Խ��
		}
	}
	area_info[temp_num].color |= 0x04;    //0x04�����ɫ
	
	temp_num=1;								//����ɫ
	temp1=-100;
	for(k=1;k<area_num2;k++)       
	{
		if(area_info[k].acre != 0 /*&& area_info[k].color != 0*/)   //�ҹ�������
		{
			int temp2=	(G[area_info[k].area_y][area_info[k].area_x]-R[area_info[k].area_y][area_info[k].area_x])+
									(G[area_info[k].area_y][area_info[k].area_x]-B[area_info[k].area_y][area_info[k].area_x])-
							 abs(B[area_info[k].area_y][area_info[k].area_x]-R[area_info[k].area_y][area_info[k].area_x]);
			if(temp1<temp2){temp_num=k;  temp1=temp2;}         //�Ӻ�Խ��Խ��
		}
	}
	area_info[temp_num].color |= 0x08;    //0x08������ɫ
	
	temp_num=1;								//����ɫ
	temp1=-100;
	for(k=1;k<area_num2;k++)       
	{
		if(area_info[k].acre != 0 /*&& area_info[k].color != 0*/)   //�ҹ�������
		{
			int temp2=	(B[area_info[k].area_y][area_info[k].area_x]-R[area_info[k].area_y][area_info[k].area_x])+
									(B[area_info[k].area_y][area_info[k].area_x]-G[area_info[k].area_y][area_info[k].area_x])-
							 abs(R[area_info[k].area_y][area_info[k].area_x]-G[area_info[k].area_y][area_info[k].area_x]);
			if(temp1<temp2){temp_num=k;  temp1=temp2;}           //�Ӻ�Խ��Խ��
		}
	}
	area_info[temp_num].color |= 0x10;    //0x10������ɫ
	
	temp_num=1;								//�һ�ɫ
	temp1=-100;
	for(k=1;k<area_num2;k++)       
	{
		if(area_info[k].acre != 0 /*&& area_info[k].color != 0*/)   //�ҹ�������
		{
			int temp2=	(R[area_info[k].area_y][area_info[k].area_x]-B[area_info[k].area_y][area_info[k].area_x])+
									(G[area_info[k].area_y][area_info[k].area_x]-B[area_info[k].area_y][area_info[k].area_x])-
							 abs(R[area_info[k].area_y][area_info[k].area_x]-G[area_info[k].area_y][area_info[k].area_x]);
			if(temp1<temp2){temp_num=k;  temp1=temp2;}                 //�Ӻ�Խ��Խ��
		}
	}
	area_info[temp_num].color |= 0x20;    //0x20�����ɫ
	
	/*�Ա��ظ���ɫ������ȷ�����������ɫ*/
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
			for(i=1;i<=5;i++)     //����i������������
				if( (area_info[k].color & (0x01<<i)) !=0 )
					color_count++;
			if(color_count>1)		//˵��ĳһ���򱻸����˶�����ɫ
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
		
		u8 color_absent=0x3e,b;  //��û�й�������ɫ��ֵ
		for(k=1;k<area_num2;k++)
			color_absent ^= area_info[k].color;
		b=color_absent>>1<<7;        //��
		if(b)
		{
			for(k=1;k<area_num2;k++)
				if(area_info[k].color != 0 && R[area_info[k].area_y][area_info[k].area_x]<15 
																	 && G[area_info[k].area_y][area_info[k].area_x]<15 
																	 && B[area_info[k].area_y][area_info[k].area_x]<15 
					 &&area_info[k].acre != 0)
				
					area_info[k].color=0x02;
		}
		b=color_absent>>2<<7;         //��
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
	
	/*���ĺ�ɫԲ����Ҫ���򣬼�����Ƿ���� 1���ڸ�����2������������3��������Բ��*/
	u8 data_valid=1,K_num;       //1����Ч
	for(k=1;k<area_num2;k++)
		if(area_info[k].color == 0x02)   
			break;
	if(k==area_num2)           //û�ҵ���ɫ
		data_valid=0;
	else
	{
		K_num=k;
		for(k=1;k<area_num2 && data_valid!=0;k++)
			if(area_info[k].acre != 0 && area_info[k].color != 0 )
				if( area_info[k].acre > area_info[K_num].acre)    //��ɫ�����������
					data_valid=0;
	}
	if(area_info[K_num].Y_N==0)    //��ɫ����Բ
		data_valid=0;
	
	/*�����ɫ��������Ϣ*/
	if(data_valid!=0)
		
		data_output(k,area_num2,area_info,data_valid);  //���������������ʾ
//	img_display(Gray);
	
	return 1;
}


/***������0����ȷ������������+1����Ϊk�Ǵ�1��ʼ�ģ�*/
u8 area_grow(void)     //������������    
{ 
	u8 i,j,i_last,j_last;
	u16 n;
	int top=0;             //ջ��
	u8 area_num=1;         //����ɫ��������� ���1��ʼ
	u16 area_acre=0;       //��ɫ�������
	for(i=4;i<120;i+=10)
		for(j=1;j<117;j++)
			if(Gray[i][j] > 9 && Gray[i-1][j] < 0x80)    //������ڶ�����ֵ�������
			{
				i_last=i; j_last=j;
				top=0;
				Gray[i][j] = 0;    //���ӵ�
	//			data_process1();
				while(top!=-1)
				{
					/*��������8��Ԫ��*/
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
					/*����8��Ԫ�ر�����*/
					if(area_acre>1996 || top>998){    //̫���ˣ�����
						return 0;}
					/*ȡ��ջ��Ԫ��*/
					if(top>0)
					{
						i=grow_stack[top-1].y;
						j=grow_stack[top-1].x;
					}
					top--;
				}
				if(area_acre>20)             //��������㹻����Ч
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
 
/*roberts���ӱ�Ե���*/
void roberts(u8 in[120][120],int rows,int cols)   //��Ե��⺯��
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

void img_display(u8 img[120][120])  //�������ͼ��
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


void data_output(u16 k , u8 area_num2 , ImgPrcs_TypeDef area_info[20] , u8 data_valid)   //���������������ʾ
{
	if(data_valid!=0)
	{
		for(k=1;k<area_num2;k++)    //�����������Ϣ
		{
			if(area_info[k].acre != 0 && area_info[k].color != 0 )
			{
				if(area_info[k].color == 0x02)
					printf("��     :%3d ����X:%3d Y%3d\r\n",area_info[k].acre,area_info[k].area_x,area_info[k].area_y);
			}
		}
		for(k=1;k<area_num2;k++)    //�����������Ϣ
		{
			if(area_info[k].acre != 0 && area_info[k].color != 0 )
			{
				if(area_info[k].color == 0x04)
					printf("��     :%3d ����X:%3d Y%3d\r\n",area_info[k].acre,area_info[k].area_x,area_info[k].area_y);
			}
		}
		for(k=1;k<area_num2;k++)    //�����������Ϣ
		{
			if(area_info[k].acre != 0 && area_info[k].color != 0 )
			{
				if(area_info[k].color == 0x08)
					printf("�� ���:%3d ����X:%3d Y%3d\r\n",area_info[k].acre,area_info[k].area_x,area_info[k].area_y);
			}
		}
		for(k=1;k<area_num2;k++)    //�����������Ϣ
		{
			if(area_info[k].acre != 0 && area_info[k].color != 0 )
			{
				if(area_info[k].color == 0x10)
					printf("��     :%3d ����X:%3d Y%3d\r\n",area_info[k].acre,area_info[k].area_x,area_info[k].area_y);
			}
		}
		for(k=1;k<area_num2;k++)    //�����������Ϣ
		{
			if(area_info[k].acre != 0 && area_info[k].color != 0 )
			{
				if(area_info[k].color == 0x20)
					printf("��     :%3d ����X:%3d Y%3d\r\n",area_info[k].acre,area_info[k].area_x,area_info[k].area_y);
			}
		}
		printf("\r\n");	
	}
	else
			printf("��Ч\r\n\r\n");
}


