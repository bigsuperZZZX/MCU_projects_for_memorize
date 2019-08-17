#include "sys.h"
#include "dcmi.h" 
#include "ov7725.h" 
#include "usart.h"
#include "delay.h"
#include "imgprcs.h"
#include "lcd.h"
#include "EasyTracer.h"
#include "touch.h" 
#include "math.h" 
#include "GUI.h" 

extern COLOR_HSL Gray2[240][240];
u16 row[320];
u16 img[240*240]; 
u16 *img_p = img;
/***************Ŀ������ = {Hmin,Hmax,Smin,Smax,Lmin,Lmax,WidMin ,HeiMin,WidMax  ,HeiMax    }; */
TARGET_CONDI condition  = {  0 ,  0 ,  0  , 0 ,  0  , 0 ,   0   ,   0   ,230  ,    230    };
RESULT result;

#define thr 21
#define l_thr 40
#define s_thr 40

#define COLORED 0x01
#define BLACKED 0x02


/*u8 img1_4[160*120];
u8* pimg1_4 = img1_4;*/

u8 rows_count = 0;

u16 x_color_all,y_color_all,x_black_all,y_black_all;

void dot_track(u8 color_black_select)
{
	static u16 x=120,y=120,x_temp,y_temp,x_color_last =120 , y_color_last =120 , x_black_last =120, y_black_last =120;
	static u16 x_color =120 , y_color =120 , x_black =120 , y_black =120;
	signed int i,j;
	u32 sum_color_x = 0 , sum_color_y = 0,  sum_color_count = 0 ,sum_black_x = 0 , sum_black_y = 0, sum_black_count = 0;;
	static COLOR_RGB pix_RGB;
	static COLOR_HSL pix_HSL;
	
	if(TP_Read_XY2(&x ,&y))
		{
			if(x % 2 == 1) x= x+1;
			if(y % 2 == 1) y= y+1;
			if(x > 239) 
			{
				x=239;
				return;
			}
			if(y > 239)
			{
				y=239;
				return;
			}				
			pix_RGB.red   =	 (unsigned char)((img[y*240+x]&0xf800)>>8);
			pix_RGB.green =	 (unsigned char)((img[y*240+x]&0x07e0)>>3);
			pix_RGB.blue  =     (unsigned char)((img[y*240+x]&0x001f)<<3);
			RGBtoHSL(&pix_RGB, &pix_HSL);
			TP_Drow_Touch_Point(x,y,RED);	//����2
			LCD_ShowNum(120,240,pix_HSL.hue,3,16);	
			LCD_ShowNum(152,240,pix_HSL.saturation,3,16);	
			LCD_ShowNum(184,240,pix_HSL.luminance,3,16);	
			if(pix_HSL.luminance > 50)
			{
				condition.H_MIN = (pix_HSL.hue - thr) <0 ? (240-(thr-pix_HSL.hue)) :(pix_HSL.hue - thr);  //����Ҫ��,���滹Ҫ������
				condition.H_MAX = (pix_HSL.hue + thr) >240 ? (pix_HSL.hue + thr-240) :(pix_HSL.hue + thr);
				condition.S_MIN = (pix_HSL.saturation - s_thr) <0 ? 0 :(pix_HSL.saturation - s_thr);
				condition.S_MAX = (pix_HSL.saturation + s_thr) >255 ? 255 :(pix_HSL.saturation + s_thr);
			}
			else
			{
				condition.L_MIN = (pix_HSL.luminance - l_thr) <0 ? 0 : (pix_HSL.luminance - l_thr) ;
				condition.L_MAX = (pix_HSL.luminance + l_thr) >255 ? 255 : (pix_HSL.luminance + l_thr) ;
			}
	/*		LCD_ShowNum(0,260,condition.H_MIN,3,16);	
			LCD_ShowNum(32,260,condition.H_MAX,3,16);	
			LCD_ShowNum(64,260,condition.S_MIN,3,16);	
			LCD_ShowNum(96,260,condition.S_MAX,3,16);	
			LCD_ShowNum(128,260,condition.L_MIN,3,16);	
			LCD_ShowNum(160,260,condition.L_MAX,3,16);	*/
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
					if(color_black_select & BLACKED)   //�����ڰף�ʶ���ɫ
					{
						if(pix_HSL.luminance > condition.L_MIN && pix_HSL.luminance < condition.L_MAX) 
						{
							sum_black_x += x;
							sum_black_y += y;
							sum_black_count++;
							img[y*240+x] = 0x07e0;
						}
					}
					if(color_black_select & COLORED)   //������ɫ
					{
						if((pix_HSL.hue > 240-thr || pix_HSL.hue < thr) && (condition.H_MIN > condition.H_MAX))
						{
							if((pix_HSL.hue < condition.H_MIN || pix_HSL.hue > condition.H_MAX) &&
							 pix_HSL.saturation > condition.S_MIN && pix_HSL.saturation < condition.S_MAX)
							{
								sum_color_x += x;
								sum_color_y += y;
								sum_color_count++;
								img[y*240+x] = 0x0000;
							}
						}
						else if(pix_HSL.hue > condition.H_MIN && pix_HSL.hue < condition.H_MAX &&
							 pix_HSL.saturation > condition.S_MIN && pix_HSL.saturation < condition.S_MAX)
						{
							sum_color_x += x;
							sum_color_y += y;
							sum_color_count++;
							img[y*240+x] = 0x0000;
						}
					}
				}
			}
			
			
			if(color_black_select & COLORED)   //�����ڰף�ʶ���ɫ
			{
				x = sum_color_x/sum_color_count;
				y = sum_color_y/sum_color_count;
				sum_color_count = 0;
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
							sum_color_count ++;
						}
					}
				}
				if(sum_color_count < 50)
				{
					x_color = x_color_last;
					y_color = y_color_last;
				}
				else
				{
					x_color = x;
					y_color = y;
					x_color_last = x_color;
					y_color_last = y_color;
				}
				
				x_color = 240- x_color ;

				x_color_all = x_color;
				y_color_all = y_color;
				
//				TP_Drow_Touch_Point(240-x_color,y_color,RED);	//����2
				LCD_ShowNum(0,256,240-x_color,3,16);	
				LCD_ShowNum(32,256,y_color,3,16);	
			}
			
			if(color_black_select & BLACKED)   //�����ڰף�ʶ���ɫ
			{
				x = sum_black_x/sum_black_count;
				y = sum_black_y/sum_black_count;
				sum_black_count = 0;
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
						if(img[(i+y_temp)*240+x_temp+j] == 0x07e0)
						{
							sum_black_count ++;
						}
					}
				}
				if(sum_black_count < 50)
				{
					x_black = x_black_last;
					y_black = y_black_last;
				}
				else
				{
					x_black = x;
					y_black = y;
					x_black_last = x_black;
					y_black_last = y_black;
				}
				
				x_black = 240- x_black ;

				x_black_all = x_black;
				y_black_all = y_black;
//				TP_Drow_Touch_Point(240-x_black,y_black,RED);	//����2
				LCD_ShowNum(64,256,240-x_black,3,16);	
				LCD_ShowNum(96,256,y_black,3,16);	
			}
			
			
			if(color_black_select == BLACKED)
			{
				USART_SendData(USART1,0xA1);
				while((USART1->SR&0X40)==0);
				delay_us(100);
				USART_SendData(USART1,y_black/2);
				while((USART1->SR&0X40)==0);
				delay_us(100);
				USART_SendData(USART1,x_black/2);
				while((USART1->SR&0X40)==0);
				delay_us(100);
			}
			else
			{
				USART_SendData(USART1,0xA0);
				while((USART1->SR&0X40)==0);
				delay_us(100);
				USART_SendData(USART1,y_color/2);
				while((USART1->SR&0X40)==0);
				delay_us(100);
				USART_SendData(USART1,x_color/2);
				while((USART1->SR&0X40)==0);
				delay_us(100);
			}
		}
		
}

void line_track(void)
{
	u8 line_count = 0, i=0 ,j=9;
//	static u16 temp = 110;
	static u16 x=120,y=120;
	static signed char line_angle = 0;
	static unsigned char line_mid = 0;
	u32 sum_y[10] = {0,0,0,0,0,0,0,0,0,0} , sum_count[10] = {0,0,0,0,0,0,0,0,0,0} ;
	static signed int y_position[10], last_y_position[10]  = {120,120,120,120,120,120,120,120,120,120};
//	static u8 last_sum_y[10] = {120,120,120,120,120,120,120,120,120,120};
	static COLOR_RGB pix_RGB;
	static COLOR_HSL pix_HSL;
	
	if(TP_Read_XY2(&x ,&y))
		{
			if(x % 2 == 1) x= x+1;
			if(y % 2 == 1) y= y+1;
			if(x > 239) x=239;
			if(y > 239) y=239;
			pix_RGB.red   =	 (unsigned char)((img[y*240+x]&0xf800)>>8);
			pix_RGB.green =	 (unsigned char)((img[y*240+x]&0x07e0)>>3);
			pix_RGB.blue  =     (unsigned char)((img[y*240+x]&0x001f)<<3);
			RGBtoHSL(&pix_RGB, &pix_HSL);
			TP_Drow_Touch_Point(x,y,RED);	//����2
			LCD_ShowNum(0,240,pix_HSL.hue,3,16);	
			LCD_ShowNum(32,240,pix_HSL.saturation,3,16);	
			LCD_ShowNum(64,240,pix_HSL.luminance,3,16);	
			condition.H_MIN = (pix_HSL.hue - thr) <0 ? (240-(thr-pix_HSL.hue)) :(pix_HSL.hue - thr);  //����Ҫ��,���滹Ҫ������
			condition.H_MAX = (pix_HSL.hue + thr) >240 ? (pix_HSL.hue + thr-240) :(pix_HSL.hue + thr);
			condition.S_MIN = (pix_HSL.saturation - 50) <0 ? 0 :(pix_HSL.saturation - 50);
			condition.S_MAX = (pix_HSL.saturation + 50) >255 ? 255 :(pix_HSL.saturation + 50);
			condition.L_MIN = (pix_HSL.luminance - l_thr) <0 ? 0 : (pix_HSL.luminance - l_thr) ;
			condition.L_MAX = (pix_HSL.luminance + l_thr) >255 ? 255 : (pix_HSL.luminance + l_thr) ;
			LCD_ShowNum(0,260,condition.H_MIN,3,16);	
			LCD_ShowNum(32,260,condition.H_MAX,3,16);	
			LCD_ShowNum(64,260,condition.S_MIN,3,16);	
			LCD_ShowNum(96,260,condition.S_MAX,3,16);	
			LCD_ShowNum(128,260,condition.L_MIN,3,16);	
			LCD_ShowNum(160,260,condition.L_MAX,3,16);	
		}
		else
		{
			//���������ƶ���ʱ��Y�����ڱ仯��ǰ���ƶ���ʱ��X����
			for(x=12;x<240;x+=24)
			{
				line_count = (x-12)/24;
				sum_count[line_count] = 0;
				for(y=0;y<240;y++)
				{
					pix_RGB.red   =	 (unsigned char)((img[y*240+x]&0xf800)>>8);
					pix_RGB.green =	 (unsigned char)((img[y*240+x]&0x07e0)>>3);
					pix_RGB.blue  =     (unsigned char)((img[y*240+x]&0x001f)<<3);
					RGBtoHSL(&pix_RGB, &pix_HSL);
					/****Ѱ����****/
				/*	if(pix_HSL.luminance > condition.L_MIN && pix_HSL.luminance < condition.L_MAX) 
					
					{
						sum_y[line_count] += y;
						sum_count[line_count]++;
						img[y*240+x] = 0x07e0;
					}*/
					/****Ѱ����****/
					if((pix_HSL.hue > 240-thr || pix_HSL.hue < thr) && (condition.H_MIN > condition.H_MAX))
					{
						if((pix_HSL.hue < condition.H_MIN || pix_HSL.hue > condition.H_MAX) &&
						 pix_HSL.saturation > condition.S_MIN && pix_HSL.saturation < condition.S_MAX)
						{
						sum_y[line_count] += y;
						sum_count[line_count]++;
						img[y*240+x] = 0x07e0;
						}
					}
					else if(pix_HSL.hue > condition.H_MIN && pix_HSL.hue < condition.H_MAX &&
						 pix_HSL.saturation > condition.S_MIN && pix_HSL.saturation < condition.S_MAX)
					{
						sum_y[line_count] += y;
						sum_count[line_count]++;
						img[y*240+x] = 0x07e0;
					}
					/************************/
					
				}
				if(sum_count[line_count] > 3)  //���㹻��
				{
					y_position[line_count] = sum_y[line_count]/sum_count[line_count];
					
					//��ͷ�������
					switch(line_count)
					{
						case 0:;
						case 9: y_position[line_count] = 88 + (y_position[line_count] - 88)*112/100; break;
						case 1:;
						case 8: y_position[line_count] = 88 + (y_position[line_count] - 88)*107/100; break;
						case 2:;
						case 7: y_position[line_count] = 88 + (y_position[line_count] - 88)*105/100; break;
						case 3:;
						case 6: y_position[line_count] = 88 + (y_position[line_count] - 88)*105/100; break;
						case 4:;
						case 5: break;
					}
					if(y_position[line_count]>255) y_position[line_count] = 255;
					if(y_position[line_count]< 0 ) y_position[line_count] = 0;
					last_y_position[line_count] = y_position[line_count];
				}
				else
				{
					y_position[line_count] = last_y_position[line_count];
				}
				if(line_count <= 4)
					LCD_ShowNum(line_count*32,280,y_position[line_count],3,16);	
				else
					LCD_ShowNum((x-12-120)/24*32,300,y_position[line_count],3,16);	
			}
			
			//��ǰ��ֱ�ʼѰ����Ч�Ĳɼ�ֵ
			while(sum_count[i] < 3) i++; 
			while(sum_count[j] < 3) j--;
			if(j==i) j+=1;
			line_angle = (y_position[i] - y_position[j])*4/(j-i);
			line_mid = (y_position[i] + y_position[j])/2;
			
			if(line_angle<0)
			{
				LCD_ShowChar(199,280,'-',16,0);
				LCD_ShowNum(207,280,~(line_angle-1),4,16);
			}
			else
			{
				LCD_ShowChar(199,280,' ',16,0);
				LCD_ShowNum(207,280,line_angle,4,16);
			}
			LCD_ShowNum(207,300,line_mid,4,16);
			
			USART_SendData(USART1,0xA1);   //ѭ��֡ͷ
			while((USART1->SR&0X40)==0);
			delay_us(100);
			USART_SendData(USART1,(u8)line_angle);
			while((USART1->SR&0X40)==0);
			delay_us(100);
			USART_SendData(USART1,line_mid/2);
		}
		
}


extern u8 uart1_rx_data[20];

u8 decode_frame(void)
{
	if(uart1_rx_data[0] == 0xb0)
		return COLORED;
	if(uart1_rx_data[0] == 0xb1)
		return BLACKED;
	return 0xff;
}

extern u8 uart1rx_cplt ;;
u16 num,nump;

void yaw_flaot_display(void)
{
	if(uart1_rx_data[0] == 0xB2 && 
		((uart1_rx_data[1] + uart1_rx_data[2] + uart1_rx_data[3])&0x00ff) == uart1_rx_data[4])
	{
		if(uart1_rx_data[1] == 0)
		{
			LCD_ShowChar(0,240,' ',16,0);
		}
		else
		{
			LCD_ShowChar(0,240,'-',16,0);
		}
		LCD_ShowxNum(8,240,uart1_rx_data[2],2,16,0);
		LCD_ShowChar(24,240,'.',16,0);
		LCD_ShowxNum(32,240,uart1_rx_data[3],1,16,0);
	}
}

//DCMI�жϷ�����
void DCMI_IRQHandler(void)
{
	u8 color_black_select = 0xff;
	
	if(DCMI_GetITStatus(DCMI_IT_FRAME)==SET)//����һ֡ͼ��  320*240
	{
		DCMI_Stop();
		DCMI_ClearITPendingBit(DCMI_IT_FRAME);//���֡�ж�
		rows_count = 0;
		/***************�������******************/
		PCout(4)=1;
		
		if( uart1rx_cplt == 1)
		{
			color_black_select = decode_frame();
			yaw_flaot_display();
		}
			
		dot_track(color_black_select);
		
		com_send( key_refresh() );
		
		
		LCD_Color_Fill(0,0,239,239,img);
		TP_Drow_Touch_Point(240-x_color_all,y_color_all,GREEN);
		TP_Drow_Touch_Point(240-x_black_all,y_black_all,RED);
		
		
    PCout(4)=0;
		/**************�������******************/
		DCMI_Start();     //Ҳ�ɴ������һ֡ͼ�����ͼ��������������
	}
} 


//DMA2_Stream1�жϷ�����
void DMA2_Stream1_IRQHandler(void)   //����һ��ͼ������жϣ�320���ص㣩
{
	static int i=0;
	if(DMA_GetFlagStatus(DMA2_Stream1,DMA_FLAG_TCIF1)==SET)//DMA2_Steam1,������ɱ�־
	{  
		DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1);//�����������ж�
		/***************�������******************/
		//rows_count������ǰ��ȡ���ǵڼ������ݣ�
		
		//PCout(4)=0;
		
		//���ٵ��ʱ����
			
			for(i=0;i<240;i+=2)
			{
				*(img_p+rows_count*240+i) = *(row+280-i);
			}
		
		//�����ߵ�ʱ����
		/*	for(i=0;i<230;i++)
			{
				*(img_p+rows_count*240+i) = *(row+280-i);
			}*/
		
//		LCD_Color_Fill(0,rows_count+80,239,rows_count+80,row+40);
		
   // PCout(4)=1;
		/***************�������******************/
		rows_count++;
	}    											 
}  

unsigned short ReadPoint(unsigned short x,unsigned short y)
{
	return *(img_p + y*240 + x);
}


//DCMI DMA����
//memaddr:�洢����ַ    ��Ҫ�洢����ͷ���ݵ��ڴ��ַ(Ҳ�����������ַ)
//DMA_BufferSize:�洢������    0~65535
//DMA_MemoryDataSize:�洢��λ��    @defgroup DMA_memory_data_size :DMA_MemoryDataSize_Byte/DMA_MemoryDataSize_HalfWord/DMA_MemoryDataSize_Word
//DMA_MemoryInc:�洢��������ʽ  @defgroup DMA_memory_incremented_mode  /** @defgroup DMA_memory_incremented_mode : DMA_MemoryInc_Enable/DMA_MemoryInc_Disable
 
void DCMI_DMA_Init(u32 DMA_Memory0BaseAddr,u16 DMA_BufferSize,u32 DMA_MemoryDataSize)
{ 
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ�� 
	DMA_DeInit(DMA2_Stream1);//�ȴ�DMA2_Stream1
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//�ȴ�DMA2_Stream1������ 
	
  /* ���� DMA Stream */
  DMA_InitStructure.DMA_Channel = 						DMA_Channel_1;  //ͨ��1 DCMIͨ�� 
  DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&DCMI->DR;//�����ַΪ:DCMI->DR
  DMA_InitStructure.DMA_Memory0BaseAddr =		  DMA_Memory0BaseAddr;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = 								DMA_DIR_PeripheralToMemory;//���赽�洢��ģʽ
  DMA_InitStructure.DMA_BufferSize =				  DMA_BufferSize;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc =			  DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = 					DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Word;//�������ݳ���:32λ
  DMA_InitStructure.DMA_MemoryDataSize =		  DMA_MemoryDataSize;//�洢�����ݳ��� 
  DMA_InitStructure.DMA_Mode = 								DMA_Mode_Circular;// ʹ��ѭ��ģʽ 
  DMA_InitStructure.DMA_Priority =					  DMA_Priority_High;//�����ȼ�
  DMA_InitStructure.DMA_FIFOMode =					  DMA_FIFOMode_Enable; //FIFOģʽ        
  DMA_InitStructure.DMA_FIFOThreshold = 			DMA_FIFOThreshold_Full;//ʹ��ȫFIFO 
  DMA_InitStructure.DMA_MemoryBurst = 			  DMA_MemoryBurst_Single;//����ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst =		  DMA_PeripheralBurst_Single;//�洢��ͻ�����δ���
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);//��ʼ��DMA Stream
		
	DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);//������������ж�
		
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
} 

//DCMI��ʼ��
void My_DCMI_Init(void)
{
	
	DCMI_InitTypeDef DCMI_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOA B C D ʱ��
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI,ENABLE);
	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_6;//PA4/6   ���ù������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //���ù������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6|GPIO_Pin_8|GPIO_Pin_9;//PB7:VSYNC  ;   PB6,8,9:D5,6,7
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11;//PC6/7/8/9/11 : D0-4
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��	

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

	
	DCMI_DeInit();//���ԭ�������� 
 
 
  DCMI_InitStructure.DCMI_CaptureMode=DCMI_CaptureMode_Continuous;//����ģʽ
	DCMI_InitStructure.DCMI_CaptureRate=DCMI_CaptureRate_All_Frame;//ȫ֡����
//	DCMI_InitStructure.DCMI_CaptureRate=DCMI_CaptureRate_1of4_Frame;
	DCMI_InitStructure.DCMI_ExtendedDataMode= DCMI_ExtendedDataMode_8b;//8λ���ݸ�ʽ  
	DCMI_InitStructure.DCMI_HSPolarity= DCMI_HSPolarity_Low;//HSYNC 
	DCMI_InitStructure.DCMI_PCKPolarity= DCMI_PCKPolarity_Rising;//PCLK ��������Ч
	DCMI_InitStructure.DCMI_VSPolarity=DCMI_VSPolarity_High;//VSYNC 
	DCMI_InitStructure.DCMI_SynchroMode= DCMI_SynchroMode_Hardware;//Ӳ��ͬ��HSYNC,VSYNC
	DCMI_Init(&DCMI_InitStructure);

	DCMI_ITConfig(DCMI_IT_FRAME,ENABLE);//����֡�ж� 
	
	DCMI_Cmd(ENABLE);	//DCMIʹ��

  NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
 
} 
//DCMI,��������
void DCMI_Start(void)
{  
	DMA_Cmd(DMA2_Stream1, ENABLE);//����DMA2,Stream1 
	DCMI_CaptureCmd(ENABLE);//DCMI����ʹ��  
}
//DCMI,�رմ���
void DCMI_Stop(void)
{ 
  DCMI_CaptureCmd(DISABLE);//DCMI����ʹ�ر�	
	
	while(DCMI->CR&0X01);		//�ȴ�������� 
	 	
	DMA_Cmd(DMA2_Stream1,DISABLE);//�ر�DMA2,Stream1
} 






