#ifndef _moto_h
#define _moto_h

#define offset 1.075   //左轮要乘上这个才能保证直行 

void moto_init(void);
void moto_control(void);
void moto_ControlInit(void);
void turn_left(void);
void turn_right(void);
void gostreight(void);
void stuck(unsigned char stuck);

#endif

