#define _HW_MAIN_
#include "global.h"
#undef _HW_MAIN_
#include "hw.h"
extern void enable_irq(void);		/* startup.asm */
extern void disable_irq(void);		/* startup.asm */
extern void set_imask(char);
extern void init_WDT(void);
void hwInit(void)
{
	uint16_t i,j,k;
	
	disable_irq();
//	set_imask(0);
	
	
	regInit();
	buzzerInit();
	init_WDT();
	tm16set();
	FND_Init();
	if(pRead_FlashOption()!=0){
		uint8_t pFndText[4];
		enable_irq();
		delay(1000);
		pFndText[0] = 0x08;
		pFndText[1] = 0x08;
		pFndText[2] = 0x08;
		pFndText[3] = 0x08;
		while(1){
			/* WDT reset */
			clear_WDT();
						
			for(k=0;k<1000;k++){
				
		/*		PORT_FND_SEL1 = 0;
				PORT_FND_SEL2 = 0;
				PORT_FND_SEL3 = 0;
				PORT_FND_SEL4 = 0;*/
				
				j = k %4;
				
	//			PORT_FND_LATCH = 0;
			
		
			      for (i = 0; i < 8; i++)  {
			            
			               //   digitalWrite(dataPin, !!(val & (1 << i)));
			                 
		/*	                PORT_FND_DATA = !!(pFndText[j] & (1 << (7 - i)));
			                  
							PORT_FND_CLKPIN = 1;
							
			            	PORT_FND_CLKPIN = 0;*/
			     	 } 
	//			PORT_FND_LATCH = 1;
				switch(j){
					case 0:
						
//						PORT_FND_SEL1 = 1;
						
					break;
			
					case 1:
//						PORT_FND_SEL2 = 1;
					
					break;
			
					case 2:
						
//						PORT_FND_SEL3 = 1;
						
					break;
			
					case 3:
						
	//					PORT_FND_SEL4 = 1;
						
					break;
				
				}
				delay(2);
			}
		}
	}
	tm16stop();
	pRead_FlashVersion((int8_t *)&Versions[0]);
	
	ledInit();
//	timerInit();
	tm67set();
	uartInit();

	

	
//	set_imask(7);
	enable_irq();			/* enable interrupt (set IE)	*/

/*	for(i=0;i<1500;i++){
		WDCTR |= 0x40;
		pLcd_UpdateDisplay2();
		delay(2);
	}
	*/
	clear_WDT();
	eepromInit();
	
	keyInit();
	cliInit();
	pn_EEP_Load();
	MAX6675_Init();
}
