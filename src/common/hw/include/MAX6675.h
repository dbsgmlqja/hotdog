#ifndef _MAX6675_H_
#define _MAX6675_H_

#include "hw_def.h"

void MAX6675_Init(void);
void readTemperature(void);
int pidControlTop(void);
int pidControlBottom(void);
void gSetStatus(void);
#endif
