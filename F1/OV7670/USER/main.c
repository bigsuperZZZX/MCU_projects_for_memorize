#include <stm32f10x.h>
#include <stdio.h>
#include <Nvic_Exit.h>
#include <delay.h>
#include <tft.h>
#include <ov7670.h>
#include <I2C.h>

/*
#define Cmos_VSYNC PCout(4)
#define Cmos_CS   PAout(0)
#define WriteControl PCout(13)
#define FIFO_WR    PCout(10) 
#define FIFO_WRST  PAout(13)
#define FIFO_RCK   PAout(2)
#define FIFO_RRST  PAout(15)
#define FIFO_OE	   PAout(14)
*/

typedef enum {TRUE = 0, FALSE = !TRUE} bool; 

//void RCC_Configuration(void);
void IO_Init(void);
//void CLK_generate(void);

int main()
{ 
	//RCC_Configuration();;//系统时钟设置
	delay_init();		//延时初始化
//	GPIOA->ODR ^= (1 <<3);						//LED1闪烁一次	   
	IO_Init(); 
//	LCD_Init();
/*	LCD_Clear(BLUE);       //清屏 
	POINT_COLOR=YELLOW;
	BACK_COLOR=BLUE;
   	LCD_ShowString(20,50,"QF Welcom");
	LCD_ShowString(20,100,"STM32 Camera");
	LCD_ShowString(20,150,"OV7670 Initing......");*/
	while(1!=Cmos7670_init());   //CMOS初始化	
	Exit_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_IPU, EXTI_Trigger_Falling, 2, 5);	//VSYNC	 	
  	while(1) 
	{		  		
	}
}

/*******************************************************************************
* Function Name  : RCC_Configuration 
* Description    : RCC select(external 8MHz)
* Input          : NO
* Output         : NO
* Return         : NO
*******************************************************************************/
/*void RCC_Configuration(void)
{ 

  ErrorStatus HSEStartUpStatus;
  RCC_DeInit();
  RCC_HSEConfig(RCC_HSE_ON);   
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)	  
  {
  
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  
    RCC_PCLK2Config(RCC_HCLK_Div1);		   
	RCC_PCLK1Config(RCC_HCLK_Div2);	 	  
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);	
    RCC_PLLCmd(ENABLE); 
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)	  
       {
       }
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
    while(RCC_GetSYSCLKSource() != 0x08)	  
       { 
       }
     }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE); 
}

void CLK_generate(void)
{
	GPIO_InitTypeDef GPIO_InitStructure1;
 	RCC_ClocksTypeDef RCC_Clocks;   	
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_AF_PP ; 
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure1);  	
	RCC_GetClocksFreq(&RCC_Clocks);  
	RCC_MCOConfig(RCC_MCO_HSE);   

}*/

void IO_Init(void)
{

  GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOD3,6
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //推挽输出
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOD3,6
		    
  // 	JTAG_Set(JTAG_SWD_DISABLE);	 //JTAG功能禁止，复用JTAG端口
}
