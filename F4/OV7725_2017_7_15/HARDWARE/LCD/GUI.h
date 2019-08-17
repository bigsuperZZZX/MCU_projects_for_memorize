#ifndef _GUI_h_ 
#define _GUI_h_
#include "stm32f4xx.h" 

void GUI_init(void);
void com_send(uint8_t key);
uint8_t key_refresh(void);

#endif


