#ifndef _UART_H_
#define _UART_H_

#define UART_MAX_CH         HW_UART_MAX_CH

typedef struct{
	volatile uint8_t rfg;
	volatile uint8_t tb[50],rb[50];
	volatile uint8_t rc,tc,tcMax;
	uint8_t dc, hi, low;
	uint8_t rxTmr;
	uint8_t deviceError;
	uint8_t commError;
	uint32_t rxTime;
	uint32_t txTime;
}USART_User_InitTypeDef;

void uartInit(void);
uint32_t uartAvailable(uint8_t ch);
uint8_t uartRead(uint8_t ch);
uint32_t uartWrite(uint8_t ch, uint8_t Data);
uint32_t uartPrintf(uint8_t ch, char *fmt, ...);
void uartWriteStatus(uint16_t val, uint16_t Data, uint16_t Data2);
void U_Printf_Num(uint8_t ch, uint32_t Data);
void transInformation(uint16_t *data);
#endif
