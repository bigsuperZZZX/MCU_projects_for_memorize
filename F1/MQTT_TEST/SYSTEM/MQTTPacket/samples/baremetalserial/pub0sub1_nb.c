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
 *    Sergio R. Caprile - port to the bare metal environment
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MQTTPacket.h"
#include "transport.h"

/* This is in order to get an asynchronous signal to stop the sample,
as the code loops waiting for msgs on the subscribed topic.
Your actual code will depend on your hw and approach, but this sample can be
run on Linux so debugging of the non-hardware specific bare metal code is easier.
See at bottom of file for details */
//#include <signal.h>

int toStop = 0;

enum states { READING, PUBLISHING };

int main(int argc, char *argv[])
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	int rc = 0;
	int mysock = 0;
	unsigned char buf[200];
	int buflen = sizeof(buf);
	int msgid = 1;
	MQTTString topicString = MQTTString_initializer;
	int req_qos = 0;
	char* payload = "mypayload";
	int payloadlen = strlen(payload);
	int len = 0;
	MQTTTransport mytransport;
	int state = READING;

	if(mysock < 0)
		return mysock;
	/* You will (or already are) 'somehow' connect(ed) to host:port via your hardware specifics. E.g.:
		you have a serial (RS-232/UART) link
		you have a cell modem and you issue your AT+ magic
		you have some TCP/IP which is not lwIP (nor a full-fledged socket compliant one)
		 and you TCP connect
	*/

	mytransport.sck = &mysock;
	mytransport.getfn = transport_getdatanb;
	mytransport.state = 0;
	data.clientID.cstring = "me";
	data.keepAliveInterval = 20;
	data.cleansession = 1;
	data.username.cstring = "testuser";
	data.password.cstring = "testpassword";

	len = MQTTSerialize_connect(buf, buflen, &data);
	/* This one blocks until it finishes sending, you will probably not want this in real life,
	in such a case replace this call by a scheme similar to the one you'll see in the main loop */
	rc = transport_sendPacketBuffer(mysock, buf, len);

	printf("Sent MQTT connect\n");
	/* wait for connack */
	do {
		int frc;
		if ((frc=MQTTPacket_readnb(buf, buflen, &mytransport)) == CONNACK){
			unsigned char sessionPresent, connack_rc;
			if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0){
				printf("Unable to connect, return code %d\n", connack_rc);
				goto exit;
			}
			break;
		}
		else if (frc == -1)
			goto exit;
	} while (1); /* handle timeouts here */

	printf("MQTT connected\n");
	/* subscribe */
	topicString.cstring = "substopic";
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

	/* This is equivalent to the one above, but using the non-blocking functions. You will probably not want this in real life,
	in such a case replace this call by a scheme similar to the one you'll see in the main loop */
	transport_sendPacketBuffernb_start(mysock, buf, len);
	while((rc=transport_sendPacketBuffernb(mysock)) != TRANSPORT_DONE);
	do {
		int frc;
		if ((frc=MQTTPacket_readnb(buf, buflen, &mytransport)) == SUBACK){ /* wait for suback */
			unsigned short submsgid;
			int subcount;
			int granted_qos;

			rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
			if (granted_qos != 0){
				printf("granted qos != 0, %d\n", granted_qos);
				goto exit;
			}
			break;
		}
		else if (frc == -1)
			goto exit;
	} while (1); /* handle timeouts here */
	printf("Subscribed\n");

	/* loop getting msgs on subscribed topic */
	topicString.cstring = "pubtopic";
	state = READING;
	while (!toStop)	{
		/* do other stuff here */
		switch(state){
		case READING:
			if((rc=MQTTPacket_readnb(buf, buflen, &mytransport)) == PUBLISH){
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
				printf("message arrived %.*s\n", payloadlen_in, payload_in);
				printf("publishing reading\n");
				state = PUBLISHING;
				len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
				transport_sendPacketBuffernb_start(mysock, buf, len);
			} else if(rc == -1){
				/* handle I/O errors here */
				goto exit;
			}	/* handle timeouts here */
			break;
		case PUBLISHING:
			switch(transport_sendPacketBuffernb(mysock)){
			case TRANSPORT_DONE:
				printf("Published\n");
				state = READING;
				break;
			case TRANSPORT_ERROR:
				/* handle any I/O errors here */
				goto exit;
				break;
			case TRANSPORT_AGAIN:
			default:
				/* handle timeouts here, not probable unless there is a hardware problem */
				break;
			}
			break;
		}
	}

	printf("disconnecting\n");
	len = MQTTSerialize_disconnect(buf, buflen);
	/* Same blocking related stuff here */
	rc = transport_sendPacketBuffer(mysock, buf, len);

exit:
	transport_close(mysock);
	
	return 0;
}


