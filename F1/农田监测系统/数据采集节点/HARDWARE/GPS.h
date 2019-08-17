#ifndef __GPS_H_
#define __GPS_H_

#include "stm32f10x.h"


typedef struct
{
	int altitude;
	float latitude;
	float longitude;
	int status;
}GPS_PacketTypeDef;

void NMEA_GPGGA_Analysis(GPS_PacketTypeDef *GPS_Packet,unsigned char *buf);  
 unsigned short USART2_Scan(u16 *len)  ;
 void USART2_Init_GPS( u32 bound )  ;
int NMEA_Str2num(unsigned char *buf,unsigned char*dx)  ;
unsigned int NMEA_Pow(unsigned char m,unsigned char n)  ;
unsigned char NMEA_Comma_Pos(unsigned char *buf,unsigned char cx)  ;
u8 GPS_Check(u8 *buff)  ;

#endif


