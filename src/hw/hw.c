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
	
	eepromInit();
	gpioInit();
	uartInit();
	ssrControl(SSR_SSR, ON);
	delay(2000);
	pLcd_Init();
	pLcd_UpdateDisplay();
	delay(500);
	gDisplay.Request = DISPLAY_REQUEST_ON;
	gDisplay.Lines = TOP_LINE | BTM_LINE;
	pLcd_UpdateDisplay();
	modbusInit();
	clear_WDT();
	mototInit();
	adcInit();
	keyInit();
	cliInit();
	pn_EEP_Load();
#if(_USE_TEMP_SENSOR == RT02)
	MAX6675_Init();
#else
//	HX711_ADC PT100_TOP;
//	HX711_ADC PT100_BOTTOM;
	spiInit();
#endif
	
	delay(1000);
	gDispSub = LCD_MAIN_STATUS_0;
	gDispMode = LCD_MAIN_INIT;
	gLCD_Mode_Backup = gDispMode;
}
