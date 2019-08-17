 
#include "DataScope_DP.h"


//函数说明：将单精度浮点数据转成4字节数据并存入指定地址 
//附加说明：用户无需直接操作此函数 
//target:目标单精度数据
//buf:待写入数组
//beg:指定从数组第几个元素开始写入
//函数无返回 
void Float2Byte(float *target,unsigned char *buf,unsigned char beg)
{
    unsigned char *point;
    point = (unsigned char*)target;	  //得到float的地址
    buf[beg]   = point[0];
    buf[beg+1] = point[1];
    buf[beg+2] = point[2];
    buf[beg+3] = point[3];
}
 
 
//函数说明：将待发送通道的单精度浮点数据写入发送缓冲区
//Data：通道数据
//Channel：选择通道（1-7）
//函数无返回 
void DataScope_Get_Channel_Data(unsigned char *buf,float Data,unsigned char Channel)
{
	if ( (Channel > 7) || (Channel == 0) ) return;  //通道个数大于10或等于0，直接跳出，不执行函数
  else
  {
     switch (Channel)
		{
	    case 1:  Float2Byte(&Data,buf,1); break;
	    case 2:  Float2Byte(&Data,buf,5); break;
		  case 3:  Float2Byte(&Data,buf,9); break;
		  case 4:  Float2Byte(&Data,buf,13); break;
		  case 5:  Float2Byte(&Data,buf,17); break;
		  case 6:  Float2Byte(&Data,buf,21); break;
		  case 7:  Float2Byte(&Data,buf,25); break;
		}
  }	 
}


//函数说明：生成 DataScopeV1.0 能正确识别的帧格式
//Channel_Number，需要发送的通道个数
//返回发送缓冲区数据个数
//返回0表示帧格式生成失败 
unsigned char DataScope_Data_Generate(unsigned char *buf,unsigned char Channel_Number)
{
	if ( (Channel_Number > 7) || (Channel_Number == 0) ) { return 0; }  //通道个数大于10或等于0，直接跳出，不执行函数
  else
  {	
		char i,j=(Channel_Number*4+1);
		
		buf[0] = '$';  //帧头
		for(i=31;i>j;i--)
			buf[i]='\0';
			
		switch(Channel_Number)   
		{ 
			case 1:   buf[5]  =  5; return  6;  
			case 2:   buf[9]  =  9; return 10;
			case 3:   buf[13] = 13; return 14; 
			case 4:   buf[17] = 17; return 18;
			case 5:   buf[21] = 21; return 22;  
			case 6:   buf[25] = 25; return 26;
			case 7:   buf[29] = 29; return 30; 
		}
  }
	return 0;
}











