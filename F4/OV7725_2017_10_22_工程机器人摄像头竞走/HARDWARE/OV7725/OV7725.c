#include "OV7725.h"
#include "sys.h"
#include "delay.h"
#include "sccb.h"
#include "usart.h"
#include "dcmi.h" 
#include "OV7725_REG.h"
#include "stm32f4xx_tim.h" 

const unsigned char ov7725_init_reg_tbl[][2] =  
{
	{OV7725_CLKRC,     0x00}, //clock config  //0x11
	{OV7725_COM7,      0x46}, //0x46  QVGA RGB565   // 0x44 QVGA YUV 
  {OV7725_HSTART,    0x3f},
	{OV7725_HSIZE,     0x50},
	{OV7725_VSTRT,     0x03},
	{OV7725_VSIZE,     0x78},
	{OV7725_HREF,      0x00},
	{OV7725_HOutSize,  0x50},
	{OV7725_VOutSize,  0x78},
	{OV7725_EXHCH,     0x00},

	//DSP control
	{OV7725_TGT_B,     0x7f},
	{OV7725_FixGain,   0x09},
	{OV7725_AWB_Ctrl0, 0xe0},
	{OV7725_DSP_Ctrl1, 0xff},
	{OV7725_DSP_Ctrl2, 0x20},
	{OV7725_DSP_Ctrl3,	0x00},
	{OV7725_DSP_Ctrl4, 0x00},

	//AGC AEC AWB
	{OV7725_COM8,		  0xf0},
	{OV7725_COM4,		  0x01}, //Pll AEC CONFIG  0x0d
	{OV7725_COM6,		  0xc5},
	{OV7725_COM9,		  0x31},
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
//	{OV7725_USAT,		  0x65}, //色彩饱和度
//	{OV7725_VSAT,		  0x65}, 
	{OV7725_USAT,		  0x80}, //色彩饱和度
	{OV7725_VSAT,		  0x80},
	{OV7725_UVADJ0,	  0x71},
	{OV7725_SDE,		    0x06},
	
    //GAMMA config
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
	{OV7725_DM_LNL,	  0x20},  //0x33  插入空像素
	{OV7725_BDBase,	  0x99},
	{OV7725_BDMStep,	  0x03},
	{OV7725_LC_RADI,	  0x00},
	{OV7725_LC_COEF,	  0x13},
	{OV7725_LC_XC,		  0x08},
	{OV7725_LC_COEFB,  0x14},
	{OV7725_LC_COEFR,  0x17},
	{OV7725_LC_CTR,	  0x05},
	
	
//	{OV7725_COM3,		  0x41},//输出彩条
	{OV7725_COM3,		  0x40},//不输出彩条

	//night mode auto frame rate control
	//{OV7725_COM5,		0xf5},	 //在夜视环境下，自动降低帧率，保证低照度画面质量
	{OV7725_COM5,		0x11},	//夜视环境帧率不变
	
	
	/*{0x32,0x00},
	{0x2a,0x00},
	{0x11,0x03},
	{0x12,0x46},//QVGA RGB565
	
	{0x42,0x7f},
	{0x4d,0x00},
	{0x63,0xf0},
	{0x64,0x1f},
	{0x65,0x20},
	{0x66,0x00},
	{0x67,0x00},
	{0x69,0x50},  
	
	
	{0x13,0xff},
	{0x0d,0x41},
	{0x0f,0x01},
	{0x14,0x06},
	
	{0x24,0x75},
	{0x25,0x63},
	{0x26,0xd1},
	{0x2b,0xff},
	{0x6b,0xaa},
	
	{0x8e,0x10},
	{0x8f,0x00},
	{0x90,0x00},
	{0x91,0x00},
	{0x92,0x00},
	{0x93,0x00},
	
	{0x94,0x2c},
	{0x95,0x24},
	{0x96,0x08},
	{0x97,0x14},
	{0x98,0x24},
	{0x99,0x38},
	{0x9a,0x9e},
	{0x15,0x00}, 
	{0x9b,0x00},
	{0x9c,0x20},
	{0xa7,0x40},  
	{0xa8,0x40},
	{0xa9,0x80},
	{0xaa,0x80},
	
	{0x9e,0x81},
	{0xa6,0x06},
	
	{0x7e,0x0c},
	{0x7f,0x16},
	{0x80,0x2a},
	{0x81,0x4e},
	{0x82,0x61},
	{0x83,0x6f},
	{0x84,0x7b},
	{0x85,0x86},
	{0x86,0x8e},
	{0x87,0x97},
	{0x88,0xa4},
	{0x89,0xaf},
	{0x8a,0xc5},
	{0x8b,0xd7},
	{0x8c,0xe8},
	{0x8d,0x20},
	
	{0x33,0x40},
	{0x34,0x00},
	{0x22,0xaf},
	{0x23,0x01},
	
	{0x49,0x10},
	{0x4a,0x10},
	{0x4b,0x14},
	{0x4c,0x17},
	{0x46,0x05},
	
	{0x47,0x08},
	{0x0e,0x01},
	{0x0c,0x60},
	{0x09,0x03},
	
	{0x29,0x50},
	{0x2C,0x78},
	
	{0x00,0x60}, */
	
};


/*--------------------------White Balance----------------------*/
void set_wb_sunny()
{
	SCCB_WR_Reg(0x13, 0xfd); //AWB off
  SCCB_WR_Reg(0x01, 0x5a);
  SCCB_WR_Reg(0x02, 0x5c);
  SCCB_WR_Reg(0x0e, 0x65); 
  SCCB_WR_Reg(0x2d, 0x00);
  SCCB_WR_Reg(0x2e, 0x00);
}

void set_wb_cloudy()
{
  SCCB_WR_Reg(0x13, 0xfd); //AWB off
  SCCB_WR_Reg(0x01, 0x58);
  SCCB_WR_Reg(0x02, 0x60);
  SCCB_WR_Reg(0x0e, 0x65); 
  SCCB_WR_Reg(0x2d, 0x00);
  SCCB_WR_Reg(0x2e, 0x00);
}

void set_wb_office()
{
  SCCB_WR_Reg(0x13, 0xfd); //AWB off
  SCCB_WR_Reg(0x01, 0x84);
  SCCB_WR_Reg(0x02, 0x4c);
  SCCB_WR_Reg(0x0e, 0x65); 
  SCCB_WR_Reg(0x2d, 0x00);
}

void set_wb_home()
{
	SCCB_WR_Reg(0x13, 0xfd); //AWB off
  SCCB_WR_Reg(0x01, 0x96);
  SCCB_WR_Reg(0x02, 0x40);
  SCCB_WR_Reg(0x0e, 0x65); 
  SCCB_WR_Reg(0x2d, 0x00);
  SCCB_WR_Reg(0x2e, 0x00);
}

void set_wb_night()
{
	SCCB_WR_Reg(0x13, 0xff); //AWB on
  SCCB_WR_Reg(0x0e, 0xe5); 
}
/*-----------------------Saturation-----------------------*/
void set_saturation(int n)
{
	SCCB_WR_Reg(0xa7, 0x40+n*0x10);
  SCCB_WR_Reg(0xa8, 0x40+n*0x10);
}
/*-----------------------Brightness-----------------------*/
void set_brightness_plus(int n)
{
	SCCB_WR_Reg(0x9b, 0x08+n*0x10);
  SCCB_WR_Reg(0xab, 0x06);
}

void set_brightness_minus(int n)
{
	SCCB_WR_Reg(0x9b, 0x08+(-1*n)*0x10);
  SCCB_WR_Reg(0xab, 0x0e);
}


/*正确返回0*/
u8 OV7725_init(void)
{
	u8 temp;
	u16 i=0;	

	GPIO_Start();	
	
	delay_ms(100);
	
	delay_ms(10);
//	DCMI_RST=1;
	
 	SCCB_Init();        		//初始化SCCB 的IO口	    IIC	  
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
	delay_ms(2);
	set_saturation(6);
	delay_ms(2);
	set_brightness_plus(0);
	delay_ms(2);
//	OV7670_Window_Set(10,174,240,320);	//设置窗口	  //(10+120,174+160,120,160)偏移（80，60）
  return 0x00; 	//ok
} 


void TIM14_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);  	//TIM14时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 	//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); //GPIOF9复用为定时器14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOF,&GPIO_InitStructure);              //初始化PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseStructure);//初始化定时器14
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1

	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
 
  TIM_ARRPreloadConfig(TIM14,ENABLE);//ARPE使能 
	
	TIM_Cmd(TIM14, ENABLE);  //使能TIM14
	TIM_SetCompare1(TIM14,arr/2);
										  
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
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_2;//PD3:PWDN 0退出掉电； PD2:RST  下拉再上拉完成复位 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	
	
//	TIM14_PWM_Init(10,4);
	
	
	
	DCMI_PWDN=0; //PD3
	DCMI_RST=1;  //PD2
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

