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
      
    //ʹ�ܴ���3��ʱ�� �Ͷ�ӦGPIOʱ��  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
      
      
    //����TX����GPIO  
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;  
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;  
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;  
    GPIO_Init(GPIOA,&GPIO_InitStructure);  
      
    //����RX����GPIO  
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;  
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  
    GPIO_Init(GPIOA,&GPIO_InitStructure);  
      
    //���ô���2  
    USART_InitStructure.USART_BaudRate = bound;  
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  
    USART_InitStructure.USART_Parity = USART_Parity_No;  
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
    USART_Init(USART2, &USART_InitStructure);  
      
    //ʹ�ܴ���2  
    USART_Cmd(USART2,ENABLE);  
      
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  
  
    //���ô���2�����ж�  
  
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
      
    USART_ClearFlag(USART2, USART_FLAG_RXNE);  
      
    USART_GetFlagStatus(USART2,USART_FLAG_TC);     /* �ȶ�SR,��дDR */  
      
}  


void USART2_IRQHandler( void )  
 {  
     char temp = 0;  
#ifdef OS_TICKS_PER_SEC     //���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.  
    OSIntEnter();      
#endif        
       
     if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)  
     {  
  
         temp = USART_ReceiveData( USART2 );     /* ��ȡUSART2���ݣ��Զ������־λ RXNE */  
       
         if( RX2_Point <= 1999)  
         {  
               
             RX2_Temp[RX2_Point] = temp;  
               
             RX2_Point++;  
               
         }  
            else    RX2_Point=0;  
           
     }  
      
#ifdef OS_TICKS_PER_SEC     //���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.  
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
            RX2_Point = 0;      /* ����ָ�� */                                                        
            *len= ftemp;  
            return 1;   /* ɨ�赽���ݣ�����1 */               
        }  
        return 0;  
}
 
 
//��buf����õ���cx���������ڵ�λ��  
//����ֵ:0~0XFE,����������λ�õ�ƫ��.  
//       0XFF,�������ڵ�cx������     
unsigned char NMEA_Comma_Pos(unsigned char *buf,unsigned char cx)  
{                 
    unsigned char *p=buf;  
    while(cx)  
    {          
        if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//����'*'���߷Ƿ��ַ�,�򲻴��ڵ�cx������  
        if(*buf==',')cx--;  
        buf++;  
    }  
    return buf-p;      
}  
  
//m^n����  
//����ֵ:m^n�η�.  
unsigned int NMEA_Pow(unsigned char m,unsigned char n)  
{  
    unsigned int result=1;     
    while(n--)  
        result*=m;      
    return result;  
}  
  
//strת��Ϊ����,��','����'*'����  
//buf:���ִ洢��  
//dx:С����λ��,���ظ����ú���  
//����ֵ:ת�������ֵ  
int NMEA_Str2num(unsigned char *buf,unsigned char*dx)  
{  
    unsigned char *p=buf;  
    unsigned int ires=0,fres=0;  
    unsigned char ilen=0,flen=0,i;  
    unsigned char mask=0;  
    int res;  
    while(1) //�õ�������С���ĳ���  
    {  
        if(*p=='-'){mask|=0X02;p++;}//�Ǹ���  
        if(*p==','||(*p=='*'))break;//����������  
        if(*p=='.'){mask|=0X01;p++;}//����С������  
        else if(*p>'9'||(*p<'0')) //�зǷ��ַ�  
        {     
            ilen=0;  
            flen=0;  
            break;  
        }     
        if(mask&0X01)flen++;  
        else ilen++;  
        p++;  
    }  
    if(mask&0X02)buf++; //ȥ������  
    for(i=0;i<ilen;i++)  //�õ�������������  
    {    
        ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');  
    }  
    if(flen>5)flen=5;    //���ȡ5λС��  
    *dx=flen;           //С����λ��  
    for(i=0;i<flen;i++)  //�õ�С����������  
    {    
        fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');  
    }   
    res=ires*NMEA_Pow(10,flen)+fres;  
    if(mask&0X02)res=-res;           
    return res;  
}             
  

//����GPGGA��Ϣ  
//gpsx:nmea��Ϣ�ṹ��  
//buf:���յ���GPS���ݻ������׵�ַ  
void NMEA_GPGGA_Analysis(GPS_PacketTypeDef *GPS_Packet,u8 *buf)  
{     
    unsigned char *p1,dx,posx;  
    unsigned int temp;    
    float rs;  
    p1=(unsigned char *)strstr((const char *)buf,"$GPGGA");  //�˴��ɸ�Ϊ"$GPGGA"  
      
    posx=NMEA_Comma_Pos(p1,9);                              //�õ����θ߶�  
    if(posx!=0XFF){  
        GPS_Packet->altitude=NMEA_Str2num(p1+posx,&dx);   
    }  
    posx=NMEA_Comma_Pos(p1,2);                              //�õ�γ��  
    if(posx!=0XFF)  
    {  
        temp=NMEA_Str2num(p1+posx,&dx);            
        GPS_Packet->latitude=(float)(temp/NMEA_Pow(10,dx+2)); //�õ���  
        rs=temp%NMEA_Pow(10,dx+2);              //�õ�'          
        GPS_Packet->latitude=GPS_Packet->latitude/**NMEA_Pow(10,5)*/+(rs*NMEA_Pow(10,5-dx))/6000000;//ת��Ϊ��   
           
    }  
               
    posx=NMEA_Comma_Pos(p1,4);                              //�õ�����  
    if(posx!=0XFF)  
    {                                                   
        temp=NMEA_Str2num(p1+posx,&dx);            
        GPS_Packet->longitude=(float)(temp/NMEA_Pow(10,dx+2));    //�õ���  
        rs=temp%NMEA_Pow(10,dx+2);              //�õ�'          
        GPS_Packet->longitude=GPS_Packet->longitude/**NMEA_Pow(10,5)*/+(rs*NMEA_Pow(10,5-dx))/6000000;//ת��Ϊ��   
          
    }  
      
    posx=NMEA_Comma_Pos(p1,6);                              //��λ״̬  
    if(posx!=0XFF){  
        GPS_Packet->status=*(p1+posx)-'0';  
    }  
}  
  
void Ublox_Send_Date(unsigned char* dbuf,unsigned short len)  
{  
    unsigned short j;  
    for(j=0;j<len;j++)//ѭ����������  
    {  
        while((USART2->SR&0X40)==0);//ѭ������,ֱ���������     
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
//  MyPrintf("���GPS����\r\n");  
    p_start =(u8*)strstr((const char*)buff,"$GPRMC");  
    if(p_start!=NULL)               //�Ҳ�������λ����Ϣ��������Ч  
    {  
            if(strstr((const char*)p_start,"$GPVTG")!=NULL && strstr((const char*)p_start,"$GPGGA")!=NULL)  
                  
                    if(strstr((const char*)p_start,"$GPGSA")!=NULL && strstr((const char*)p_start,"$GPGSV")!=NULL)  
                          
                        if((p_process=(u8*)strstr((const char*)p_start,"$GPGLL"))!=NULL)  
                        {     
//                              MyPrintf("�����Ч�ַ�\r\n");  
                                if((strstr((const char*)p_process,"V"))!=NULL)  res=0xFF;  
                        }  
    }else   res=1;                    
      
//  MyPrintf("GPS Check Result=%d\r\n",res);  
    return res;                                     //������Ч��λ����  
}



