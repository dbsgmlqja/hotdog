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
	disable_irq();
	
	regInit();
	buzzerInit();
	init_WDT();
	tm16set();
	if(pRead_FlashOption()!=0){
		enable_irq();
		delay(1000);
		/* LCD INIT */
		pLcd_Init();
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
		pLcd_UpdateDisplay();
		delay(100);
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
		pLcd_UpdateDisplay();
		while(1){
			/* WDT reset */
			clear_WDT();			/* reset WDT binary counter */
		}
	}
	
	tm16stop();
	pRead_FlashVersion((int8_t *)&Versions[0]);
	
	ledInit();
//	timerInit();
	tm67set();
	enable_irq();			/* enable interrupt (set IE)	*/
	
	gpioInit();
	uartInit();
	ssrControl(SSR_INVERTER, ON);
	ssrControl(SSR_SCR, ON);
	
	delay(2000);
	modbusInit();
	pLcd_Init();
	gDisplay.Connect = 1;
	pLcd_UpdateDisplay();
	clear_WDT();
	eepromInit();
	
	keyInit();
	cliInit();
	pn_EEP_Load();
	MAX6675_Init();
	
	gDispSub = LCD_MAIN_STATUS_0;
	gDispMode = LCD_MAIN_INIT;
	gLCD_Mode_Backup = gDispMode;
}
