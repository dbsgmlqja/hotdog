#ifndef _SSR_H_
#define _SSR_H_
#include "hw_def.h"

void ssrInit(void);
void ssrOn(void);
void ssrOff(void);
uint8_t ssrUpdate(void);
void pidPrameterReset(void);
void getSSRData(uint16_t *data);
#endif
