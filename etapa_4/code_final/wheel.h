#ifndef MAIN_WHEEL_H_
#define MAIN_WHEEL_H_

#include "driver/pulse_cnt.h"  //periférico PCNT do ESP32

// GPIO do encoder
#define ENCODER_GPIO 14

void wheel_Init(void);
void wheel_UpdateRPM(void);
float wheel_GetRPM(void);

#endif