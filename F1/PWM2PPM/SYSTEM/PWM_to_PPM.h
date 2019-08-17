#ifndef PWM_to_PPM_h
#define PWM_to_PPM_h
#include "stm32f10x.h"

typedef struct int16_rcget
{
				int16_t ROLL;
				int16_t PITCH;
				int16_t THROTTLE;
				int16_t YAW;
				int16_t AUX1;
				int16_t AUX2;
				int16_t AUX3;
}T_RC_Data;

void CAP_init(void);
void CAP_GPIO_init(void);
void CAP_data(void);

#endif




