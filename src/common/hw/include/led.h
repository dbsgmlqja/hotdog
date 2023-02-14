#ifndef _LED_H_
#define _LED_H_

#include "hw_def.h"


#ifdef _USE_HW_LED


#define LED_MAX_CH          HW_LED_MAX_CH

typedef struct{
	uint16_t Comp1;
	uint16_t Comp2;
	uint16_t OpTime;
	uint16_t RunTime;
	uint16_t Temp1;
	uint16_t Temp2;
	uint32_t pre_time;
	uint16_t Mode;
	uint8_t OldStart;
	uint8_t Start;	
}_StatusLed;

uint8_t ledInit(void);
void ledOn(uint8_t ch);
void ledOff(uint8_t ch);
void ledToggle(uint8_t ch);
void ledStatus(void);
#endif


#endif
