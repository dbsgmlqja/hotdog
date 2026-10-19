#ifndef _SOFT_I2C_H_
#define _SOFT_I2C_H_

#include "hw_def.h"

void si2cInit(void);
void si2c_Start(void);
void si2cWrite(uint8_t Data);
uint8_t si2cAck(void);
void si2cStop(void);
uint8_t SI2C_ReadByte(uint8_t addr,uint8_t *data, uint8_t length);
void si2cWriteByte(uint8_t addr, uint8_t *Data, uint8_t length);
void SI2C_Write_Verify(uint8_t addr, uint8_t Data);

#endif
