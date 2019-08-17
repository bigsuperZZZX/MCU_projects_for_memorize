#include "sys.h"
#include "ov7670.h"
#include "ov7670cfg.h"
#include "timer.h"	  
#include "delay.h"
#include "usart.h"			 
#include "sccb.h"	
#include "exti.h"
#include "lcd.h"

extern u8 ov_sta;	//在exit.c里面定义	
u8 in[38400],out[38400];   //边缘检测的时候用
short cen_x=0,cen_y=0;
		    
//初始化OV7670
//返回0:成功
//返回其他值:错误代码
u8 OV7670_Init(void)
{
	u8 temp;
	u16 i=0;	  
	//设置IO
	RCC->AHB1ENR|=1<<0;		//先使能外设PORTA时钟
  	RCC->AHB1ENR|=1<<2;		//先使能外设PORTC时钟
	RCC->AHB1ENR|=1<<4;		//先使能外设PORTE时钟	   
	RCC->AHB1ENR|=1<<5;		//先使能外设PORTF时钟
	
	GPIO_Set(GPIOF,PIN9,GPIO_MODE_IN,0,0,GPIO_PUPD_PD); 			//PF9设置为下拉输入 
	GPIOF->ODR|=1<<9; 
	
	GPIO_Set(GPIOC,PIN1|PIN3,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PC1 3设置输出
	GPIOC->ODR|=5<<1; 
	
	GPIO_Set(GPIOF,PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN6|PIN7,GPIO_MODE_IN,0,0,GPIO_PUPD_PD); 			//PF0 7设置为下拉输入 	
	GPIOF->ODR|=0x00ff; 
	
	GPIO_Set(GPIOE,PIN5|PIN3,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PE3 5设置输出
 	GPIOE->ODR|=5<<3;
	
	GPIO_Set(GPIOC,PIN13,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PC13设置输出
	GPIOC->ODR=1<<13;      


 	SCCB_Init();        		//初始化SCCB 的IO口	   	  
 	if(SCCB_WR_Reg(0x12,0x80))return 1;	//复位SCCB	  
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
	OV7670_Window_Set(10,174,240,320);	//设置窗口	
  OV7670_CS=0;	
   	return 0x00; 	//ok
} 


void roberts(u8 in[38400],u8 out[38400],int rows,int cols)   //边缘检测函数
{
	int h,q,v,i,j;
	for(i=0;i<rows-1;i++)
	{
		for(j=0;j<cols-1;j++)
		{
			h=*(in+cols*i+j)-*(in+cols*(i+1)+j+1);
			v=*(in+cols*i+j+1)-*(in+cols*(i+1)+j);
			q=abs(h)+abs(v);
			if(q>255) q=255;
			*(out+cols*i+j)=q;
		}
		*(out+cols*i+cols-1)=*(in+cols*i+cols-1);
	}
}


 void camera_convert_display(void)   //边缘检测完整读取+显示，YUV输出
{
	u16 Gray;
	u32 j;
// 	u16 color;
	
	u16 count=0;
	long sum_x=0,sum_y=0;  //找中心点
	
	if(ov_sta==2)
		{
			EXTI->IMR&=~(1<<9);	 //禁止外部中断，准备从FIFO中取数据
			EXTI->EMR&=~(1<<9);
			LCD_Scan_Dir(L2R_U2D);		//从上到下,从左到右 
			LCD_SetCursor(0x00,0x0000);	//设置光标位置 
			LCD_WriteRAM_Prepare();     //开始写入GRAM	
			OV7670_RRST=0;				//开始复位读指针 
			OV7670_RCK=0;
			OV7670_RCK=1;
			OV7670_RCK=0;
			OV7670_RRST=1;				//复位读指针结束 
			OV7670_RCK=1;  
			
			for(j=0;j<76800;j++)
			{
				OV7670_RCK=0;
				if(j%2==0)
					*(in+j/2)=GPIOF->IDR&0XFF;	//读数据
				OV7670_RCK=1; 
				OV7670_RCK=0;
				OV7670_RCK=1;
			}
			
			/******************/
			roberts(in,out,240,160);
			
			for(j=0;j<38400;j++)    //图像处理循环
			{
				
				if((*(out+j)>30)&&((j%160)<150)&&((j%160)>10)&&((j/160)<225)&&((j/160)>15))  //(*(out+j)>10)要看情况调高，如果灰色的话噪点很多
				{
					sum_x+=j%160;  //x输出的时候还要乘2，因为横向只有160列
					sum_y+=j/160;
					count++;
				}
			}
			cen_x=sum_x*2/count;
			cen_y=sum_y/count;
			/********************/
			
			
			for(j=0;j<76800;j++)   //图像显示循环
			{
				Gray=(((*(out+j/2))/8)<<11) + (((*(out+j/2))/4)<<5) + ((*(out+j/2))/8);
				LCD->LCD_RAM=Gray;
			}
			LCD_Draw_Circle(cen_x,cen_y,9);
			//LCD_Fill(cen_x-5,cen_y-5,cen_x+5,cen_y+5,RED);
			
			EXTI->PR=1<<9;     			//清除LINE9上的中断标志位
			ov_sta=0;					//开始下一次采集 
			EXTI->IMR|=(1<<9);	  //允许外部中断，以便接收下帧图像数据
			EXTI->EMR|=(1<<9);
		}
}	

 /*void camera_convert_display(void)
{
	u16 Buffer[10][10],R,G,B,Gray;
	u32 j;
 	u16 color;	
	if(ov_sta==2)
		{
			EXTI->IMR&=~(1<<9);	 //禁止外部中断，准备从FIFO中取数据
			EXTI->EMR&=~(1<<9);
			LCD_Scan_Dir(L2R_U2D);		
			LCD_SetCursor(0x00,0x0000);	//设置光标位置 
			LCD_WriteRAM_Prepare();     //开始写入GRAM	
			OV7670_RRST=0;				//开始复位读指针 
			OV7670_RCK=0;
			OV7670_RCK=1;
			OV7670_RCK=0;
			OV7670_RRST=1;				//复位读指针结束 
			OV7670_RCK=1;  
			
			for(j=0;j<76800;j++)
			{
				OV7670_RCK=0;
				color=GPIOF->IDR&0XFF;	//读数据
				OV7670_RCK=1; 
				color<<=8;        //YUV读取Y时注释掉
				OV7670_RCK=0;
		  	color|=GPIOF->IDR&0XFF;	//YUV读取Y时注释掉
				OV7670_RCK=1;
			//	color&=0x001f;   //通道蓝
			//	color&=0x07e0;   //通道绿
			//	color&=0xf800;   //通道红
			//  color&=0xffe0;	 //滤掉蓝
			//  color&=0xf81f;	 //滤掉绿
			//  color&=0x07e0;	 //滤掉红		
			//	if(color>80)
			//		color=255;
			//	Gray=((color/8)<<11) + ((color/4)<<5) + (color/8) ;
			//	LCD->LCD_RAM=Gray;
				LCD->LCD_RAM=color; 
			}
			LCD_Draw_Circle(160,120,5);
			EXTI->PR=1<<9;     			//清除LINE9上的中断标志位
			ov_sta=0;					//开始下一次采集 
			EXTI->IMR|=(1<<9);	  //允许外部中断，以便接收下帧图像数据
			EXTI->EMR|=(1<<9);
		}
}	
*/

////////////////////////////////////////////////////////////////////////////
//OV7670功能设置
//白平衡设置
//0:自动
//1:太阳sunny
//2,阴天cloudy
//3,办公室office
//4,家里home
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



/********************************
寄存器0x12
值0x14  QVGA, RGB565
值0x10  YUV,


寄存器里有关于YUV参数设置的说明 
采集到数据后 
R= (data & 0xF800) >> 8; 
G= (data& 0x07E0) >> 3;  
B= (data & 0x001F) << 3;  
Gray=(u16)(int)((R*77+G*150+B*29+128)/256); 
我是用这个公式计算灰度的 
我一般是隔8行采集
**********************************/

























