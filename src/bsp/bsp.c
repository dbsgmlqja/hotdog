#include "bsp.h"
static volatile uint32_t systick_ms = 0;
extern void swtimerISR(void);
void SysTick_Handler(void)
{
  	systick_ms++;
	swtimerISR();
	p1msTimer();
}
void SysTick_Handler2(void)
{
  	systick_ms++;
}
void bspInit(void)
{
	
}

void delay(uint32_t ms)
{
	uint32_t time = millis();
	while(millis() - time < ms) clear_WDT();
}
uint32_t millis(void)
{
	return systick_ms;
}

void Error_Handler(void)
{
	
}
void init_WDT(void)
{
#if 1
	PRTWDT = 0xd9;
	WDCTR &= ~0x70;
	WDCTR = 0x63;		/* initialize WDT, clock fosc/2^14 	*/
	WDCTR |= 0x80;		/* start timer count 			*/
	PRTWDT = 0xff;
#else
	PRTWDT = 0xd9;		/* enable writing to WDCTR register */
	WDCTR &= ~0x80;		/* WDT Stop */
	PRTWDT = 0xff;		/* disable writing to target registers above */
#endif
}
/**********************************************************************
Function:  int_WDT()

Description: WDT overflow
		micom reset

**********************************************************************/
void int_WDT(void)
{
//	gWatchdogSave();
	
	while(1){
		CHIPRSTCTR = 0x01;		/* micom software reset */
	}
}
void clear_WDT(void)
{
	PRTWDT = 0xd9;		/* enable writing to WDCTR register */
	WDCTR |= 0x40;		/* WDT Stop */
	PRTWDT = 0xff;		/* disable writing to target registers above */
}
