/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/

//#include <sys/types.h>

#if !defined(SOCKET_ERROR)
	/** error in socket operation */
	#define SOCKET_ERROR -1
#endif

#if defined(WIN32)
/* default on Windows is 64 - increase to make Linux and Windows the same */
#define FD_SETSIZE 1024
#include <winsock2.h>
#include <ws2tcpip.h>
#define MAXHOSTNAMELEN 256
#define EAGAIN WSAEWOULDBLOCK
#define EINTR WSAEINTR
#define EINVAL WSAEINVAL
#define EINPROGRESS WSAEINPROGRESS
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ENOTCONN WSAENOTCONN
#define ECONNRESET WSAECONNRESET
#define ioctl ioctlsocket
#define socklen_t int
#else
#define INVALID_SOCKET SOCKET_ERROR
/*#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>*/
#include <stdio.h>
/*#include <unistd.h>
#include <errno.h>
#include <fcntl.h>*/
#include <string.h>
#include <stdlib.h>
#endif

#if defined(WIN32)
#include <Iphlpapi.h>
#else
/*#include <sys/ioctl.h>
#include <net/if.h>*/
#endif

#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "delay.h"

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
static int mysock = INVALID_SOCKET;
int SIM900A_WaitCnt=0;
extern u8 USART1_RcvCpltFlag;
extern u8 USART1_DataBuf[200];
extern u8 Connect_Flag;

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	int rc = 0;
	int i;
	u8 SIM900A_TryCnt2=0;
	printf("AT+CIPSEND\r\n");
	while(USART1_RcvCpltFlag == 0 || USART1_DataBuf[14]!='>')
	{
		SIM900A_TryCnt2++;
		delay_ms(1);
		if(SIM900A_TryCnt2 == 100) return -1;
	}
	USART1_RcvCpltFlag = 0;
	SIM900A_TryCnt2=0;
	if(USART1_DataBuf[6]=='O') 
	{
		USART1_DataBuf[0]=255;
	}
	for(i=0;i<buflen;i++) 
	{
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) *(buf+i); 
		delay_ms(20);		
	}
	while((USART1->SR&0X40)==0);
	USART1->DR = 0x1a;
	while(USART1_RcvCpltFlag != 1 );
	USART1_RcvCpltFlag = 0;
	while(USART1_RcvCpltFlag != 1 );
//	USART1_RcvCpltFlag = 0;
//	for(i=0;i<200-11;i++) USART1_DataBuf[i] = USART1_DataBuf[i+11];
	return 0;
}

int Read_BufLen = 0;

int recv(u8 *buf,int count)
{
	if(USART1_RcvCpltFlag == 1)
	{
		if(memcmp(USART1_DataBuf,"\r\nSEND OK\r\n",11)==0)
		{
			memcpy(buf,&USART1_DataBuf[11],Read_BufLen-11);
			return Read_BufLen-11;
		}
		else
		{
			memcpy(buf,USART1_DataBuf,Read_BufLen);
			return Read_BufLen;
		}
	}
}

int transport_getdata(unsigned char* buf, int count)
{
	
	//printf("received %d bytes count %d\n", rc, (int)count);
	while(USART1_RcvCpltFlag != 1 );
	//memcpy(buf, USART1_DataBuf, Read_BufLen-11);
	recv(buf,1);
 // Read_BufLen += count;
	return count;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	int sock = *((int *)sck); 	/* sck: pointer to whatever the system may use to identify the transport */
	/* this call will return after the timeout set on initialization if no bytes;
	   in your system you will use whatever you use to get whichever outstanding
	   bytes your socket equivalent has ready to be extracted right now, if any,
	   or return immediately */
	int rc/* = recv(sock, buf, count, 0)*/;	
	if (rc == -1) {
		/* check error conditions from your system here, and return -1 */
		return 0;
	}
	return count;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(char* addr, int port)
{
	int SIM900A_TryCnt1=0;
	int SIM900A_TryCnt2=0;
	delay_ms(1000);
	while(SIM900A_TryCnt1 < 20)
	{
		printf("AT\r\n");
		delay_ms(100);
		while(USART1_RcvCpltFlag == 0)
		{
			SIM900A_TryCnt2++;
			delay_ms(1);
			if(SIM900A_TryCnt2 == 500) return -1;
		}
		USART1_RcvCpltFlag = 0;
		SIM900A_TryCnt2=0;
		if(USART1_DataBuf[6]=='O') 
		{
			SIM900A_TryCnt1 = 0;
			USART1_DataBuf[0]=255;
			break;
		}
		SIM900A_TryCnt1++;
		delay_ms(1000);
	}
	if(SIM900A_TryCnt1>=20) return -1;
	
	delay_ms(1000);
	
	while(SIM900A_TryCnt1 < 1)
	{
		printf("AT+CGCLASS=\"B\"\r\n");
		delay_ms(100);
		while(USART1_RcvCpltFlag == 0)
		{
			SIM900A_TryCnt2++;
			delay_ms(1);
			if(SIM900A_TryCnt2 == 500) return -1;
		}
		USART1_RcvCpltFlag = 0;
		SIM900A_TryCnt2=0;
		if(USART1_DataBuf[18]=='O') 
		{
			SIM900A_TryCnt1 = 0;
			USART1_DataBuf[0]=255;
			break;
		}
		SIM900A_TryCnt1++;
		delay_ms(1000);
	}
	if(SIM900A_TryCnt1>=1) return -1;
	
	delay_ms(1000);
	
	while(SIM900A_TryCnt1 < 1)
	{
		printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
		delay_ms(100);
		while(USART1_RcvCpltFlag == 0)
		{
			SIM900A_TryCnt2++;
			delay_ms(1);
			if(SIM900A_TryCnt2 == 500) return -1;
		}
		USART1_RcvCpltFlag = 0;
		SIM900A_TryCnt2=0;
		if(USART1_DataBuf[29]=='O') 
		{
			SIM900A_TryCnt1 = 0;
			USART1_DataBuf[0]=255;
			break;
		}
		SIM900A_TryCnt1++;
		delay_ms(1000);
	}
	if(SIM900A_TryCnt1>=1) return -1;
	
	delay_ms(1000);
	
	while(SIM900A_TryCnt1 < 1)
	{
		printf("AT+CGATT=1\r\n");
		delay_ms(100);
		while(USART1_RcvCpltFlag == 0)
		{
			SIM900A_TryCnt2++;
			delay_ms(1);
			if(SIM900A_TryCnt2 == 500) return -1;
		}
		USART1_RcvCpltFlag = 0;
		SIM900A_TryCnt2=0;
		if(USART1_DataBuf[14]=='O') 
		{
			SIM900A_TryCnt1 = 0;
			USART1_DataBuf[0]=255;
			break;
		}
		SIM900A_TryCnt1++;
		delay_ms(1000);
	}
	if(SIM900A_TryCnt1>=1) return -1;
	
	delay_ms(1000);
	
	while(SIM900A_TryCnt1 < 1)
	{
		printf("AT+CIPCSGP=1,\"CMNET\"\r\n");
		delay_ms(100);
		while(USART1_RcvCpltFlag == 0)
		{
			SIM900A_TryCnt2++;
			delay_ms(1);
			if(SIM900A_TryCnt2 == 500) return -1;
		}
		USART1_RcvCpltFlag = 0;
		SIM900A_TryCnt2=0;
		if(USART1_DataBuf[24]=='O') 
		{
			SIM900A_TryCnt1 = 0;
			USART1_DataBuf[0]=255;
			break;
		}
		SIM900A_TryCnt1++;
		delay_ms(1000);
	}
	if(SIM900A_TryCnt1>=1) return -1;
	
	delay_ms(1000);
	
	while(SIM900A_TryCnt1 < 1)
	{
		printf("AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n",addr,port);
		delay_ms(100);
		while(USART1_RcvCpltFlag == 0)
		{
			SIM900A_TryCnt2++;
			delay_ms(1);
			if(SIM900A_TryCnt2 == 500) return -1;
		}
		USART1_RcvCpltFlag = 0;
		SIM900A_TryCnt2=0;
		if(USART1_DataBuf[46]=='O' || USART1_DataBuf[47]=='O' || USART1_DataBuf[45]=='O' 
			|| USART1_DataBuf[44]=='O' || USART1_DataBuf[43]=='O' || USART1_DataBuf[47]=='O') 
		{
			SIM900A_TryCnt1 = 0;
			USART1_DataBuf[0]=255;
			break;
		}
		SIM900A_TryCnt1++;
		delay_ms(1000);
	}
	if(SIM900A_TryCnt1>=1) return -1;
	
	delay_ms(100);
	
	while(USART1_RcvCpltFlag == 0);
	USART1_RcvCpltFlag = 0;
	if(USART1_DataBuf[10]!='O') return -1;
	
	mysock = 0;
	return mysock;
}

int transport_close(int sock)
{
	int rc;
	int SIM900A_TryCnt1=0;
	int SIM900A_TryCnt2=0;
	if(Connect_Flag == 1 )
	{
		while(SIM900A_TryCnt1 < 10)
		{
			printf("AT+CIPCLOSE=1\r\n");
			delay_ms(100);
			while(USART1_RcvCpltFlag == 0)
			{
				SIM900A_TryCnt2++;
				delay_ms(1);
				if(SIM900A_TryCnt2 == 100) return -1;
			}
			USART1_RcvCpltFlag = 0;
			SIM900A_TryCnt2=0;
			if(USART1_DataBuf[23]=='O') 
			{
				SIM900A_TryCnt1 = 0;
				USART1_DataBuf[0]=255;
				break;
			}
			SIM900A_TryCnt1++;
			delay_ms(1000);
		}
		if(SIM900A_TryCnt1>=10) return -1;
		Connect_Flag = 0;
		delay_ms(500);
	}
	
	while(SIM900A_TryCnt1 < 10)
	{
		printf("AT+CIPSHUT\r\n");
		delay_ms(100);
		while(USART1_RcvCpltFlag == 0)
		{
			SIM900A_TryCnt2++;
			delay_ms(1);
			if(SIM900A_TryCnt2 == 100) return -1;
		}
		USART1_RcvCpltFlag = 0;
		SIM900A_TryCnt2=0;
		if(USART1_DataBuf[19]=='O') 
		{
			SIM900A_TryCnt1 = 0;
			USART1_DataBuf[0]=255;
			break;
		}
		SIM900A_TryCnt1++;
		delay_ms(1000);
	}
	if(SIM900A_TryCnt1>=10) return -1;
	
	return rc;
}
