#ifndef _usart_H
#define _usart_H

#include "stm32f10x.h"
#include "stdio.h"

void usart_init(u32);


//extern u8 USART_RX_BUF[64];     //接收缓冲,最大63个字节.末字节为换行符 
//e/xtern u8 USART_RX_STA;         //接收状态标记	
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义



#endif
