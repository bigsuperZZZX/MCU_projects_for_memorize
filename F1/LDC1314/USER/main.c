#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "stm32f10x.h"
#include "IIC.h"
#include "LDC13xx16xx_cmd.h"
#include "LDC13xx16xx_evm.h"



 int main(void)
 {
	
		delay_init();	    	 //��ʱ������ʼ��	  
	//	OLED_Init();			//��ʼ��OLED 
		IIC_Config();
			evm_init();
	 
		while(1)
		{
			delay_ms(2000);
		}
}

