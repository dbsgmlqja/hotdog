#ifndef _ADC_H_
#define _ADC_H_

#include "hw_def.h"


void adcInit(void);
void adcRead(void);
void adc0Read(void);
uint16_t adcReadCH(uint8_t ch);
#endif
