#ifndef WHEEL_H_
#define WHEEL_H_

#include <stdbool.h>

#define ENCODER_GPIO       14
#define PULSOS_POR_VOLTA   20

void wheel_Init(void);

float wheel_GetRPM(void);

#endif