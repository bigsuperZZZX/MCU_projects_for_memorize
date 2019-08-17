#include "LCD.h"
#include "GUI.h"
#include "TOUCH.h"
#include "stm32f4xx.h" 

#define x_offset 15
#define y_offset 4

void mode_display_refresh()
{
	LCD_ShowString(0 + x_offset , 272 + y_offset, 48 ,16,16 , "MODE 1");
	LCD_ShowString(80 + x_offset , 272 + y_offset, 48 ,16,16 , "MODE 2");
	LCD_ShowString(160 + x_offset , 272 + y_offset, 48 ,16,16 , "MODE 3");
	LCD_ShowString(0 + x_offset , 296 + y_offset, 48 ,16,16 , "MODE 4");
	LCD_ShowString(80 + x_offset , 296 + y_offset, 48 ,16,16 , "MODE 5");
	LCD_ShowString(160 + x_offset , 296 + y_offset, 52 ,16,16 , "START");
}

void GUI_init(void)
{
	POINT_COLOR=BLACK;
	LCD_DrawLine(0,272,240,272);
	LCD_DrawLine(0,296,240,296);
	LCD_DrawLine(80,272,80,320);
	LCD_DrawLine(160,272,160,320);
	LCD_Fill(0,272,80,296,LIGHTGRAY);
	LCD_Fill(80,272,160,296,LIGHTGRAY);
	LCD_Fill(160,272,240,296,LIGHTGRAY);
	LCD_Fill(0,296,80,320,LIGHTGRAY);
	LCD_Fill(80,296,160,320,LIGHTGRAY);
	LCD_Fill(160,296,240,320,LIGHTGRAY);
	LCD_ShowString(0 + x_offset , 272 + y_offset, 48 ,16,16 , "MODE 1");
	LCD_ShowString(80 + x_offset , 272 + y_offset, 48 ,16,16 , "MODE 2");
	LCD_ShowString(160 + x_offset , 272 + y_offset, 48 ,16,16 , "MODE 3");
	LCD_ShowString(0 + x_offset , 296 + y_offset, 48 ,16,16 , "MODE 4");
	LCD_ShowString(80 + x_offset , 296 + y_offset, 48 ,16,16 , "MODE 5");
	LCD_ShowString(160 + x_offset , 296 + y_offset, 52 ,16,16 , "START");
}


uint8_t key_refresh(void)
{
	uint16_t x,y;
	uint8_t key;
	if(TP_Read_XY2(&x,&y))
	{
			if(x!=0)
			{
				if(x<80&&x>=0&&y<296&&y>=272)
				{
					key=1;
					LCD_Fill(0,272,240,320,LIGHTGRAY);
					LCD_Fill(0,272,80,296,GRAY);
				}
				else if(x<160&&x>=80&&y<296&&y>=272)
				{
					key=2;
					LCD_Fill(0,272,240,320,LIGHTGRAY);
					LCD_Fill(80,272,160,296,GRAY);
				}
				else if(x<240&&x>=160&&y<296&&y>=272)
				{
					key=3;
					LCD_Fill(0,272,240,320,LIGHTGRAY);
					LCD_Fill(160,272,240,296,GRAY);
				}
				else if(x<80&&x>=0&&y<320&&y>=296)
				{
					key=4;
					LCD_Fill(0,272,240,320,LIGHTGRAY);
					LCD_Fill(0,296,80,320,GRAY);
				}
				else if(x<160&&x>=80&&y<320&&y>=296)
				{
					key=5;
					LCD_Fill(0,272,240,320,LIGHTGRAY);
					LCD_Fill(80,296,160,320,GRAY);
				}
				else if(x<240&&x>=160&&y<320&&y>=296)
				{
					key=6;
					LCD_Fill(0,272,240,320,LIGHTGRAY);
					LCD_Fill(160,296,240,320,GREEN);
				}
				else
					key=7;
			}
			POINT_COLOR=BLACK;
			mode_display_refresh();
			return key;
	}
	else
		return 0;
}


void com_send(uint8_t key)
{
		if(key==0)
		{
			return;
		}
		else if(key==1)
		{
			USART_SendData(USART1,0xA3);
			while((USART1->SR&0X40)==0);
		}
		else if(key==2)
		{
			USART_SendData(USART1,0xA4);
			while((USART1->SR&0X40)==0);
		}
		else if(key==3)
		{
			USART_SendData(USART1,0xA5);
			while((USART1->SR&0X40)==0);
		}
		else if(key==4)
		{
			USART_SendData(USART1,0xA6);
			while((USART1->SR&0X40)==0);
		}
		else if(key==5)
		{
			USART_SendData(USART1,0xA7);
			while((USART1->SR&0X40)==0);
		}
		else if(key==6)
		{
			USART_SendData(USART1,0xA8);
			while((USART1->SR&0X40)==0);
		}
		else if(key==7)//触摸到其他区域
		{
			USART_SendData(USART1,0xAA);
			while((USART1->SR&0X40)==0);
		}
}



