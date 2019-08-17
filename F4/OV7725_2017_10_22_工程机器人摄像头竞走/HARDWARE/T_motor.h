#ifndef _T_MOTOR_H_
#define _T_MOTOR_H_
#include "sys.h"

#define ACTION_BYTES 150


void TIM3_Int_Init(u16 arr,u16 psc);
void action_delay_read(uint16_t* action_delay, const char (*action_list)[ACTION_BYTES]);
void action_set(uint16_t* action_delay, const char (*action_list)[ACTION_BYTES]);










#endif

