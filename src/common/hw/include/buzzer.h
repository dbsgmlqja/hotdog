#ifndef _BUZZER_H_
#define _BUZZER_H_

#include "hw_def.h"

void buzzerInit(void);
void pBeep(uint8_t Cnt,uint8_t sl);

void buzzer_start(void);
void buzzer_stop(void);
#endif
