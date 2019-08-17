#include "delay.h"
#include "sys.h"
#include "math.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "usart.h"	
#include "GPS.h"
#include "string.h"


u8 RX2_Temp[2000];
u16 RX2_Point ;
u16 lenth;
GPS_PacketTypeDef GPS_Data;

void USART2_Init_GPS( u32 bound )  
{  
      
    GPIO_InitTypeDef  GPIO_InitStructure;  
    USART_InitTypeDef USART_InitStructure;  
    NVIC_InitTypeDef  NVIC_InitStructure;  
      
    USART_DeInit(USART2);  
      
    //使能串口3的时钟 和对应GPIO时钟  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
      
      
    //配置TX引脚GPIO  
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;  
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;  
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;  
    GPIO_Init(GPIOA,&GPIO_InitStructure);  
      
    //配置RX引脚GPIO  
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;  
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  
    GPIO_Init(GPIOA,&GPIO_InitStructure);  
      
    //配置串口2  
    USART_InitStructure.USART_BaudRate = bound;  
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  
    USART_InitStructure.USART_Parity = USART_Parity_No;  
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
    USART_Init(USART2, &USART_InitStructure);  
      
    //使能串口2  
    USART_Cmd(USART2,ENABLE);  
      
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  
  
    //配置串口2接收中断  
  
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
      
    USART_ClearFlag(USART2, USART_FLAG_RXNE);  
      
    USART_GetFlagStatus(USART2,USART_FLAG_TC);     /* 先读SR,再写DR */  
      
}  


void USART2_IRQHandler( void )  
 {  
     char temp = 0;  
#ifdef OS_TICKS_PER_SEC     //如果时钟节拍数定义了,说明要使用ucosII了.  
    OSIntEnter();      
#endif        
       
     if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)  
     {  
  
         temp = USART_ReceiveData( USART2 );     /* 读取USART2数据，自动清零标志位 RXNE */  
       
         if( RX2_Point <= 1999)  
         {  
               
             RX2_Temp[RX2_Point] = temp;  
               
             RX2_Point++;  
               
         }  
            else    RX2_Point=0;  
           
     }  
      
#ifdef OS_TICKS_PER_SEC     //如果时钟节拍数定义了,说明要使用ucosII了.  
OSIntExit();                                               
#endif    
       
 } 
 
 
unsigned short USART2_Scan(u16 *len)  
{  
        unsigned short int ftemp = 0;  
        ftemp = RX2_Point;  
        *len=0;  
        if( ftemp != 0 )  
        {  
            delay_ms(100);  
            while ( ftemp != RX2_Point )  
            {  
                 ftemp = RX2_Point;  
                 delay_ms(100);  
            }  
            RX2_Point = 0;      /* 重置指针 */                                                        
            *len= ftemp;  
            return 1;   /* 扫描到数据，返回1 */               
        }  
        return 0;  
}
 
 
//从buf里面得到第cx个逗号所在的位置  
//返回值:0~0XFE,代表逗号所在位置的偏移.  
//       0XFF,代表不存在第cx个逗号     
unsigned char NMEA_Comma_Pos(unsigned char *buf,unsigned char cx)  
{                 
    unsigned char *p=buf;  
    while(cx)  
    {          
        if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号  
        if(*buf==',')cx--;  
        buf++;  
    }  
    return buf-p;      
}  
  
//m^n函数  
//返回值:m^n次方.  
unsigned int NMEA_Pow(unsigned char m,unsigned char n)  
{  
    unsigned int result=1;     
    while(n--)  
        result*=m;      
    return result;  
}  
  
//str转换为数字,以','或者'*'结束  
//buf:数字存储区  
//dx:小数点位数,返回给调用函数  
//返回值:转换后的数值  
int NMEA_Str2num(unsigned char *buf,unsigned char*dx)  
{  
    unsigned char *p=buf;  
    unsigned int ires=0,fres=0;  
    unsigned char ilen=0,flen=0,i;  
    unsigned char mask=0;  
    int res;  
    while(1) //得到整数和小数的长度  
    {  
        if(*p=='-'){mask|=0X02;p++;}//是负数  
        if(*p==','||(*p=='*'))break;//遇到结束了  
        if(*p=='.'){mask|=0X01;p++;}//遇到小数点了  
        else if(*p>'9'||(*p<'0')) //有非法字符  
        {     
            ilen=0;  
            flen=0;  
            break;  
        }     
        if(mask&0X01)flen++;  
        else ilen++;  
        p++;  
    }  
    if(mask&0X02)buf++; //去掉负号  
    for(i=0;i<ilen;i++)  //得到整数部分数据  
    {    
        ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');  
    }  
    if(flen>5)flen=5;    //最多取5位小数  
    *dx=flen;           //小数点位数  
    for(i=0;i<flen;i++)  //得到小数部分数据  
    {    
        fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');  
    }   
    res=ires*NMEA_Pow(10,flen)+fres;  
    if(mask&0X02)res=-res;           
    return res;  
}             
  

//分析GPGGA信息  
//gpsx:nmea信息结构体  
//buf:接收到的GPS数据缓冲区首地址  
void NMEA_GPGGA_Analysis(GPS_PacketTypeDef *GPS_Packet,u8 *buf)  
{     
    unsigned char *p1,dx,posx;  
    unsigned int temp;    
    float rs;  
    p1=(unsigned char *)strstr((const char *)buf,"$GPGGA");  //此处可改为"$GPGGA"  
      
    posx=NMEA_Comma_Pos(p1,9);                              //得到海拔高度  
    if(posx!=0XFF){  
        GPS_Packet->altitude=NMEA_Str2num(p1+posx,&dx);   
    }  
    posx=NMEA_Comma_Pos(p1,2);                              //得到纬度  
    if(posx!=0XFF)  
    {  
        temp=NMEA_Str2num(p1+posx,&dx);            
        GPS_Packet->latitude=(float)(temp/NMEA_Pow(10,dx+2)); //得到°  
        rs=temp%NMEA_Pow(10,dx+2);              //得到'          
        GPS_Packet->latitude=GPS_Packet->latitude/**NMEA_Pow(10,5)*/+(rs*NMEA_Pow(10,5-dx))/6000000;//转换为°   
           
    }  
               
    posx=NMEA_Comma_Pos(p1,4);                              //得到经度  
    if(posx!=0XFF)  
    {                                                   
        temp=NMEA_Str2num(p1+posx,&dx);            
        GPS_Packet->longitude=(float)(temp/NMEA_Pow(10,dx+2));    //得到°  
        rs=temp%NMEA_Pow(10,dx+2);              //得到'          
        GPS_Packet->longitude=GPS_Packet->longitude/**NMEA_Pow(10,5)*/+(rs*NMEA_Pow(10,5-dx))/6000000;//转换为°   
          
    }  
      
    posx=NMEA_Comma_Pos(p1,6);                              //定位状态  
    if(posx!=0XFF){  
        GPS_Packet->status=*(p1+posx)-'0';  
    }  
}  
  
void Ublox_Send_Date(unsigned char* dbuf,unsigned short len)  
{  
    unsigned short j;  
    for(j=0;j<len;j++)//循环发送数据  
    {  
        while((USART2->SR&0X40)==0);//循环发送,直到发送完毕     
        USART2->DR=dbuf[j];    
    }     
}  
  
  
u8 GPS_Check(u8 *buff)  
{  
    u8 *p_start,*p_process;  
    //,*p_end;  
//  u8 temp[500];  
    u8 res=0;  
//  u16 cnt=0,i=0;  
//  MyPrintf("检查GPS数据\r\n");  
    p_start =(u8*)strstr((const char*)buff,"$GPRMC");  
    if(p_start!=NULL)               //找不到地理位置信息，返回无效  
    {  
            if(strstr((const char*)p_start,"$GPVTG")!=NULL && strstr((const char*)p_start,"$GPGGA")!=NULL)  
                  
                    if(strstr((const char*)p_start,"$GPGSA")!=NULL && strstr((const char*)p_start,"$GPGSV")!=NULL)  
                          
                        if((p_process=(u8*)strstr((const char*)p_start,"$GPGLL"))!=NULL)  
                        {     
//                              MyPrintf("检查无效字符\r\n");  
                                if((strstr((const char*)p_process,"V"))!=NULL)  res=0xFF;  
                        }  
    }else   res=1;                    
      
//  MyPrintf("GPS Check Result=%d\r\n",res);  
    return res;                                     //返回无效定位数据  
}



