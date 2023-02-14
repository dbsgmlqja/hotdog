#include "timer.h"

void timerInit(void)
{
	TM16MD1 &= ~0xc000;
//	PSW &= ~0x0800;
	G31ICR &= ~0x0400;
	TM16CA = 0x752f;
	TM16MD1 = 0x0002;
	TM16MD1 |= 0x2000;
	TM16MDA &= ~0x0080;
	TM16MD1 |= 0x0040;
	TM16MD1 &= ~0x0040;
	G4ICR |= 0x0200;
	TM16MD1 |= 0x0080;
}
