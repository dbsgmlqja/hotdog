#ifndef _MODBUS_H_
#define _MODBUS_H_
#define MAX_485_CH (3)

void modbusInit(void);
void modbusTransmit(void);
void gSetCommand(uint16_t addr, uint16_t value, uint16_t Flag);
void gSetMonitor(uint16_t addr, uint16_t Flag);
uint16_t gGetMonitor(uint16_t addr);
void modbusCheck(void);
void gReleaseError(uint8_t mode);
uint8_t modbusConnectCheck(void);
#endif
