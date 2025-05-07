//#include "global.h"
#include "minit.h"
void regInit(void)
{
	/* Watch Dog Timer */
	/*==============================================================*/
	/*purpose:	set Watch Dog Timer									*/
	/*==============================================================*/
	
	
	G2ICR  = 0x700f;	/* level 0 clear interrput request flag */
	G3ICR  = 0x700f;	/* level 7 clear interrupt request flag */
	G4ICR  = 0x700f;	/* level 7 clear interrupt request flag */	/* 1ms		   	*/
	G5ICR  = 0x700f;	/* level 4 clear interrupt request flag */	/* 100us	   	*/
	G6ICR  = 0x700f;	/* level 3 clear interrupt request flag */
	G7ICR  = 0x700f;	/* level 7 clear interrupt request flag */
	G8ICR  = 0x700f;	/* level 7 clear interrupt request flag */
	G9ICR  = 0x700f;	/* level 7 clear interrupt request flag */
	G10ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G11ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G12ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G13ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G14ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G15ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G16ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G17ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G18ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G19ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G20ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G21ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G24ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G25ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G26ICR = 0x700f;	/* level 1 clear interrupt request flag */	/* AD0			*/
	G27ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G29ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G31ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G32ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G33ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G34ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G35ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G36ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G41ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G42ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G43ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G45ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G47ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G48ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G49ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G52ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G54ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G55ICR = 0x700f;	/* level 7 clear interrupt request flag */
	G56ICR = 0x700f;	/* level 7 clear interrupt request flag */
	
	/* external interrupt condition */
	/*==============================================================*/
	/*purpose:	set external interrupt condition					*/
	/*		(negative edge, positive edge or level)					*/
	/*==============================================================*/
	EXTMD0 = 0x0000;	/* IRQ7 Falling Edge */
	EXTMD1 = 0x0000;
	
	
	/*===================================================================*/
	/* GPIO																 */
	/*===================================================================*/
	
	P0OUT = 0x00;
	P2OUT = 0x00;
	P3OUT = 0x00;
	P4OUT = 0x00;
	P5OUT = 0x00;
	P6OUT = 0x00;
	P8OUT = 0x00;
	P9OUT = 0x00;
	PCOUT = 0x00;
	PDOUT = 0x00;
	
	
	/*===================================================================*/
	/* GPIO	 Mode														 */
	/*===================================================================*/
	//					15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
	P0MD = 0x00;
	P2MD = 0x0000;	//							  1 1 0 0 1	1
	P3MD = 0x00;
	P4MD = 0x00;
	P5MD = 0x00;
	P6MD = 0x00;
	P8MD = 0x00;
	P9MD = 0x00;
	PCMD = 0x00000000;
	PDMD = 0x00000000;
	
	
	/*===================================================================*/
	/* GPIO	 Direction													 */
	/*===================================================================*/
	P0DIR = 0x00;
	P2DIR = 0x00;
	P3DIR = 0x00;
	P4DIR = 0x00;
	P5DIR = 0x00;
	P6DIR = 0x00;
	P8DIR = 0x3f;
	P9DIR = 0x00;
	PCDIR = 0x00;
	PDDIR = 0x00;
	
	/*===================================================================*/
	/* GPIO	 Pull-up control											 */
	/*===================================================================*/
	P0PLU = 0x00;
	P2PLU = 0x00;
	P3PLU = 0x00;
	P4PLU = 0x40;
	P5PLU = 0x00;
	P6PLU = 0x00;
	P8PLU = 0x00;
	P9PLU = 0x00;
	PCPLU = 0x00;
	PDPLU = 0x00;
	
	
	/*===================================================================*/
	/* Noise Filter														 */
	/*===================================================================*/
	NF0CNT = 0x0000;
	NF1CNT = 0x0000;
	NF2CNT = 0x0000;
	NF3CNT = 0x0000;
	NF4CNT = 0x0000;
	NF5CNT = 0x0000;
	NF6CNT = 0x0000;
	NF7CNT = 0x0000;
	NF8CNT = 0x0000;
	NF9CNT = 0x0000;
	NF10CNT = 0x0000;
	NF11CNT = 0x130B;
	NF12CNT = 0x0000;
	NF13CNT = 0x0000;
	NF14CNT = 0x0000;
	NF15CNT = 0x0000;
	
	/*===================================================================*/
	/* TIMER															 */
	/*===================================================================*/
	TM0003PSC = 0x80;
	TM0407PSC = 0x80;
	TM00MD = 0x00;
	TM01MD = 0x00;
	TM02MD = 0x00;
	TM03MD = 0x00;
	TM04MD = 0x00;
	TM05MD = 0x00;
	TM06MD = 0x00;
	TM07MD = 0x00;
	TM08MD = 0x00;
	TM09MD = 0x00;
	TM10MD = 0x00;
	TM11MD = 0x00;
	
	TM16MD1 = 0x0000;
	TM17MD1 = 0x0000;
	TM18MD1 = 0x0000;
	TM19MD1 = 0x0000;
	
	SIFCLK0 = 0x00;
	
}

void tm67set(void)
{

	TM0407EXPSC = 0x80;
	TM06MD_W = 0x0301;	/* IOCLK/8ÅATM2,3 cascade 		*/
	TM06BR_W = (3750 - 1);	/* 1ms * (30MHz/8) = 3750 		*/
	TM06MD_W |= 0x4040;	/* load base register			*/
	TM06MD_W &= ~0x4040;	/* end of load				*/
	TM06MD_W |= 0x8080;	/* enable timer count 			*/
	G48ICR = 0x3800;	/* enable interrupt (1ms)		*/
}

enum{
	LOWER_RISING,
	LOWER_FALLING,
	UPPER_RISING,
	UPPER_FALLING
};
void tm16set(void)
{
	TM16MD1 &= ~(short)0x00c0;
	G31ICR &= ~(short)0x0f00;
	TM16MD1 = (TM16MD1 & ~(short)0x000f) | 0x0002;
	TM16CA = 0x752f;

	TM16MD1 |= 0x2000;
	TM16MD1 |= 0x0040;
	TM16MD1 &= ~0x0040;
	G31ICR = 0x3200; // 1ms - 2
	TM16MD1 |= 0x80;
	

}

void tm16stop(void)
{
	TM16MD1 &= ~0x0080;
}

