#ifndef MAIN_WHEEL_H_
#define MAIN_WHEEL_H_

#include "driver/pulse_cnt.h"

// Pino do encoder (D0)
#define ENCODER_GPIO  14

void wheel_Init(void);
void wheel_GetEncoderPulses(int *pulsos);

#endif