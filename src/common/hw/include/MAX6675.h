#ifndef _MAX6675_H_
#define _MAX6675_H_

#include "hw_def.h"
typedef struct{
	uint8_t index;
	uint16_t Arr[5];
	uint16_t Rec[5];
	uint16_t prev_temp;
}_TempArr;
void MAX6675_Init(void);
void readTemperature(void);
int pidControl(void);
void gSetStatus(void);
void pSetTemperature(void);
#endif
