#ifndef _UART_H_
#define _UART_H_

#define UART_MAX_CH         HW_UART_MAX_CH


void uartInit(void);
uint32_t uartAvailable(uint8_t ch);
uint8_t uartRead(uint8_t ch);
uint32_t uartWrite(uint8_t ch, uint8_t Data);
uint32_t uartPrintf(uint8_t ch, char *fmt, ...);
void uartWriteStatus(uint16_t val, uint16_t Data, uint16_t Data2);
void U_Printf_Num(uint8_t ch, uint32_t Data);
void transInformation(uint16_t *data);
#endif
