#ifndef SRC_HW_HW_H_
#define SRC_HW_HW_H_

#define	RELAY_INRUSH	((*(RB_BTF*)&P5OUT).p0)
#include "hw_def.h"

#include "minit.h"
#include "led.h"
#include "timer.h"
#include "uart.h"
#include "display.h"
#include "adc.h"
#include "key.h"
#include "FlashOption.h"
#include "cli.h"
#include "buzzer.h"
#include "FND.h"
#include "i2c.h"
#include "eeprom.h"
#include "key.h"
#include "swtimer.h"
void hwInit(void);


#endif /* SRC_HW_HW_H_ */
