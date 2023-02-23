#ifndef _ADC_H_
#define _ADC_H_

#include "hw_def.h"


void adcInit(void);
void adcRead(void);
uint16_t adcReadCH(uint8_t ch);
uint8_t adcCheck(void);
#endif
