#ifndef _GPIO_H_
#define _GPIO_H_
#include "hw_def.h"
void gpioInit(void);
void ssrControl(uint8_t no, uint8_t status);
void directionControl(uint8_t state);
void errorProccess(void);
void errorRelease(void);
#endif
