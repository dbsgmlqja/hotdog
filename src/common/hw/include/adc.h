#ifndef _ADC_H_
#define _ADC_H_

#include "hw_def.h"

typedef struct{
	uint16_t Temp1;
	uint16_t Temp2;
}_AD;
void adcInit(void);
void adcRead(_AD *p_adc);
uint16_t adcReadCH(uint8_t ch);
uint8_t adcCheck(_AD *p_obj);
#endif
