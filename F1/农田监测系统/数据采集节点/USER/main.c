#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "DataScope_DP.h"
#include "usart.h"	
#include "ds18b20.h"	
#include "BMP180.h"
#include "my_ADC.h"
#include "dht11.h"
#include "GPS.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MQTTPacket.h"
#include "transport.h"

/****数据输出格式*************
A：海拔（整数）
B：纬度（有小数）
C：经度（有小数）
D：GPS状态（整数）
E：温度（有小数）
F：湿度（有小数）
G：ADC_PA0 0-4096
H：ADC_PA1 0-4096
I：ADC_PA4 0-4096
J：ADC_PA5 0-4096
K：ADC_PA6 0-4096
L：ADC_PA7 0-4096
$：终止
***********************/


u16 temperature, shidu;
//u16 value[M];
extern DHT11_TypeDef DHT11;	
extern uint16_t ADC_ConvertedValue;
extern u16 lenth;
extern u8 RX2_Temp[2000];
extern u16 RX2_Point ;
extern GPS_PacketTypeDef GPS_Data;
extern u16 After_filter[M]; //用来存放求平均值之后的结果
extern u8 USART1_RcvCpltFlag;
extern u8 USART1_RcvNewByteFlag;
extern u8 USART1_DataBuf[200];
u8 Connect_Flag = 0;
enum states { READING, PUBLISHING };
u8  circle_flag=0;


int main(void)
{ 
	u8 buf[100]="ABC";
	int rc = 0;
	int mysock = 0;
	int buflen = sizeof(buf);
	int msgid = 1;
	MQTTString topicString = MQTTString_initializer;
	int req_qos = 0;
	char payload[200];
	int payloadlen = strlen(payload);
	int len = 0;
//	char *host = "123.206.219.144";
//	int port = 9999;
	char *host = "219.219.33.121";
	int port = 1883;
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	
//	payload = malloc(100);
	
	delay_init();                 //延时初始化
	uart_init(115200);
	Init_ADC_Periph();
	DHT11_Init();	
	USART2_Init_GPS(9600);
	sys_pulse();
	
	//	GSM模块启动和连接
	while(transport_open(host, port) !=0)
	{
		delay_ms(1000);
		Connect_Flag = 0;
		transport_close(mysock);
		delay_ms(1000);
	}
	Connect_Flag = 1;
	
	//设置参数
	data.clientID.cstring = "me";
	data.keepAliveInterval = 100;
	data.cleansession = 1;
	data.username.cstring = "usr";
	data.password.cstring = "pwd";
	//发送参数
	len = MQTTSerialize_connect(buf, buflen, &data);
	rc = transport_sendPacketBuffer(mysock, buf, len);
	
	//订阅主题
	topicString.cstring = "control_code";
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);
	rc = transport_sendPacketBuffer(mysock, buf, len);
	
	//发布主题
	topicString.cstring = "field_data";
	
	
	while(1)
	{
		if( USART2_Scan(&lenth)==1 )
		{		
				NMEA_GPGGA_Analysis(&GPS_Data, RX2_Temp);  
		}
		
		if(circle_flag == 1)
		{
				circle_flag =0; //5s间隔
				DHT11_ReadData();
				filter();
				sprintf(payload,"A%d,B%f,C%f,D%d,E%.2f,F%.2f,G%d,H%d",GPS_Data.altitude,GPS_Data.latitude,GPS_Data.longitude,GPS_Data.status,
					(float)((float)DHT11.Tem_H+(float)DHT11.Tem_L/100.0),(float)((float)DHT11.Hum_H+(float)DHT11.Hum_L/100.0),
					After_filter[0],After_filter[1]);
				
				len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, strlen(payload));
				rc = transport_sendPacketBuffer(mysock, buf, len);
		}
		delay_ms(200);
		
	}
}

