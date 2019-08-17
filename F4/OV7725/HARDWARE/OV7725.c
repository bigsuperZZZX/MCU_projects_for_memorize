#include "OV7725.h"
#include "sys.h"
#include "delay.h"
#include "sccb.h"
#include "usart.h"
#include "dcmi.h" 
#include "OV7725_REG.h"

const unsigned char ov7725_init_reg_tbl[][2] =  
{
	{OV7725_CLKRC,     0x01}, /*clock config*/
	{OV7725_COM7,      0x4a}, /*QVGA RGB565 */
  {OV7725_HSTART,    0x3f},
	{OV7725_HSIZE,     0x50},
	{OV7725_VSTRT,     0x03},
	{OV7725_VSIZE,     0x78},
	{OV7725_HREF,      0x00},
	{OV7725_HOutSize,  0x50},
	{OV7725_VOutSize,  0x78},
	{OV7725_EXHCH,     0x00},

	/*DSP control*/
	{OV7725_TGT_B,     0x7f},
	{OV7725_FixGain,   0x09},
	{OV7725_AWB_Ctrl0, 0xe0},
	{OV7725_DSP_Ctrl1, 0xff},
	{OV7725_DSP_Ctrl2, 0x20},
	{OV7725_DSP_Ctrl3,	0x00},
	{OV7725_DSP_Ctrl4, 0x00},

	/*AGC AEC AWB*/
	{OV7725_COM8,		  0xf0},
	{OV7725_COM4,		  0x01}, /*Pll AEC CONFIG*/
	{OV7725_COM6,		  0xc5},
	{OV7725_COM9,		  0x21},
	{OV7725_BDBase,	  0xFF},
	{OV7725_BDMStep,	  0x01},
	{OV7725_AEW,		    0x34},
	{OV7725_AEB,		    0x3c},
	{OV7725_VPT,		    0xa1},
	{OV7725_EXHCL,		  0x00},
	{OV7725_AWBCtrl3,  0xaa},
	{OV7725_COM8,		  0xff},
	{OV7725_AWBCtrl1,  0x5d},

	{OV7725_EDGE1,		  0x0a},
	{OV7725_DNSOff,	  0x01},
	{OV7725_EDGE2,		  0x01},
	{OV7725_EDGE3,		  0x01},

	{OV7725_MTX1,		  0x5f},
	{OV7725_MTX2,		  0x53},
	{OV7725_MTX3,		  0x11},
	{OV7725_MTX4,		  0x1a},
	{OV7725_MTX5,		  0x3d},
	{OV7725_MTX6,		  0x5a},
	{OV7725_MTX_Ctrl,  0x1e},

//	{OV7725_BRIGHT,	  0x00}, //亮度
	{OV7725_BRIGHT,	  0x38},//亮度
	{0xab , 0x06},          //亮度
	{OV7725_CNST,		  0x2c},
//	{OV7725_CNST,		  0x25},  //对比度
/*	{OV7725_USAT,		  0x65}, //色彩饱和度
	{OV7725_VSAT,		  0x65}, */
	{OV7725_USAT,		  0x80}, //色彩饱和度
	{OV7725_VSAT,		  0x80},
	{OV7725_UVADJ0,	  0x71},
	{OV7725_SDE,		    0x06},
	
    /*GAMMA config*/
	{OV7725_GAM1,		  0x0c},
	{OV7725_GAM2,		  0x16},
	{OV7725_GAM3,		  0x2a},
	{OV7725_GAM4,		  0x4e},
	{OV7725_GAM5,		  0x61},
	{OV7725_GAM6,		  0x6f},
	{OV7725_GAM7,		  0x7b},
	{OV7725_GAM8,		  0x86},
	{OV7725_GAM9,		  0x8e},
	{OV7725_GAM10,		  0x97},
	{OV7725_GAM11,		  0xa4},
	{OV7725_GAM12,		  0xaf},
	{OV7725_GAM13,		  0xc5},
	{OV7725_GAM14,		  0xd7},
	{OV7725_GAM15,		  0xe8},
	{OV7725_SLOP,		  0x20},

	{OV7725_HUECOS,	  0x80},
	{OV7725_HUESIN,	  0x80},
	{OV7725_DSPAuto,	  0xff},
	{OV7725_DM_LNL,	  0x00},
	{OV7725_BDBase,	  0x99},
	{OV7725_BDMStep,	  0x03},
	{OV7725_LC_RADI,	  0x00},
	{OV7725_LC_COEF,	  0x13},
	{OV7725_LC_XC,		  0x08},
	{OV7725_LC_COEFB,  0x14},
	{OV7725_LC_COEFR,  0x17},
	{OV7725_LC_CTR,	  0x05},
	
	{OV7725_COM3,		  0xd0},/*Horizontal mirror image*/

	/*night mode auto frame rate control*/
	//{OV7725_COM5,		0xf5},	 /*在夜视环境下，自动降低帧率，保证低照度画面质量*/
	{OV7725_COM5,		0x31},	/*夜视环境帧率不变*/
	
};


/*正确返回0*/
u8 OV7725_init(void)
{
	u8 temp;
	u16 i=0;	

	GPIO_Start();	
	
	delay_ms(100);
	
	delay_ms(10);
//	DCMI_RST=1;
	
 	SCCB_Init();        		//初始化SCCB 的IO口	   	  
 	if(SCCB_WR_Reg(OV7725_COM7,0x80)) return 1;	//复位SCCB	  
	delay_ms(50); 
	//读取产品型号
 	temp=SCCB_RD_Reg(OV7725_PID);   
	if(temp!=0x77)return 2;  
 	temp=SCCB_RD_Reg(OV7725_VER);   
	if(temp!=0x21)return 2;
	//初始化序列	  
	for(i=0;i<sizeof(ov7725_init_reg_tbl)/sizeof(ov7725_init_reg_tbl[0]);i++)
	{
		u8 count=0;
		while( SCCB_WR_Reg(ov7725_init_reg_tbl[i][0],ov7725_init_reg_tbl[i][1]) !=0 ) 
		{
			if(count++ == 100)
				return 3;
		}
		delay_ms(2);
 	}
//	OV7670_Window_Set(10,174,240,320);	//设置窗口	  //(10+120,174+160,120,160)偏移（80，60）
  return 0x00; 	//ok
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
	
	RCC_MCO1Config(RCC_MCO1Source_PLLCLK,RCC_MCO1Div_4);
//	RCC_MCO1Config(RCC_MCO1Source_HSE,RCC_MCO1Div_1);
	
	/****PWDN和RST设置******/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;//PD3:PWDN 0退出掉电； PD4:RST  下拉再上拉完成复位 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	DCMI_PWDN=0; //PD3
	DCMI_RST=1;  //PD4
}

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

