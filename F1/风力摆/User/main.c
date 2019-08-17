
/*****程序备忘记录***********************************
		方位角读取来自于串口mpu6050，发送速率100/s
		定点控制全部基于PID控制，但稍做了修改，为了取得更好的定位效果，
取消了比例控制项，取而代之的是提前测量的每隔0.5或1厘米稳定的风扇输入PWM值，
最终输出量为：（基于所需坐标的马达pwm值+积分输出+微分输出），取得了标胶好的定位效果。
		画线，画圆，写数字等功能全部基于定点控制，
		几个主要且重要的全局变量，即当前x坐标，当前y坐标，设定x坐标，设定y坐标，设定当前模式
		为调试方便，所有输出与控制全部通过计算机串口完成，未设置显示屏与按键
		生怕未来看不懂，所以几乎所有变量在第一次定义的时候都在后面加了注解。同时，这个程序还有很大的优化改进空间，
比如中断函数.c文件中的一些flag(sign)之前定义了，但是后来似乎没什么用了。
		第一次写一个完整的工程，所以很多函数的位置分配出现了很大的问题，不知道该放到哪个c文件夹里。
		关于具体控制，启动后先等6050模块暂时预热，数值稳定下来，然后输入  j  进行归零。随后输入模式代号接数据，
系统便开始工作，各模式代码与数据格式如下：
一.定点：|| p12045 || 其中，p为point，12为极坐标下半径值，045为极坐标下角度值；
二.画线：|| l15045 || 其中，l为line，15和045含义同上；
三.画圆：|| c15 || 其中，c为circle，15为半径；  （画圆函数中期望为输入半径乘1.15，不过感觉乘的还要稍微小一点）
四.写数字：|| w5 || 其中，w为write，5为要写的数字，这个程序现在只编写了0到7的数字码；
五.pid系数在线调节：|| +10520 || 或 || -10520 || 其中，+与-表示当前值加上或减去输入数据，
	输入数据分为三部分：10代表kp，单位为1，5代表ki，单位为0.01，20代表kd，单位为1
六.风扇pwm输入与推动距离测试：|| s1560 || 其中，s为set，1560代表输入pwm值1560（最高3000）
		控制的总体架构大概可以描述为：每读取一次6050姿态数据（0.01s），即进入一次姿态的pid计算控制（每秒100次），
所有的控制都在中断函数中完成，总体套路是：每经过0.01s通过switch跳到对应模式，然后进行一次定点pid计算并将输出
pwm值直接传递给被控风扇，期间每隔一定毫秒（实现的方式是通过另一个变量记录进入进入当前中断次数，每进一次都是很
精确的0.01s，所以间隔时间可以由进入次数来确定），通过一个“ChangePoint”函数改变set_x和set_y，因为是全局变量，所
以改变后的设定值马上可以在pid计算中产生效果。
		之前在建立模型的时候发现了很好的一点，如果将电机十字形放置，那么x方向和y方向可以被单独控制，所有的二维平面
运动这样一来都转变为一维了，所以只用了两组pid，同时由于重力的帮助，在每个象限只要开两个马达就好了，之所以不四
个全开，主要还是这样一来编程又会复杂很多，当然之所以不用oled屏幕和键盘，主要也是因为临近期末时间紧，所以就不弄
了。		
		编译的时候一大堆警告全是来自于一个字符串转数字的函数，没有关系，不用睬
******************2016年6月16日***********************/




#include "stm32f10x.h"
#include "pwm.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include "mpu6050.h"
#include "moto.h"
#include "control.h"

//#define H (50)  节点高 宏定义在control.h中

/**********************///mpu6050数据读取，中断
extern float cur_x,cur_y,cur_z;
/**********************///电脑发送控制中断
extern int set_a,set_r;
extern u8 set_mode;
/*调试*/
extern float set_x,set_y;
extern u8 set_quadrant,counter;

extern struct
{
	float SumError;
	float LastError;
	float Proportion;
	float Integral;
	float Derivative;	
}PID_X,PID_Y;

/*设定值的定义有 set_a , set_r , set_x , set_y , set_quadrant ,set_mode*/
/*当前值的定义有 cur_x , cur_y */



int main(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	delay_init();            //初始化延时函数	
	pwm_init();			        //TIM2
	usart1_init();         //初始化 配置USART1		(电脑通信)
  usart3_init();        //初始化 配置USART2 （用于接收MPU6050数据）
  NVIC_init();         //中断优先级初始化
	pid_config();
	
	/****PWM测试**************/
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);     
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
/******************************************/
	  
  while (1)
  {
		if(set_mode=='p'||set_mode=='w')
			printf("X:%.2f||Y:%.2f||L:%d,%d||SX%.2f,SY%.2f||KP%.0fKI%.2fKD%.0f\r\n",cur_x,cur_y,set_r,set_a,set_x,set_y,PID_X.Proportion,PID_X.Integral,PID_X.Derivative);
		else if(set_mode=='c')
			printf("X:%.2f||Y:%.2f||R=%d||%.2f||%.2f\r\n",cur_x,cur_y,set_r,PID_X.Proportion,PID_X.Derivative);
		else
			printf("X:%.2f||Y:%.2f||KP%.0fKI%.2fKD%.0f\r\n",cur_x,cur_y,PID_X.Proportion,PID_X.Integral,PID_X.Derivative);
		
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0)
		{
			delay_ms(10);
			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0)
			{
				PID_Y.Integral+=0.02;
				PID_X.Integral+=0.02;
				while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0);
			}
		}
		delay_ms(300);
	}
	
}
