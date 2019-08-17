#include "sys.h"
#include "dcmi.h" 
#include "ov7725.h" 
#include "usart.h"
#include "delay.h"
#include "imgprcs.h"
#include "lcd.h"
#include "EasyTracer.h"
#include "touch.h" 

extern COLOR_HSL Gray2[240][240];
u16 row[320];
u16 img[240*240]; 
u16 *img_p = img;
/***************目标特征 = {Hmin,Hmax,Smin,Smax,Lmin,Lmax,WidMin ,HeiMin,WidMax  ,HeiMax    }; */
TARGET_CONDI condition  = {120 ,130 ,200  ,250 ,100  ,200 ,  3   , 3   ,   230  ,   230    }; //green
RESULT result;

/*u8 img1_4[160*120];
u8* pimg1_4 = img1_4;*/

u8 rows_count = 0;

//DCMI中断服务函数
void DCMI_IRQHandler(void)
{
	u8 flag;
	u8 fit_count = 0;
	static u8 set_flag = 0;
	static u16 x=120,y=120,x_last,y_last,x_temp,y_temp,sum_check;
	signed int i,j;
	u32 sum_x = 0 , sum_y = 0, sum_H = 0, sum_count = 0;
	static COLOR_RGB pix_RGB;
	static COLOR_HSL pix_HSL;
	
	
	if(DCMI_GetITStatus(DCMI_IT_FRAME)==SET)//捕获到一帧图像  320*240
	{
		DCMI_Stop();
		DCMI_ClearITPendingBit(DCMI_IT_FRAME);//清除帧中断
		rows_count = 0;
		/***************编程区域******************/
		PCout(4)=1;
		
		if(TP_Read_XY2(&x ,&y))
		{
			if(y > 239) y=239;
			pix_RGB.red   =	 (unsigned char)((img[y*240+x]&0xf800)>>8);
			pix_RGB.green =	 (unsigned char)((img[y*240+x]&0x07e0)>>3);
			pix_RGB.blue  =     (unsigned char)((img[y*240+x]&0x001f)<<3);
			RGBtoHSL(&pix_RGB, &pix_HSL);
			TP_Drow_Touch_Point(x,y,RED);	//画点2
			LCD_ShowNum(0,240,pix_HSL.hue,3,16);	
			LCD_ShowNum(32,240,pix_HSL.saturation,3,16);	
			LCD_ShowNum(64,240,pix_HSL.luminance,3,16);	
			condition.H_MIN = (pix_HSL.hue - 15) <0 ? 0 :(pix_HSL.hue - 15);
			condition.H_MAX = (pix_HSL.hue + 15) >255 ? 255 :(pix_HSL.hue + 15);
			condition.S_MIN = (pix_HSL.saturation - 50) <0 ? 0 :(pix_HSL.saturation - 50);
			condition.S_MAX = (pix_HSL.saturation + 50) >255 ? 255 :(pix_HSL.saturation + 50);
			condition.L_MIN = (pix_HSL.luminance - 80) <0 ? 0 : (pix_HSL.luminance - 80) ;
			condition.L_MAX = (pix_HSL.luminance + 80) >255 ? 255 : (pix_HSL.luminance + 80) ;
			LCD_ShowNum(0,260,condition.H_MIN,3,16);	
			LCD_ShowNum(32,260,condition.H_MAX,3,16);	
			LCD_ShowNum(64,260,condition.S_MIN,3,16);	
			LCD_ShowNum(96,260,condition.S_MAX,3,16);	
			LCD_ShowNum(128,260,condition.L_MIN,3,16);	
			LCD_ShowNum(160,260,condition.L_MAX,3,16);	
		}
		else
		{
			for(y=0;y<240;y+=2)
			{
				for(x=0;x<240;x+=2)
				{
					pix_RGB.red   =	 (unsigned char)((img[y*240+x]&0xf800)>>8);
					pix_RGB.green =	 (unsigned char)((img[y*240+x]&0x07e0)>>3);
					pix_RGB.blue  =     (unsigned char)((img[y*240+x]&0x001f)<<3);
					RGBtoHSL(&pix_RGB, &pix_HSL);
					if(pix_HSL.hue > condition.H_MIN && pix_HSL.hue < condition.H_MAX &&
						 pix_HSL.saturation > condition.S_MIN && pix_HSL.saturation < condition.S_MAX)
					{
						sum_x += x;
						sum_y += y;
						sum_H += pix_HSL.hue;
						sum_count++;
						img[y*240+x] = 0x0000;
					}
				}
			}
			x = sum_x/sum_count;
			y = sum_y/sum_count;
			sum_count = 0;
			
			x_temp = x;
			y_temp = y;
			if(x<16) x_temp=16;
			if(x>238) x_temp=238;
			if(x_temp%2) x_temp += 1;
			if(y<16) y_temp=16;
			if(y>238) y_temp=238;
			if(y_temp%2) y_temp += 1;
			for(i=-16;i<=16;i+=2)
			{
				for(j=-16;j<=16;j+=2)
				{
					if(img[(i+y_temp)*240+x_temp+j] == 0x0000)
					{
						sum_count ++;
					}
				}
			}
			if(sum_count < 100)
			{
				x = x_last;
				y = y_last;
			}
			/*pix_RGB.red   =	 (unsigned char)((img[y*240+x+4]&0xf800)>>8);
			pix_RGB.green =	 (unsigned char)((img[y*240+x+4]&0x07e0)>>3);
			pix_RGB.blue  =     (unsigned char)((img[y*240+x+4]&0x001f)<<3);
			RGBtoHSL(&pix_RGB, &pix_HSL);
			if(pix_HSL.hue > condition.H_MIN && pix_HSL.hue < condition.H_MAX) 
				fit_count++;
			pix_RGB.red   =	 (unsigned char)((img[y*240+x-4]&0xf800)>>8);
			pix_RGB.green =	 (unsigned char)((img[y*240+x-4]&0x07e0)>>3);
			pix_RGB.blue  =     (unsigned char)((img[y*240+x-4]&0x001f)<<3);
			RGBtoHSL(&pix_RGB, &pix_HSL);
			if(pix_HSL.hue > condition.H_MIN && pix_HSL.hue < condition.H_MAX) 
				fit_count++;
			pix_RGB.red   =	 (unsigned char)((img[(y+4)*240+x]&0xf800)>>8);
			pix_RGB.green =	 (unsigned char)((img[(y+4)*240+x]&0x07e0)>>3);
			pix_RGB.blue  =     (unsigned char)((img[(y+4)*240+x]&0x001f)<<3);
			RGBtoHSL(&pix_RGB, &pix_HSL);
			if(pix_HSL.hue > condition.H_MIN && pix_HSL.hue < condition.H_MAX) 
				fit_count++;
			pix_RGB.red   =	 (unsigned char)((img[(y-4)*240+x]&0xf800)>>8);
			pix_RGB.green =	 (unsigned char)((img[(y-4)*240+x]&0x07e0)>>3);
			pix_RGB.blue  =     (unsigned char)((img[(y-4)*240+x]&0x001f)<<3);
			RGBtoHSL(&pix_RGB, &pix_HSL);
			if(pix_HSL.hue > condition.H_MIN && pix_HSL.hue < condition.H_MAX) 
				fit_count++;
			
			if(fit_count<4)
			{
				x = x_last;
				y = y_last;
			}*/
			
			x_last = x;
			y_last = y;
			
			y = 240- y ;
			
			USART_SendData(USART1,0xA0);
			while((USART1->SR&0X40)==0);
			delay_us(100);
			USART_SendData(USART1,y/2);
			while((USART1->SR&0X40)==0);
			delay_us(100);
			USART_SendData(USART1,x/2);
			while((USART1->SR&0X40)==0);
			delay_us(100);
		
			sum_check=(x+y)/2&0xff;
			
			USART_SendData(USART1,sum_check);
			while((USART1->SR&0X40)==0);
			
			TP_Drow_Touch_Point(x,240-y,RED);	//画点2
			LCD_ShowNum(0,280,x,3,16);	
			LCD_ShowNum(32,280,240-y,3,16);	
		}
		
		LCD_Color_Fill(0,0,239,239,img);
		
		
		
			
		/*	set_flag = 1;
			condition.x_init = x;
			condition.y_init = y;
			condition.flag_temp = 1;
			Trace(&condition , &result);
		}
		else if(set_flag == 1)
		{
			flag = Trace(&condition , &result);
			if(flag == 1)
			{
				//LCD_DrawRectangle(2*result.x - result.w,2*result.y - result.h,2*result.x + result.w,2*result.y - result.h);
				TP_Drow_Touch_Point(result.x,result.y,RED);	//画点2
				
			pix_RGB.red   =	 ((unsigned char)((img[result.y*240+result.x]&0xf800)>>8) + pix_RGB.red)/2;
			pix_RGB.green =	 ((unsigned char)((img[result.y*240+result.x]&0x07e0)>>3) + pix_RGB.green)/2;
			pix_RGB.blue  =     ((unsigned char)((img[result.y*240+result.x]&0x001f)<<3) + pix_RGB.blue)/2;
			RGBtoHSL(&pix_RGB, &pix_HSL);
			LCD_ShowNum(0,240,pix_HSL.hue,3,16);	
			LCD_ShowNum(32,240,pix_HSL.saturation,3,16);	
			LCD_ShowNum(64,240,pix_HSL.luminance,3,16);	
			condition.H_MIN = (pix_HSL.hue - 8) <0 ? 0 :(pix_HSL.hue - 8);
			condition.H_MAX = (pix_HSL.hue + 8) >255 ? 255 :(pix_HSL.hue + 8);
			condition.S_MIN = (pix_HSL.saturation - 20) <0 ? 0 :(pix_HSL.saturation - 20);
			condition.S_MAX = (pix_HSL.saturation + 20) >255 ? 255 :(pix_HSL.saturation + 20);
			condition.L_MIN = (pix_HSL.luminance - 20) <0 ? 0 : (pix_HSL.luminance - 20) ;
			condition.L_MAX = (pix_HSL.luminance + 20) >255 ? 255 : (pix_HSL.luminance + 20) ;
			}
		}*/
		
    PCout(4)=0;
		/***************编程区域******************/
		DCMI_Start();     //也可处理完成一帧图像后在图像处理函数中启动
	}
} 


//DMA2_Stream1中断服务函数
void DMA2_Stream1_IRQHandler(void)   //传完一行图像进入中断（320像素点）
{
	static int i=0;
	if(DMA_GetFlagStatus(DMA2_Stream1,DMA_FLAG_TCIF1)==SET)//DMA2_Steam1,传输完成标志
	{  
		DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1);//清除传输完成中断
		/***************编程区域******************/
		//rows_count代表当前读取的是第几行数据；
		
		//PCout(4)=0;
		if(rows_count == 230) 
			rows_count = rows_count;
	//	if(rows_count%2==0)
			for(i=0;i<240;i+=2)
			{
				*(img_p+rows_count*240+i) = *(row+280-i);
			}
//		LCD_Color_Fill(0,rows_count+80,239,rows_count+80,row+40);
		
   // PCout(4)=1;
		/***************编程区域******************/
		rows_count++;
	}    											 
}  

unsigned short ReadPoint(unsigned short x,unsigned short y)
{
	return *(img_p + y*240 + x);
}


//DCMI DMA配置
//memaddr:存储器地址    将要存储摄像头数据的内存地址(也可以是外设地址)
//DMA_BufferSize:存储器长度    0~65535
//DMA_MemoryDataSize:存储器位宽    @defgroup DMA_memory_data_size :DMA_MemoryDataSize_Byte/DMA_MemoryDataSize_HalfWord/DMA_MemoryDataSize_Word
//DMA_MemoryInc:存储器增长方式  @defgroup DMA_memory_incremented_mode  /** @defgroup DMA_memory_incremented_mode : DMA_MemoryInc_Enable/DMA_MemoryInc_Disable
 
void DCMI_DMA_Init(u32 DMA_Memory0BaseAddr,u16 DMA_BufferSize,u32 DMA_MemoryDataSize)
{ 
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2时钟使能 
	DMA_DeInit(DMA2_Stream1);//等待DMA2_Stream1
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//等待DMA2_Stream1可配置 
	
  /* 配置 DMA Stream */
  DMA_InitStructure.DMA_Channel = 						DMA_Channel_1;  //通道1 DCMI通道 
  DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&DCMI->DR;//外设地址为:DCMI->DR
  DMA_InitStructure.DMA_Memory0BaseAddr =		  DMA_Memory0BaseAddr;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = 								DMA_DIR_PeripheralToMemory;//外设到存储器模式
  DMA_InitStructure.DMA_BufferSize =				  DMA_BufferSize;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc =			  DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = 					DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Word;//外设数据长度:32位
  DMA_InitStructure.DMA_MemoryDataSize =		  DMA_MemoryDataSize;//存储器数据长度 
  DMA_InitStructure.DMA_Mode = 								DMA_Mode_Circular;// 使用循环模式 
  DMA_InitStructure.DMA_Priority =					  DMA_Priority_High;//高优先级
  DMA_InitStructure.DMA_FIFOMode =					  DMA_FIFOMode_Enable; //FIFO模式        
  DMA_InitStructure.DMA_FIFOThreshold = 			DMA_FIFOThreshold_Full;//使用全FIFO 
  DMA_InitStructure.DMA_MemoryBurst = 			  DMA_MemoryBurst_Single;//外设突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst =		  DMA_PeripheralBurst_Single;//存储器突发单次传输
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);//初始化DMA Stream
		
	DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);//开启传输完成中断
		
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
} 

//DCMI初始化
void My_DCMI_Init(void)
{
	
	DCMI_InitTypeDef DCMI_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOA B C D 时钟
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI,ENABLE);
	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_6;//PA4/6   复用功能输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6|GPIO_Pin_8|GPIO_Pin_9;//PB7:VSYNC  ;   PB6,8,9:D5,6,7
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11;//PC6/7/8/9/11 : D0-4
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化	

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_DCMI); //PA4,AF13  DCMI_HERF
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_DCMI); //PA6,AF13  DCMI_XCLK  
 	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_DCMI); //PB7,AF13  DCMI_VSYNC 
 	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_DCMI); //PC6,AF13  DCMI_D2  
 	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_DCMI); //PC7,AF13  DCMI_D3 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_DCMI); //PC8,AF13  DCMI_D4
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_DCMI); //PC9,AF13  DCMI_D5
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_DCMI);//PC11,AF13 DCMI_D6 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_DCMI); //PB6,AF13  DCMI_D7 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_DCMI); //PB8,AF13  DCMI_D8
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_DCMI); //PB9,AF13  DCMI_D9

	
	DCMI_DeInit();//清除原来的设置 
 
 
  DCMI_InitStructure.DCMI_CaptureMode=DCMI_CaptureMode_Continuous;//连续模式
	DCMI_InitStructure.DCMI_CaptureRate=DCMI_CaptureRate_All_Frame;//全帧捕获
//	DCMI_InitStructure.DCMI_CaptureRate=DCMI_CaptureRate_1of4_Frame;
	DCMI_InitStructure.DCMI_ExtendedDataMode= DCMI_ExtendedDataMode_8b;//8位数据格式  
	DCMI_InitStructure.DCMI_HSPolarity= DCMI_HSPolarity_Low;//HSYNC 
	DCMI_InitStructure.DCMI_PCKPolarity= DCMI_PCKPolarity_Rising;//PCLK 上升沿有效
	DCMI_InitStructure.DCMI_VSPolarity=DCMI_VSPolarity_High;//VSYNC 
	DCMI_InitStructure.DCMI_SynchroMode= DCMI_SynchroMode_Hardware;//硬件同步HSYNC,VSYNC
	DCMI_Init(&DCMI_InitStructure);

	DCMI_ITConfig(DCMI_IT_FRAME,ENABLE);//开启帧中断 
	
	DCMI_Cmd(ENABLE);	//DCMI使能

  NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
 
} 
//DCMI,启动传输
void DCMI_Start(void)
{  
	DMA_Cmd(DMA2_Stream1, ENABLE);//开启DMA2,Stream1 
	DCMI_CaptureCmd(ENABLE);//DCMI捕获使能  
}
//DCMI,关闭传输
void DCMI_Stop(void)
{ 
  DCMI_CaptureCmd(DISABLE);//DCMI捕获使关闭	
	
	while(DCMI->CR&0X01);		//等待传输结束 
	 	
	DMA_Cmd(DMA2_Stream1,DISABLE);//关闭DMA2,Stream1
} 







