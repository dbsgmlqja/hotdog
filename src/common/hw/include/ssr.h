#ifndef _SSR_H_
#define _SSR_H_
#include "hw_def.h"

typedef struct{
	uint8_t error;
	uint8_t output;
	uint16_t voltage;
	uint16_t outVoltage;
	uint16_t temperature;
}_SSR;
void ssrInit(void);
void ssrOn(void);
void ssrOff(void);
uint8_t ssrUpdate(void);
void gSetSetpoint(uint16_t value);
void pidPrameterReset(void);
void getSSRData(uint16_t *data);
void gResetSSR(void);
uint8_t getSSRError(void);
uint8_t getSSROutput(void);
uint16_t getSSRVoltage(void);
uint16_t getSSRTemperature(void);
void setSSROutput(uint8_t data);
void setSSRVoltage(uint16_t data);
void setSSROutVoltage(uint16_t data);
void setSSRTemperature(uint16_t data);
void setSSRError(uint8_t data);
#endif
