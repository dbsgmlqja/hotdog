#include "global.h"
#include "gpio.h"

void gpioInit(void)
{
	P8DIR |= 0x0B;
	P8OUT &= ~0x0A;
}
void ssrControl(uint8_t no, uint8_t state)
{
	switch(no)
	{
		case 1:
			
			break;
		case 2:
			
			break;
		case 3:
			SSR3_OUT = state;
			break;
		case 4:
			SSR4_OUT = state;
			break;
	}
}
