#ifndef _I2C_H_
#define _I2C_H_

#include "hw_def.h"

#define I2C_MAX_CH       HW_I2C_MAX_CH


#define DEVICE_ADDR 0xa0

void i2cInit(void);
void i2cReset(void);
uint8_t pIIC_SendByte(uint8_t Byte);
uint8_t pIIC_SendByte_NACK(uint8_t Byte);
uint8_t i2cRecovery(void);
uint8_t i2cReadByte (uint16_t dev_addr, uint16_t reg_addr, uint8_t *data, uint32_t timeout);
uint8_t i2cReadBytes(uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);

uint8_t i2cWriteByte (uint16_t dev_addr, uint16_t reg_addr, uint8_t data, uint32_t timeout);
uint8_t i2cWriteBytes(uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);


uint8_t i2cReadData(uint16_t dev_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);
uint8_t i2cWriteData(uint16_t dev_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);


void     i2cSetTimeout(uint32_t timeout);
uint32_t i2cGetTimeout(void);

void     i2cClearErrCount(void);
uint32_t i2cGetErrCount(void);
uint8_t pCheckACK(void);
#endif
