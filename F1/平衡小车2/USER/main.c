#include "stm32f10x.h"
#include "sys.h"   
#include "BMP.h"

/*
 *编译环境：Keil5
 *时间：2017/4/26/
 */

int main(void)
{ 
		delay_init();	    	         
		uart_init(115200);	         
    OLED_Init();                      
	  KEY_Init();                   
    myPWM_Init(7200);  
    Encoder_Init();         
    IIC_Init();                  
    MPU6050_initialize();         
    DMP_Init();                    
	  myEXTI_Init(); 
	
		OLED_ShowCHinese(16,6,6);
		OLED_ShowCHinese(32,6,7);
		OLED_ShowCHinese(88,6,8);
		OLED_DrawBMP(72,0,120,6,bmp3);
    while(1)
	  {
			
	  } 
}

