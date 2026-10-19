#ifndef _SPI_H_
#define _SPI_H_
#include "hw_def.h"
void spiInit(void);
void updateSpi(void);
float getSpiData(uint8_t ch);
#endif
