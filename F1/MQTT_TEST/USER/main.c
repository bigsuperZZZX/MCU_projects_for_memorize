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
#include "stm32f10x_tim.h"
#include "transport.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MQTTPacket.h"
#include "transport.h"

unsigned char i,j;          //计数变量
extern u8 USART1_RcvCpltFlag;
extern u8 USART1_RcvNewByteFlag;
extern u8 USART1_DataBuf[200];
u8 Connect_Flag = 0;
enum states { READING, PUBLISHING };

int main(void)
{ 
	
		
	u8 buf[200]="ABC";
	int rc = 0;
	int mysock = 0;
	int buflen = sizeof(buf);
	int msgid = 1;
	MQTTString topicString = MQTTString_initializer;
	int req_qos = 0;
	char* payload = "AAA";
	int payloadlen = strlen(payload);
	int len = 0;
//	char *host = "123.206.219.144";
//	int port = 9999;
	char *host = "219.219.33.121";
	int port = 1883;
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	
//	recv(USART1_DataBuf,10);
	delay_init();                 //延时初始化
	uart_init(115200);   //用到了定时器3的50ms中断
	sys_pulse();
	
	
	while(transport_open(host, port) !=0)
	{
		delay_ms(1000);
		Connect_Flag = 0;
		transport_close(mysock);
		delay_ms(1000);
	}
	Connect_Flag = 1;
	if(mysock < 0)
		return mysock;

//	printf("Sending to hostname %s port %d\n", host, port);

	data.clientID.cstring = "me";
	data.keepAliveInterval = 100;
	data.cleansession = 1;
	data.username.cstring = "usr";
	data.password.cstring = "pwd";
	

	len = MQTTSerialize_connect(buf, buflen, &data);
	rc = transport_sendPacketBuffer(mysock, buf, len);

	/* wait for connack */
	/*if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;

		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
		{
//			printf("Unable to connect, return code %d\n", connack_rc);
			goto exit;
		}
	}
	else
		goto exit; */

	
	/* subscribe */
	topicString.cstring = "control_code";
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

	rc = transport_sendPacketBuffer(mysock, buf, len);
/*	if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 	 //wait for suback 
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;

		rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
		if (granted_qos != 0)
		{
//			printf("granted qos != 0, %d\n", granted_qos);
			goto exit;
		}
	}
	else
		goto exit;*/

	/* loop getting msgs on subscribed topic */
	topicString.cstring = "control_code";
	while (1)
	{
		/* transport_getdata() has a built-in 1 second timeout,
		your mileage will vary */
		if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
		{
			unsigned char dup;
			int qos;
			unsigned char retained;
			unsigned short msgid;
			int payloadlen_in;
			unsigned char* payload_in;
			int rc;
			MQTTString receivedTopic;

			rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, &payloadlen_in, buf, buflen);
//			printf("message arrived %.*s\n", payloadlen_in, payload_in);
		}

//		printf("publishing reading\n");
		len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
		rc = transport_sendPacketBuffer(mysock, buf, len);
		
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
	}

//	printf("disconnecting\n");
	len = MQTTSerialize_disconnect(buf, buflen);
	rc = transport_sendPacketBuffer(mysock, buf, len);

exit:
	transport_close(mysock);

	
	
	while(1);
	
	
	while(1)
	{
		transport_sendPacketBuffer(123, buf, 10);
		delay_ms(1000);
	//	transport_close(100);
		delay_ms(1000);
		delay_ms(1000);
	}
}

