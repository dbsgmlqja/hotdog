
#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_


#include "def.h"
#include "srMN103HFD5.h"

void bspInit(void);

void delay(uint32_t ms);
uint32_t millis(void);
void clear_WDT(void);
void Error_Handler(void);
void int_WDT(void);

#endif /* SRC_BSP_BSP_H_ */
