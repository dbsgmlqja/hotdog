#include "global.h"
#include "gpio.h"

void gpioInit(void)
{
	P2DIR |= 0x04;
	P4DIR |= 0x80;
	P5DIR |= 0x03;
	P8DIR |= 0x04;
	P2OUT &= ~0x04;
	P4OUT &= ~0x80;
	P5OUT &= ~0x03;
	P8OUT &= ~0x04;
}
void ssrControl(uint8_t no, uint8_t state)
{
	switch(no)
	{
		case 1:
			SSR1_OUT = state;
			break;
		case 2:
			SSR2_OUT = state;
			break;
		case 3:
			SSR3_OUT = state;
			break;
		case 4:
			SSR4_OUT = state;
			break;
		case 5:
			SSR5_OUT = state;
			break;
	}
}
