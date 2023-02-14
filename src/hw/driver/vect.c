#include "hw_def.h"
#include "global.h"
extern void enable_irq(void);
extern void disable_irq(void);
extern void set_imask(char);
extern void SysTick_Handler(void);
extern void SysTick_Handler2(void);
extern void int_WDT(void);
extern void UART1_IRQHandler(void);
void irq_group00(void);  /* group 0: */
void irq_group01(void);  /* group 1: */
void irq_group02(void);  /* group 2: */
void irq_group03(void);  /* group 3: */
void irq_group04(void);  /* group 4: */
void irq_group05(void);  /* group 5: */
void irq_group06(void);  /* group 6: */
void irq_group07(void);  /* group 7: */
void irq_group08(void);  /* group 8: */
void irq_group09(void);  /* group 9: */
void irq_group10(void);  /* group 10: */
void irq_group11(void);  /* group 11: */
void irq_group12(void);  /* group 12: */
void irq_group13(void);  /* group 13: */
void irq_group14(void);  /* group 14: */
void irq_group15(void);  /* group 15: */
void irq_group16(void);  /* group 16: */
void irq_group17(void);  /* group 17: */
void irq_group18(void);  /* group 18: */
void irq_group19(void);  /* group 19: */
void irq_group20(void);  /* group 20: */
void irq_group21(void);  /* group 21: */
void irq_group22(void);  /* group 22: */
void irq_group23(void);  /* group 23: */
void irq_group24(void);  /* group 24: */
void irq_group25(void);  /* group 25: */
void irq_group26(void);  /* group 26: */
void irq_group27(void);  /* group 27: */
void irq_group28(void);  /* group 28: */
void irq_group29(void);  /* group 29: */
void irq_group30(void);  /* group 30: */
void irq_group31(void);  /* group 31: */
void irq_group32(void);  /* group 32: */
void irq_group33(void);  /* group 33: */
void irq_group34(void);  /* group 34: */
void irq_group35(void);  /* group 35: */
void irq_group36(void);  /* group 36: */
void irq_group37(void);  /* group 37: */
void irq_group38(void);  /* group 38: */
void irq_group39(void);  /* group 39: */
void irq_group40(void);  /* group 40: */
void irq_group41(void);  /* group 41: */
void irq_group42(void);  /* group 42: */
void irq_group43(void);  /* group 43: */
void irq_group44(void);  /* group 44: */
void irq_group45(void);  /* group 45: */
void irq_group46(void);  /* group 46: */
void irq_group47(void);  /* group 47: */
void irq_group48(void);  /* group 48: */
void irq_group49(void);  /* group 49: */
void irq_group50(void);  /* group 50: */
void irq_group51(void);  /* group 51: */
void irq_group52(void);  /* group 52: */
void irq_group53(void);  /* group 53: */
void irq_group54(void);  /* group 54: */
void irq_group55(void);  /* group 55: */
void irq_group56(void);  /* group 56: */

/* declare functions for interrupt vector */
void (* const irq_vct_tbl[])() = {
	irq_group00,  /* group 0: */
	irq_group01,  /* group 1: */
	irq_group02,  /* group 2: */
	irq_group03,  /* group 3: */
	irq_group04,  /* group 4: */
	irq_group05,  /* group 5: */
	irq_group06,  /* group 6: */
	irq_group07,  /* group 7: */
	irq_group08,  /* group 8: */
	irq_group09,  /* group 9: */
	irq_group10,  /* group 10: */
	irq_group11,  /* group 11: */
	irq_group12,  /* group 12: */
	irq_group13,  /* group 13: */
	irq_group14,  /* group 14: */
	irq_group15,  /* group 15: */
	irq_group16,  /* group 16: */
	irq_group17,  /* group 17: */
	irq_group18,  /* group 18: */
	irq_group19,  /* group 19: */
	irq_group20,  /* group 20: */
	irq_group21,  /* group 21: */
	irq_group22,  /* group 22: */
	irq_group23,  /* group 23: */
	irq_group24,  /* group 24: */
	irq_group25,  /* group 25: */
	irq_group26,  /* group 26: */
	irq_group27,  /* group 27: */
	irq_group28,  /* group 28: */
	irq_group29,  /* group 29: */
	irq_group30,  /* group 30: */
	irq_group31,  /* group 31: */
	irq_group32,  /* group 32: */
	irq_group33,  /* group 33: */
	irq_group34,  /* group 34: */
	irq_group35,  /* group 35: */
	irq_group36,  /* group 36: */
	irq_group37,  /* group 37: */
	irq_group38,  /* group 38: */
	irq_group39,  /* group 39: */
	irq_group40,  /* group 40: */
	irq_group41,  /* group 41: */
	irq_group42,  /* group 42: */
	irq_group43,  /* group 43: */
	irq_group44,  /* group 44: */
	irq_group45,  /* group 45: */
	irq_group46,  /* group 46: */
	irq_group47,  /* group 47: */
	irq_group48,  /* group 48: */
	irq_group49,  /* group 49: */
	irq_group50,  /* group 50: */
	irq_group51,  /* group 51: */
	irq_group52,  /* group 52: */
	irq_group53,  /* group 53: */
	irq_group54,  /* group 54: */
	irq_group55,  /* group 55: */
	irq_group56,  /* group 56: */
};
void irq_group00(void)
{
	int_WDT();
}
void irq_group01(void)	
{
	int_WDT();
}
void irq_group02(void)	//Power Detect
{
	if(G2ICR & 0x0001){
		G2ICR_C = 0x01;					// clear interrupt request flag
		G2ICR &= ~0x0100;				// ----|---0|----|----	Š„ž‚Ý‹Ö?
	}
	else{
		G2ICR &= ~0x0C00;				// ----|00--|----|----	Š„ž‚Ý‹Ö?
		G2ICR_C = 0x0C;					// clear interrupt request flag
	}
}
void irq_group03(void)
{
	if(G3ICR & 0x0001){
		G3ICR_C = 0x01;					// clear interrupt request flag
		G3ICR &= ~0x0100;				// ----|---0|----|----	Š„ž‚Ý‹Ö?
	}
	else if(G3ICR & 0x0002){
		G3ICR_C = 0x02;
		G3ICR &= ~0x0200;
	}
	else{
		G3ICR &= ~0x0C00;				// ----|00--|----|----	Š„ž‚Ý‹Ö?
		G3ICR_C = 0x0C;					// clear interrupt request flag
	}
}
void irq_group04(void)	//UL1
{
	if(G4ICR & 0x0001){
		
		enable_irq();
		G4ICR_C = 0x01;					// clear interrupt request flag
		
	}
}
void irq_group05(void)
{
	//Not Use
}
void irq_group06(void)	//Lin
{
	if(G6ICR & 0x0001){
		G6ICR_C = 0x01;
		G6ICR &= ~0x0100;
	}
}
void irq_group07(void)	//IRQ1 UL2
{
	if(G7ICR & 0x0001){
		enable_irq();
		G7ICR_C = 0x01;
		
	}
}
void irq_group08(void)	//IRQ2 LL1
{
	if(G8ICR & 0x0001){
		enable_irq();
		G8ICR_C = 0x01;
		
	}
}
void irq_group09(void)	//IRQ3 LL2
{
	if(G9ICR & 0x0001){
		enable_irq();
		G9ICR_C = 0x01;
	}
}
void irq_group10(void)	//IRQ8
{
	if(G10ICR & 0x0001){
		enable_irq();
		
		G10ICR_C = 0x01;
	}
}
void irq_group11(void)	//IRQ 9
{
	if(G11ICR & 0x0001){
		G11ICR_C = 0x01;
		G11ICR_C &= ~0x0100;
	}
}
void irq_group12(void)	//IRQ10
{
	if(G12ICR & 0x0001){
		G12ICR_C = 0x01;
		
	}
}
void irq_group13(void)	//IRQ11 PowerFail
{
	if(G13ICR & 0x0001){
		
	
		G13ICR_C = 0x01;
	}
}
void irq_group14(void)	//PWM0A
{
	if(G14ICR & 0x0001){
		G14ICR_C = 0x01;
		G14ICR_C &= ~0x0100;
	}
	else if(G14ICR & 0x0002){
		G14ICR_C = 0x02;
		G14ICR &= ~0x0200;
	}
	else if(G14ICR & 0x0004){
		G14ICR_C = 0x04;
		G14ICR &= ~0x0400;
	}
}
void irq_group15(void)	//PWM1,2
{
	if(G15ICR & 0x0001){
		G15ICR_C = 0x01;
		G15ICR_C &= ~0x0100;
	}
	else if(G15ICR & 0x0002){
		G15ICR_C = 0x02;
		G15ICR &= ~0x0200;
	}
}
void irq_group16(void)	//PWM0
{
	if(G16ICR & 0x0001){
		G16ICR_C = 0x01;
		G16ICR_C &= ~0x0100;
	}
	else if(G14ICR & 0x0002){
		G16ICR_C = 0x02;
		G16ICR &= ~0x0200;
	}
}
void irq_group17(void)	//PWM0B
{
	if(G17ICR & 0x0001){
		G17ICR_C = 0x01;
		G17ICR_C &= ~0x0100;
	}
	else if(G17ICR & 0x0002){
		G17ICR_C = 0x02;
		G17ICR &= ~0x0200;
	}
}
void irq_group18(void)	//PWM0C
{
	if(G18ICR & 0x0001){
		G18ICR_C = 0x01;
		G18ICR_C &= ~0x0100;
	}
	else if(G18ICR & 0x0002){
		G18ICR_C = 0x02;
		G18ICR &= ~0x0200;
	}
}
void irq_group19(void)	//PWM0 -> AD
{
	if(G19ICR & 0x0001){
		G19ICR_C = 0x01;
		G19ICR_C &= ~0x0100;
	}
	else if(G19ICR & 0x0002){
		G19ICR_C = 0x02;
		G19ICR &= ~0x0200;
	}
}
void irq_group20(void)
{
	if(G20ICR & 0x0001){	//PWM1 Over-flow
		G20ICR_C = 0x01;
		G20ICR_C &= ~0x0100;
	}
	else if(G20ICR & 0x0002){	//PWM1 Under-flow
		G20ICR_C = 0x02;
		G20ICR &= ~0x0200;
	}
}
void irq_group21(void)	//PWM1 -> A/D
{
	if(G20ICR & 0x0001){	
		G21ICR_C = 0x01;
		G21ICR_C &= ~0x0100;
	}
	else if(G21ICR & 0x0002){	
		G21ICR_C = 0x02;
		G21ICR &= ~0x0200;
	}
}
void irq_group22(void)
{

}
void irq_group23(void)
{
	
}
void irq_group24(void)
{
	if(G24ICR & 0x0001){
		G24ICR_C = 0x01;
		G24ICR &= 0x0100;
	}
	else if(G24ICR & 0x0002){
		G24ICR_C = 0x02;
		G24ICR &= 0x0200;
	}
}
void irq_group25(void)
{
	if(G25ICR & 0x0001){
		enable_irq();
		G25ICR_C = 0x01;		
	}
	else if(G25ICR & 0x0002){
		G25ICR_C = 0x02;
		G25ICR &= 0x0200;
	}
}
void irq_group26(void)
{
	if(G26ICR & 0x0001){
		G26ICR_C = 0x01;
		G26ICR &= 0x0100;
	}
	else if(G26ICR & 0x0002){
		G26ICR_C = 0x02;
		G26ICR &= 0x0200;
	}

}
void irq_group27(void)
{
	if(G27ICR & 0x0001){
		enable_irq();
		G27ICR_C = 0x01;
		
	}
	else if(G27ICR & 0x0002){
		enable_irq();
		G27ICR_C = 0x02;
		
	}
}
void irq_group28(void)
{
	if(G28ICR & 0x0001){
		G28ICR_C = 0x01;
		G28ICR &= 0x0100;
	}
}
void irq_group29(void)
{
	if(G29ICR & 0x0001){
		G29ICR_C = 0x01;
		G29ICR &= 0x0100;
	}
	else if(G29ICR & 0x0002){
		G29ICR_C = 0x02;
		G29ICR &= 0x0200;
	}
	else if(G29ICR & 0x0004){
		G29ICR_C = 0x04;
		G29ICR &= 0x0400;
	}
		else if(G29ICR & 0x0008){
		G29ICR_C = 0x08;
		G29ICR &= 0x0800;
	}
}
void irq_group30(void)
{
	
}
void irq_group31(void)
{
	if(G31ICR & 0x0001){
		G31ICR_C = 0x01;
		enable_irq();
	}
	else if(G31ICR & 0x0002){
		G31ICR_C = 0x02;
		enable_irq();
		SysTick_Handler2();
	}
	else if(G31ICR & 0x0004){
		G31ICR_C = 0x04;
		G31ICR &= 0x0400;
	}
	else if(G31ICR & 0x0008){
		G31ICR_C = 0x08;
		G31ICR &= 0x0800;
	}
}
void irq_group32(void)
{
	if(G32ICR & 0x0001){
		G32ICR_C = 0x01;
		G32ICR &= 0x0100;
	}
	else if(G32ICR & 0x0002){
		G32ICR_C = 0x02;
		G32ICR &= 0x0200;
	}
	else if(G32ICR & 0x0004){
		G32ICR_C = 0x04;
		G32ICR &= 0x0400;
	}
	else if(G32ICR & 0x0008){
		G32ICR_C = 0x08;
		G32ICR &= 0x0800;
	}
}
void irq_group33(void)
{
	if(G33ICR & 0x0001){
		G33ICR_C = 0x01;
		G33ICR &= 0x0100;
	}
	else if(G33ICR & 0x0002){
		G33ICR_C = 0x02;
		G33ICR &= 0x0200;
	}
	else if(G33ICR & 0x0004){
		G33ICR_C = 0x04;
	G33ICR &= 0x0400;
	}
	else if(G33ICR & 0x0008){
		G33ICR_C = 0x08;
		G33ICR &= 0x0800;
	}
}
void irq_group34(void)
{
	if(G34ICR & 0x0001){
		G34ICR_C = 0x01;
		G34ICR &= 0x0100;
	}
	else if(G34ICR & 0x0002){
		G34ICR_C = 0x02;
		G34ICR &= 0x0200;
	}
	else if(G34ICR & 0x0004){
		G34ICR_C = 0x04;
		G34ICR &= 0x0400;
	}
	else if(G34ICR & 0x0008){
		G34ICR_C = 0x08;
		G34ICR &= 0x0800;
	}
}
void irq_group35(void)
{
	if(G35ICR & 0x0001){
		G35ICR_C = 0x01;
		G35ICR &= 0x0100;
	}
	else if(G35ICR & 0x0002){
		G35ICR_C = 0x02;
		G35ICR &= 0x0200;
	}
	else if(G35ICR & 0x0004){
		G35ICR_C = 0x04;
		G35ICR &= 0x0400;
	}else if(G35ICR & 0x0008){
		G35ICR_C = 0x08;
		G35ICR &= 0x0800;
	}
}
void irq_group36(void)	//Timer21
{
	if(G36ICR & 0x0001){
		G36ICR_C = 0x01;
		G36ICR &= 0x0100;
	}
	else if(G36ICR & 0x0002){
		G36ICR_C = 0x02;
		G36ICR &= 0x0200;
	}
	else if(G36ICR & 0x0004){
		G36ICR_C = 0x04;
		G36ICR &= 0x0400;
	}
	else if(G36ICR & 0x0008){
		G36ICR_C = 0x08;
		G36ICR &= 0x0800;
	}
}
void irq_group37(void)	//Timer 22
{
	
}
void irq_group38(void)	//Timer 23
{
	
}
void irq_group39(void)	//Timer 24
{
	
}
void irq_group40(void)	//Timer 25
{
	
}
void irq_group41(void)	//serial 0
{
	if(G41ICR & 0x0001){
		G41ICR_C = 0x01;
		G41ICR &= 0x0100;
	}
	else if(G41ICR & 0x0002){
		G41ICR_C = 0x02;
		G41ICR &= 0x0200;
	}
}
void irq_group42(void)	//IIC
{
	if(G42ICR & 0x0002){
		G42ICR_C = 0x02;
		enable_irq();
		IIC_TX_Complete = 1;
	}
	else if(G42ICR & 0x0001){
		G42ICR_C = 0x01;
	}
}
void irq_group43(void)	//Serial 1
{
	if(G43ICR & 0x0001){
		G43ICR_C = 0x01;
		enable_irq();
		UART1_IRQHandler();
	}
	else if(G43ICR & 0x0002){
		G43ICR_C = 0x02;
		G43ICR &= 0x0200;
	}
}
void irq_group44(void)	//Serial 2
{
	if(G44ICR & 0x0001){
		G44ICR_C = 0x01;
		G44ICR &= 0x0100;
	}
	else if(G44ICR & 0x0002){
		G44ICR_C = 0x02;
		G44ICR &= 0x0200;
	}
}
void irq_group45(void)	//Serial 3
{
	if(G45ICR & 0x0001){
		G45ICR_C = 0x01;
		G45ICR &= 0x0100;
	}
	else if(G45ICR & 0x0002){
		G45ICR_C = 0x02;
		G45ICR &= 0x0200;
	}
}
void irq_group46(void)	//Serial 4
{
	if(G46ICR & 0x0001){
		G46ICR_C = 0x01;
		G46ICR &= 0x0100;
	}
	else if(G46ICR & 0x0002){
		G46ICR_C = 0x02;
		G46ICR &= 0x0200;
	}
}
void irq_group47(void)	//Timer 0 ~ 3
{
	if(G47ICR & 0x0001){
		
		G47ICR_C = 0x01;
		G47ICR &= 0x0100;
	}
	else if(G47ICR & 0x0002){
		enable_irq();
		G47ICR_C = 0x02;
		G47ICR &= 0x0200;
	}
	else if(G47ICR & 0x0004){
		G47ICR_C = 0x04;
		G47ICR &= 0x0400;
	}
	else if(G47ICR & 0x0008){
			
		G47ICR_C = 0x08;
		G47ICR &= 0x0800;
	}
}
void irq_group48(void)	//Timer 4~7
{
	if(G48ICR & 0x0008){
		enable_irq();
		SysTick_Handler();
		
		G48ICR_C = 0x08;
	}
}
void irq_group49(void)	//Timer 8 ~ 11
{
	if(G49ICR & 0x0001){
		G49ICR_C = 0x01;
		G49ICR &= ~0x0100;
	}
	else if(G49ICR & 0x0002){
		G49ICR_C = 0x02;
		G49ICR &= ~0x0200;
	}
	else if(G49ICR & 0x0004){
		G49ICR_C = 0x04;
		G49ICR &= ~0x0400;
	}
}
void irq_group50(void)	//Timer 12 ~ 15~
{
	
}
void irq_group51(void)	//Timer 26 ~ 29
{
	if(G51ICR & 0x0001){
		G51ICR_C = 0x01;
		G51ICR &= 0x0100;
	}
	else if(G51ICR & 0x0002){
		G51ICR_C = 0x02;
		G51ICR &= 0x0200;
	}
	else if(G51ICR & 0x0004){
		G51ICR_C = 0x04;
		G51ICR &= 0x0400;
	}
	else if(G51ICR & 0x0008){
		G51ICR_C = 0x08;
		G51ICR &= 0x0800;
	}
}
void irq_group52(void)	//IRQ 4 ~ 5
{
	if(G52ICR & 0x0001){
		G52ICR_C = 0x01;
		G52ICR &= 0x0100;
	}
	else if(G52ICR & 0x0002){
		G52ICR_C = 0x02;
		G52ICR &= 0x0200;
	}
}
void irq_group53(void)
{
	
}
void irq_group54(void)
{
	
}void irq_group55(void)
{
	
}
void irq_group56(void)
{
	
}
