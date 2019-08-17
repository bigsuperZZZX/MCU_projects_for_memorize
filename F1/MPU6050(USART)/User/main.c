#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "MY_time_Conf.h"
//#include "uart_1.h"
//#include "dma.h"
//#include "RS485.h"
//#include "74hc595.h"
#include "string.h"

/**************************
模板配置信息：
选用芯片是 stm32f103vc
启动信息：stm32f10x_hd.s
固件库：stm32固件库v3.5
***************************/

#define led_PB12   PBout(12)	   //位绑定，位定义PB12口作为LED输出。这样速度最快
//#define led_PB1   PBout(1)	   //位绑定，位定义PB1口作为LED输出。这样速度最快
//#define PA9   PAout(10)
//#define CLK1   PDout(0)
//#define CWW1   PDout(1)
//#define CLK2   PDout(2)
//#define CWW2   PDout(3)


//uint8_t i = 0;    //测试用

float a[3],w[3],angle[3],T;
extern u8 Re_buf[11],temp_buf[11],counter,sign;








/*************************************************
函数: int main(void)
功能: main主函数
参数: 无
返回: 无
**************************************************/

int main(void)
{
  
   u8 Temp[11];
   //u8 i;

//   RCC_Configuration();      //初始化时钟
   delay_init(72);            //初始化延时函数
   NVIC_Configuration();      //中断优先级初始化

   USART1_Config();   //初始化 配置USART1		(USART2串口pwm占用，故使用1接收MPU6050数据)
   USART2_Config();   //初始化 配置USART2 （用于接收MPU6050数据）
	 USART3_Config();
   
   delay_ms(200);
      
   while (1)
   {
			if(sign)
      {  
         memcpy(Temp,Re_buf,11);
         sign=0;
         if(Re_buf[0]==0x55)       //检查帧头
         {  
            if(Re_buf[1]==0x53)
            {
                  angle[0] = ((short)(Temp[3]<<8| Temp[2]))/32768.0*180;   //X轴滚转角（x 轴）
                  angle[1] = ((short)(Temp[5]<<8| Temp[4]))/32768.0*180;   //Y轴俯仰角（y 轴）
                  angle[2] = ((short)(Temp[7]<<8| Temp[6]))/32768.0*180;   //Z轴偏航角（z 轴）
            }
						printf("X角度:%.2f  Y角度:%.2f  Z角度:%.2f\r\n",angle[0],angle[1],angle[2]);
         }
         
     }
     delay_ms(1000);
   }
}


/*
int main(void)
{
	
	//u8 i;

	RCC_Configuration();	   //初始化时钟
	GPIO_Configuration();	   //初始化IO端口
	delay_init(72);			   //初始化延时函数
	NVIC_Configuration();	   //中断优先级初始化

	USART1_Config();   //初始化 配置USART1
	USART2_Config();   //初始化 配置USART2 （用于接收MPU6050数据）
	
	delay_ms(200);

		
	while (1)
	{
		if(sign)
		{  
			sign=0;
//			if(Re_buf[0]==0x55)       //检查帧头
//			{  
//				switch(Re_buf[1])
//				{
//					case 0x51: //标识这个包是加速度包
//						a[0] = ((short)(Re_buf[3]<<8 | Re_buf[2]))/32768.0*16;		//X轴加速度
//						a[1] = ((short)(Re_buf[5]<<8 | Re_buf[4]))/32768.0*16;		//Y轴加速度
//						a[2] = ((short)(Re_buf[7]<<8 | Re_buf[6]))/32768.0*16;		//Z轴加速度
//						T    = ((short)(Re_buf[9]<<8 | Re_buf[8]))/340.0+36.25;		//温度
//						break;
//					case 0x52: //标识这个包是角速度包
//						w[0] = ((short)(Re_buf[3]<<8| Re_buf[2]))/32768.0*2000;		//X轴角速度
//						w[1] = ((short)(Re_buf[5]<<8| Re_buf[4]))/32768.0*2000;		//Y轴角速度
//						w[2] = ((short)(Re_buf[7]<<8| Re_buf[6]))/32768.0*2000;		//Z轴角速度
//						T    = ((short)(Re_buf[9]<<8| Re_buf[8]))/340.0+36.25;		//温度
//						break;
//					case 0x53: //标识这个包是角度包
//						angle[0] = ((short)(Re_buf[3]<<8| Re_buf[2]))/32768.0*180;	//X轴滚转角（x 轴）
//						angle[1] = ((short)(Re_buf[5]<<8| Re_buf[4]))/32768.0*180;	//Y轴俯仰角（y 轴）
//						angle[2] = ((short)(Re_buf[7]<<8| Re_buf[6]))/32768.0*180;	//Z轴偏航角（z 轴）
//						T        = ((short)(Re_buf[9]<<8| Re_buf[8]))/340.0+36.25;	//温度
//
//						//printf("X轴角度：%.2f   Y轴角度：%.2f   Z轴角度：%.2f\r\n",angle[0],angle[1],angle[2]);
//						printf("%.2f   %.2f   %.2f\r\n",angle[0],angle[1],angle[2]);
//						break;
//					default:  break;
//				}
//			}
			//printf("X角度：%.2f  Y角度：%.2f  Z角度：%.2f  X速度：%.2f  Y速度：%.2f  Z速度：%.2f\r\n",angle[0],angle[1],angle[2],w[0],w[1],w[2]);

//			if(temp_buf[0]==0x55)       //检查帧头
//			{  
//				temp_buf[0] = 0;
//				switch(temp_buf[1])
//				{
//					case 0x51: //标识这个包是加速度包
//						a[0] = ((short)(temp_buf[3]<<8 | temp_buf[2]))/32768.0*16;		//X轴加速度
//						a[1] = ((short)(temp_buf[5]<<8 | temp_buf[4]))/32768.0*16;		//Y轴加速度
//						a[2] = ((short)(temp_buf[7]<<8 | temp_buf[6]))/32768.0*16;		//Z轴加速度
//						T    = ((short)(temp_buf[9]<<8 | temp_buf[8]))/340.0+36.25;		//温度
//						break;
//					case 0x52: //标识这个包是角速度包
//						w[0] = ((short)(temp_buf[3]<<8| temp_buf[2]))/32768.0*2000;		//X轴角速度
//						w[1] = ((short)(temp_buf[5]<<8| temp_buf[4]))/32768.0*2000;		//Y轴角速度
//						w[2] = ((short)(temp_buf[7]<<8| temp_buf[6]))/32768.0*2000;		//Z轴角速度
//						T    = ((short)(temp_buf[9]<<8| temp_buf[8]))/340.0+36.25;		//温度
//						break;
//					case 0x53: //标识这个包是角度包
//						angle[0] = ((short)(temp_buf[3]<<8| temp_buf[2]))/32768.0*180;	//X轴滚转角（x 轴）
//						angle[1] = ((short)(temp_buf[5]<<8| temp_buf[4]))/32768.0*180;	//Y轴俯仰角（y 轴）
//						angle[2] = ((short)(temp_buf[7]<<8| temp_buf[6]))/32768.0*180;	//Z轴偏航角（z 轴）
//						T        = ((short)(temp_buf[9]<<8| temp_buf[8]))/340.0+36.25;	//温度
//
//						printf("X轴角度：%.2f   Y轴角度：%.2f   Z轴角度：%.2f\r\n",angle[0],angle[1],angle[2]);
//						break;
//					default:  break;
//				}
//			}
		}

		//printf("X轴角度：%.2f   Y轴角度：%.2f   Z轴角度：%.2f\r\n",angle[0],angle[1],angle[2]);
		delay_ms(50);
	}
}	 */

