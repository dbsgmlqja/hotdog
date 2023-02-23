//#define _HWMAIN_


#include "global.h"
#include "ap.h"

#include "swtimer.h"
//#undef _HWMAIN_

#define BASE_TEMP 10000
#define _DEF_OPTIME 15
void pTriacCheck(void);
void adcISR(void *arg);
void apInit(void)
{
  
//  uartOpen(_DEF_UART1, 57600);  // USART1
//  uartOpen(_DEF_UART2, 115200); // ESP8266
	cliOpen(_DEF_SERIAL1, 115200);
	P3DIR |= 0x06;
	P5DIR |= 0x02;
	relay(OFF);
}
void pHeaterCheck(void)
{
	uint16_t p1,p2;
	
	
	switch(gStatus.status){
		case STATUS_IDLE:
			p1 = gADC.Temp1 * 100 / gStatus.Comp1;
			p2 = gADC.Temp2 * 100 / gStatus.Comp2;
			if((p1 > 95) && (p2 > 95)){
				gStatus.status = STATUS_READY;
				pBeep(2, 1);
			}
			p1 = gStatus.Comp1 + 6000;
			p2 = gStatus.Comp2 + 6000;
			if((p1 < gADC.Temp1) || (p2 < gADC.Temp2)){
				/*gStatus.status = STATUS_ERR;
				relay(OFF);*/
			}
			break;
		case STATUS_ERR:
			
		/*	p1 = gStatus.Comp1 + 6000;
			p2 = gStatus.Comp2 + 6000;
			if((p1 > gADC.Temp1) && (p2 > gADC.Temp2)){
				gStatus.status = STATUS_READY;
				TRIAC_TOP = 0;
				TRIAC_BOTTOM = 0;
				delay(200);
				RELAY_OUT = 1;
			}*/
			break;
		case STATUS_READY:
		case STATUS_ON:
		default:
			p1 = gStatus.Comp1 + 6000;
			p2 = gStatus.Comp2 + 6000;
			if((p1 < gADC.Temp1) || (p2 < gADC.Temp2)){
				/*gStatus.status = STATUS_ERR;
				relay(OFF);*/
			}
			p1 = gADC.Temp1 * 100 / gStatus.Comp1;
			p2 = gADC.Temp2 * 100 / gStatus.Comp2;
			if((p1 < 95) || (p2 < 95)){
				pBeep(5, 0);
				gStatus.status = STATUS_IDLE;
			}
	}
}
void apMain(void)
{
	uint32_t pre_time,pre_time2;
	pre_time = millis();
	pre_time2 = millis();
	uartPrintf(_DEF_SERIAL1, "Welcome to Waffle machine \n");
	delay(300);
	adcRead();
	gStatus.status = STATUS_IDLE;//adcCheck();
	if(gStatus.status == STATUS_IDLE)relay(ON);
	else relay(OFF);
	while(1)
	{
	    /* WDT reset */
		clear_WDT();			/* reset WDT binary counter */
		if(millis() - pre_time > 300){
			pre_time = millis();
			pTriacCheck();
		}
		if(millis() - pre_time2 >= 170)
		{
			pre_time2 = millis();
			adcRead();
			uartPrintf(_DEF_SERIAL1, "%d\t%d\n",gADC.Temp1, gADC.Temp2);
		}
		FND_Update();
		gModeKeyScan();
		gStartKeyScan();
		pDecKeyScan();
		pIncKeyScan();
		if(gStatus.OldStart != gStatus.Start){
			if(gStatus.Start == 1){
				gStatus.pre_time = millis();
			}
			gStatus.OldStart = gStatus.Start;
		}
		pHeaterCheck();
		pn_EEP_SaveCheck();
//		gModeKeyReleaseCheck();
		
		ledStatus();
		cliMain();
	}
}
void pTriacCheck(void)
{
	uint16_t pRTime;
	uint32_t pTime = millis();
	if(gStatus.Start){
		pRTime = (gStatus.OpTime * 1000 - (pTime - gStatus.pre_time))/1000;
		if(pRTime <= gStatus.OpTime)gStatus.RunTime = pRTime;
		else gStatus.RunTime = gStatus.OpTime;
		if(pTime - gStatus.pre_time < (gStatus.OpTime * 1000)){
			
		}
		else{
			gStatus.Start = 0;
			pBeep(3, 1);
		}
	}
	else{
		gStatus.RunTime = gStatus.OpTime;
	}
	if(gStatus.Comp1 < gADC.Temp1)TRIAC_TOP = 0;
	else TRIAC_TOP = 1;
	if(gStatus.Comp2 < gADC.Temp2)TRIAC_BOTTOM = 0;
	else TRIAC_BOTTOM = 1;
	if(gStatus.status > STATUS_IDLE){
		if(TRIAC_TOP && TRIAC_BOTTOM)
		{
			gStatus.status = STATUS_ON;
		}
		else gStatus.status = STATUS_READY;
	}
}
