#ifndef _encoder_h
#define _encoder_h

void encoder_ExtiInit(void);
signed long read_encoder_pulse(void);   //读取脉冲数
char read_encoder_dirction(void);       //读取方向

#endif

