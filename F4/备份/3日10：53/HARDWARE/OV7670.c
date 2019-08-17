#include "sys.h"
#include "delay.h"
#include "OV7670.h"
#include "sccb.h"
#include "OV7670cfg.h"
#include "usart.h"
#include "dcmi.h" 

u8 rows_get;    //存储到哪一行
u8 rows_pros;   //处理到哪一行
u8 row[640];
u8 R[120][120];
u8 G[120][120];
u8 B[120][120];
u16 AREA[8][8][8];   //AERA(R,G,B),每个颜色区域的像素数量   不要用指针读取

struct bound      //各种颜色对应上下限
{
	u16 R_top;
	u16 R_bottom;
	u16 G_top;
	u16 G_bottom;
	u16 B_top;
	u16 B_bottom;
}black_bound , R_bound , G_bound , B_bound , Y_bound;

struct color_info 
	{	
		u32 x;
		u32 y;
		u16 count;
	}black_info , R_info , G_info , B_info ;

void data_process1(void)  //串口输出
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
			 USART_SendData(USART1, B[i][j]*8);
		 }
  }
	DCMI_Start();
}

void data_process2(void)   //寻找几种颜色的标准值
{
	u16 i,j,k;
	u16 AREA_MAX;
	u8 max_i=7,max_j=0,max_k=0;
	if(rows_pros<rows_get)
	{
		/****每次进入这里后大概有90us的可运行时间***/
		for(i=0;i<120;i++)
		{
			AREA[ R[rows_pros][i]/4 ][ G[rows_pros][i]/4 ][ B[rows_pros][i]/4 ]++;				
		}
		rows_pros++;
		if(rows_pros>=120)
		{
			disp_AREA();
			/***一帧读完进入这里的时候能有2.2ms可运行时间（保险2ms）****/
			rows_pros=0;
			for(i=1;i<7;i++)
			{
				if(AREA[i][i][i]-AREA[i-1][i-1][i-1]>400)   //对角线像素点数量相减，当前这个减前一个>400
					for(j=i;j<7;j++)
					{
						if( AREA[j][j][j]-AREA[j+1][j+1][j+1] > 100 )  //对角线像素点数量相减，当前这个减下一个>100 ， 此时说明“当前这个”是黑色区域对应色彩
						{
							black_bound.G_bottom=black_bound.B_bottom=black_bound.R_bottom=i*4;
							black_bound.G_top   =black_bound.B_top   =black_bound.R_top   =j*4+4;
							goto black_bound_get;
						}
					}
			}
			black_bound_get:;     	  //从向上第四行跳转来
//			printf("%d %d %d %d\r\n",i,j,black_bound.G_bottom,black_bound.G_top);
//			disp_AREA();
			
			/**红色部分****/
			AREA_MAX=AREA[7][0][0];
			max_i=7,max_j=0,max_k=0;
			for(i=7;i>3;i--)
				for(j=0;j<3;j++)
					for(k=0;k<3;k++)
					{
						if(i==4&&j==2&&i==2)
						{
							R_bound.R_bottom=4*max_i;
							R_bound.R_top=4*max_i+4;
							R_bound.G_bottom=4*max_j;
							R_bound.G_top=4*max_j+4;
							R_bound.B_bottom=4*max_k;
							R_bound.B_top=4*max_k+4;
							goto R_bound_get;
						}
						if(AREA_MAX<AREA[i][j][k])
						{
							AREA_MAX=AREA[i][j][k];
							max_i=i; max_j=j; max_k=k;
						}
					}
			R_bound_get:;
			
			/*处理计算各个颜色的面积和位置*/
//			data_process3();
			/****************************/
		}
	}
}
  
void data_process3(void)   //输出五种颜色相对画面的位置和面积
{
	u16 i,j;
	DCMI_Stop();
/*	struct color_info    //放到全局变量是为了初始化0
	{	
		u32 x;
		u32 y;
		u16 count;
	}black_info , R_info , G_info , B_info ;*/
	for(i=0;i<120;i++)
		for(j=0;j<120;j++)
		{
			if(R[i][j]>black_bound.R_bottom && R[i][j]<black_bound.R_top && 
					 G[i][j]>black_bound.G_bottom && G[i][j]<black_bound.G_top && 
					 B[i][j]>black_bound.B_bottom&&B[i][j]<black_bound.B_top )
			{
				black_info.count++;
				black_info.x+=j;
				black_info.y+=i;
			}
					
			if(R[i][j]>R_bound.R_bottom && R[i][j]<R_bound.R_top && 
					 G[i][j]>R_bound.G_bottom && G[i][j]<R_bound.G_top && 
					 B[i][j]>R_bound.B_bottom&&B[i][j]<R_bound.B_top )
			{
				R_info.count++;
				R_info.x+=j;
				R_info.y+=i;
			}
		}
		
	black_info.x = black_info.x/black_info.count;
	black_info.y = black_info.y/black_info.count;
	R_info.x = R_info.x/R_info.count;
	R_info.y = R_info.y/R_info.count;
	
	for(i=black_info.y-7;i<black_info.y+7;i++)
		for(j=black_info.x-7;j<black_info.x+7;j++)
			B[i][j]=0x1e;
	for(i=R_info.y-4;i<R_info.y+4;i++)
		for(j=R_info.x-4;j<R_info.x+4;j++)
			B[i][j]=0x1e;
//	data_process1();
	black_info.x=black_info.y=black_info.count=0;
	R_info.x=R_info.y=R_info.count=0;
//	printf("%d %d %d\r\n",balck_x,balck_y,balck_count);
}



void disp_AREA(void)    //显示那个8*8*8的像素数量阵列
{
	u16 i,j,k;
	DCMI_Stop();
	printf("NEW_FRAME\r\n");
	for(i=0;i<8;i++)
		for(j=0;j<8;j++)
			for(k=0;k<8;k++)
			{
				printf("%d ",AREA[i][j][k]);
				AREA[i][j][k]=0;
				if(k==7)
					printf("\r\n");
				if(j==7&&k==7)
					printf("\r\n");
			}
	rows_pros=0;
	DCMI_Start();
}


u8 OV7670_init(void)
{
	u8 temp;
	u16 i=0;	

	GPIO_Start();	
	
	delay_ms(100);
	
	DCMI_PWDN=0;
	DCMI_RST=1;
	delay_ms(10);
//	DCMI_RST=1;
	
 	SCCB_Init();        		//初始化SCCB 的IO口	   	  
 	if(SCCB_WR_Reg(0x12,0x80)) return 1;	//复位SCCB	  
	delay_ms(50); 
	//读取产品型号
 	temp=SCCB_RD_Reg(0x0b);   
	if(temp!=0x73)return 2;  
 	temp=SCCB_RD_Reg(0x0a);   
	if(temp!=0x76)return 2;
	//初始化序列	  
	for(i=0;i<sizeof(ov7670_init_reg_tbl)/sizeof(ov7670_init_reg_tbl[0]);i++)
	{
	  SCCB_WR_Reg(ov7670_init_reg_tbl[i][0],ov7670_init_reg_tbl[i][1]);
		delay_ms(2);
 	}
	OV7670_Light_Mode(0);	
	OV7670_Color_Saturation(4);
	OV7670_Window_Set(10,174,240,320);	//设置窗口	  //(10+120,174+160,120,160)偏移（80，60）
  return 0x00; 	//ok
} 

void OV7670_Light_Mode(u8 mode)
{
	u8 reg13val=0XE7;//默认就是设置为自动白平衡
	u8 reg01val=0;
	u8 reg02val=0;
	switch(mode)
	{
		case 1://sunny
			reg13val=0XE5;
			reg01val=0X5A;
			reg02val=0X5C;
			break;	
		case 2://cloudy
			reg13val=0XE5;
			reg01val=0X58;
			reg02val=0X60;
			break;	
		case 3://office
			reg13val=0XE5;
			reg01val=0X84;
			reg02val=0X4c;
			break;	
		case 4://home
			reg13val=0XE5;
			reg01val=0X96;
			reg02val=0X40;
			break;	
	}
	SCCB_WR_Reg(0X13,reg13val);//COM8设置 
	SCCB_WR_Reg(0X01,reg01val);//AWB蓝色通道增益 
	SCCB_WR_Reg(0X02,reg02val);//AWB红色通道增益 
}				  
//色度设置
//0:-2
//1:-1
//2,0
//3,1
//4,2
void OV7670_Color_Saturation(u8 sat)
{
	u8 reg4f5054val=0X80;//默认就是sat=2,即不调节色度的设置
 	u8 reg52val=0X22;
	u8 reg53val=0X5E;
 	switch(sat)
	{
		case 0://-2
			reg4f5054val=0X40;  	 
			reg52val=0X11;
			reg53val=0X2F;	 	 
			break;	
		case 1://-1
			reg4f5054val=0X66;	    
			reg52val=0X1B;
			reg53val=0X4B;	  
			break;	
		case 3://1
			reg4f5054val=0X99;	   
			reg52val=0X28;
			reg53val=0X71;	   
			break;	
		case 4://2
			reg4f5054val=0XC0;	   
			reg52val=0X33;
			reg53val=0X8D;	   
			break;	
	}
	SCCB_WR_Reg(0X4F,reg4f5054val);	//色彩矩阵系数1
	SCCB_WR_Reg(0X50,reg4f5054val);	//色彩矩阵系数2 
	SCCB_WR_Reg(0X51,0X00);			//色彩矩阵系数3  
	SCCB_WR_Reg(0X52,reg52val);		//色彩矩阵系数4 
	SCCB_WR_Reg(0X53,reg53val);		//色彩矩阵系数5 
	SCCB_WR_Reg(0X54,reg4f5054val);	//色彩矩阵系数6  
	SCCB_WR_Reg(0X58,0X9E);			//MTXS 
}
//亮度设置
//0:-2
//1:-1
//2,0
//3,1
//4,2
void OV7670_Brightness(u8 bright)
{
	u8 reg55val=0X00;//默认就是bright=2
  	switch(bright)
	{
		case 0://-2
			reg55val=0XB0;	 	 
			break;	
		case 1://-1
			reg55val=0X98;	 	 
			break;	
		case 3://1
			reg55val=0X18;	 	 
			break;	
		case 4://2
			reg55val=0X30;	 	 
			break;	
	}
	SCCB_WR_Reg(0X55,reg55val);	//亮度调节 
}
//对比度设置
//0:-2
//1:-1
//2,0
//3,1
//4,2
void OV7670_Contrast(u8 contrast)
{
	u8 reg56val=0X40;//默认就是contrast=2
  	switch(contrast)
	{
		case 0://-2
			reg56val=0X30;	 	 
			break;	
		case 1://-1
			reg56val=0X38;	 	 
			break;	
		case 3://1
			reg56val=0X50;	 	 
			break;	
		case 4://2
			reg56val=0X60;	 	 
			break;	
	}
	SCCB_WR_Reg(0X56,reg56val);	//对比度调节 
}
//特效设置
//0:普通模式    
//1,负片
//2,黑白   
//3,偏红色
//4,偏绿色
//5,偏蓝色
//6,复古	    
void OV7670_Special_Effects(u8 eft)
{
	u8 reg3aval=0X04;//默认为普通模式
	u8 reg67val=0XC0;
	u8 reg68val=0X80;
	switch(eft)
	{
		case 1://负片
			reg3aval=0X24;
			reg67val=0X80;
			reg68val=0X80;
			break;	
		case 2://黑白
			reg3aval=0X14;
			reg67val=0X80;
			reg68val=0X80;
			break;	
		case 3://偏红色
			reg3aval=0X14;
			reg67val=0Xc0;
			reg68val=0X80;
			break;	
		case 4://偏绿色
			reg3aval=0X14;
			reg67val=0X40;
			reg68val=0X40;
			break;	
		case 5://偏蓝色
			reg3aval=0X14;
			reg67val=0X80;
			reg68val=0XC0;
			break;	
		case 6://复古
			reg3aval=0X14;
			reg67val=0XA0;
			reg68val=0X40;
			break;	 
	}
	SCCB_WR_Reg(0X3A,reg3aval);//TSLB设置 
	SCCB_WR_Reg(0X68,reg67val);//MANU,手动U值 
	SCCB_WR_Reg(0X67,reg68val);//MANV,手动V值 
}	
//设置图像输出窗口
//对QVGA设置。
void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height)
{
	u16 endx;
	u16 endy;
	u8 temp; 
	endx=sx+width*2;	//V*2
 	endy=sy+height*2;
	if(endy>784)endy-=784;
	temp=SCCB_RD_Reg(0X03);				//读取Vref之前的值
	temp&=0XF0;
	temp|=((endx&0X03)<<2)|(sx&0X03);
	SCCB_WR_Reg(0X03,temp);				//设置Vref的start和end的最低2位
	SCCB_WR_Reg(0X19,sx>>2);			//设置Vref的start高8位
	SCCB_WR_Reg(0X1A,endx>>2);			//设置Vref的end的高8位

	temp=SCCB_RD_Reg(0X32);				//读取Href之前的值
	temp&=0XC0;
	temp|=((endy&0X07)<<3)|(sy&0X07);
	SCCB_WR_Reg(0X17,sy>>3);			//设置Href的start高8位
	SCCB_WR_Reg(0X18,endy>>3);			//设置Href的end的高8位
}

void GPIO_Start(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*****时钟输出设置******/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_MCO); 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	
//	RCC_MCO1Config(RCC_MCO1Source_PLLCLK,RCC_MCO1Div_5);
	RCC_MCO1Config(RCC_MCO1Source_HSE,RCC_MCO1Div_1);
	
	/****PWDN和RST设置******/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;//PD3:PWDN 0退出掉电； PD4:RST  下拉再上拉完成复位 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}



/********************************
寄存器0x12
值0x14  QVGA, RGB565
值0x10  YUV,
********************************/



