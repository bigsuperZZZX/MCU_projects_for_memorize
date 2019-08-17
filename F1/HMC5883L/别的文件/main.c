#include "system.h"
#include "delay.h"
#include "nokia_5110.h"
#include "HMC5883.H"

//=== 电 ===
u8 dian[24] = {0x00,0xFC,0x24,0x24,0x24,0xFF,0x24,0x24,
      0x24,0xFE,0x04,0x00,0x00,0x01,0x01,0x01,
      0x01,0x07,0x09,0x09,0x09,0x09,0x08,0x0E};
//=== 子 ===
u8 zi[24] = {0x00,0x20,0x21,0x21,0x21,0x21,0xF9,0x25,
      0x23,0x21,0x30,0x20,0x00,0x00,0x00,0x04,
      0x08,0x08,0x0F,0x00,0x00,0x00,0x00,0x00};
//=== 罗 ===
u8 luo[24] = {0x00,0x1F,0x89,0x49,0xAF,0x39,0x29,0x2F,
      0xA9,0x69,0x1F,0x00,0x08,0x09,0x08,0x04,
      0x04,0x03,0x02,0x01,0x00,0x00,0x00,0x00};
//=== 盘 ===
u8 pan[24] = {0x10,0x90,0x50,0x3E,0x12,0x37,0x5A,0x12,
      0x52,0x7E,0x10,0x10,0x08,0x08,0x0F,0x09,
      0x0F,0x09,0x09,0x0F,0x09,0x0F,0x08,0x08};
	
//=============================
//NOKIA5110显示整型数
//=============================
void PrintInt16_5110(u8 x,u8 y,u8 ws,i16 num)
{
	u8 i = 0;
	u8 arr[5] = {0x20,0x20,0x20,0x20,0x20};		//空格
	if(num < 0)
		{	
			num = -num;
			LCD_set_XY(x,y);
			LCD_write_char('-');
		}
		else if(num > 0)
		{
			LCD_set_XY(x,y);
			LCD_write_char(' ');
		}
		else
		{
			LCD_set_XY(x,y);
			LCD_write_char(' ');
		}
		
		if(num >= 10000)
		{
			arr[0] = num/10000%10 + '0';
			arr[1] = num/1000%10 + '0';
			arr[2] = num/100%10 + '0';
			arr[3] = num/10%10 + '0';
			arr[4] = num%10 + '0';
		}
		else if(num >= 1000)
		{
			arr[0] = num/1000%10 + '0';
			arr[1] = num/100%10 + '0';
			arr[2] = num/10%10 + '0';
			arr[3] = num%10 + '0';
			arr[4] = '\0';
		}
		else if(num >= 100)
		{
			arr[0] = num/100%10 + '0';
			arr[1] = num/10%10 + '0';
			arr[2] = num%10 + '0';
			arr[3] = '\0';
		}
		else if(num >= 10)
		{
			arr[0] = num/10%10 + '0';
			arr[1] = num%10 + '0';
			arr[2] = '\0';
		}	
		else
		{
			arr[0] = num%10 + '0';
			arr[1] = '\0';
		}
		while(arr[i] != '\0')
		{
			LCD_write_char(arr[i]);
			i++;
		}
		while(i < ws)
		{
			LCD_write_char(' ');
			i++;
		}
}
//=============================
//NOKIA5110显示单个汉字 
//从左到右，从上到下，纵向8点下高位，12点阵。
//=============================
void PrintHz_5110(u8 x,u8 y,u8 *hanzi)
{		
	u8 i = 0;
	LCD_set_XY(x,y);
	for(i = 0;i < 12;i++)
	{
		LCD_write_byte(hanzi[i],1);
	}
	LCD_set_XY(x,y+1);
	for(i = 12;i < 24;i++)
	{
		LCD_write_byte(hanzi[i],1);
	}
}
//=============================
//主函数
//=============================
int main(void)
{	
	i16 HMCx = 0;
	i16 HMCy = 0;
	i16 HMCz = 0;
	

	
	Clock_Init(9);
	delay_init(72);
	delayms(500);
	LCD_init(); //初始化液晶    
	LCD_clear();
	HMC5883_Init();

	//======== PC13置零 LED亮 ===========
	RCC ->APB2ENR |= 1 << 4;			//使能PC时钟
	GPIOC -> CRH &= 0xFF0FFFFF;		//清除PC13设置
	GPIOC -> CRH |= 0x00300000;		//通用推挽输出，50MHz
	GPIOC -> BRR = 1<< 13;
	
	while(1)
	{
		HMC5883_Read_6Data();
		HMCx = (HMC5883_BUF[0] << 8) | HMC5883_BUF[1] ;
		HMCz = (HMC5883_BUF[2] << 8) | HMC5883_BUF[3] ;
		HMCy = (HMC5883_BUF[4] << 8) | HMC5883_BUF[5] ;
		
		LCD_set_XY(0,0);
		LCD_write_char('X');
		LCD_set_XY(6,0);
		LCD_write_char(':');
		PrintInt16_5110(12,0,4,HMCx);
		
		LCD_set_XY(0,1);
		LCD_write_char('Y');
		LCD_set_XY(6,1);
		LCD_write_char(':');
		PrintInt16_5110(12,1,4,HMCy);
		
		LCD_set_XY(0,2);
		LCD_write_char('Z');
		LCD_set_XY(6,2);
		LCD_write_char(':');
		PrintInt16_5110(12,2,4,HMCz);

		
		PrintHz_5110(10,4,dian);
		PrintHz_5110(24,4,zi);
		PrintHz_5110(38,4,luo);
		PrintHz_5110(52,4,pan);
		
	}
}







