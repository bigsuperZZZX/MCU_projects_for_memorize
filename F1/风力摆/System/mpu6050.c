#include "mpu6050.h"
#include "stm32f10x.h"
#include "string.h"


/*float cur_x,cur_y;

void get_angle(void)
{
	u8 Temp[11];
	extern u8 Re_buf[11],temp_buf[11],counter,sign;
	if(sign)
  {  
      memcpy(Temp,Re_buf,11);
      sign=0;
      if(Re_buf[0]==0x55)       //ºÏ≤È÷°Õ∑
      {  
         if(Re_buf[1]==0x53)
         {
            cur_y = ((short)(Temp[3]<<8| Temp[2]))/32768.0*180;   //X÷·πˆ◊™Ω«£®x ÷·£©
            cur_x = ((short)(Temp[5]<<8| Temp[4]))/32768.0*180;   //Y÷·∏©—ˆΩ«£®y ÷·£©
         }
      }
         
   }
			
}*/

