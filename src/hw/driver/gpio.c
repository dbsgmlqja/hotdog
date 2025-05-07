#include "global.h"
#include "gpio.h"

void gpioInit(void)
{
	P2DIR |= 0x04;
	P9DIR |= 0x02;
	P6DIR |= 0x11;
	P6OUT &= ~0x10;
	P2OUT &= ~0x04;
	P9OUT &= ~0x02;
}
void errorProccess(void)
{
	ssrControl(1, 0);
	P24RELAY = 0;
}
void errorRelease(void)
{
	ssrControl(1, 1);
	P24RELAY = 1;
}
void directionControl(uint8_t state)
{
	MDir = state;
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
	}
}
