//#define _HWMAIN_


#include "global.h"
#include "ap.h"

#include "swtimer.h"
//#undef _HWMAIN_

#define BASE_TEMP 10000
#define _DEF_OPTIME 15
void pTriacCheck(void);
void spiISR(void *arg);
uint8_t spi_ch;
void apInit(void)
{
  
//  uartOpen(_DEF_UART1, 57600);  // USART1
//  uartOpen(_DEF_UART2, 115200); // ESP8266
	cliOpen(_DEF_SERIAL1, 115200);
	spi_ch = swTimerGetHandle();
	swtimerSet(spi_ch, 250, 1, spiISR, NULL);
	swtimerStart(spi_ch);
}
void pHeaterCheck(void)
{
	uint16_t p1,p2;
	
	
	switch(gStatus.status){
		case STATUS_IDLE:
			p1 = gStatus.Heater_Top.Temp * 100 / gStatus.Heater_Top.Comp;
			p2 = gStatus.Heater_Bottom.Temp * 100 / gStatus.Heater_Bottom.Comp;
			if((p1 > 95) && (p2 > 95)){
				gStatus.status = STATUS_READY;
				pBeep(2, 1);
			}
			p1 = gStatus.Heater_Top.Comp + 6000;
			p2 = gStatus.Heater_Bottom.Comp + 6000;
			if((p1 < gStatus.Heater_Top.Temp) || (p2 < gStatus.Heater_Bottom.Temp)){
				gStatus.status = STATUS_ERR;
			//	relay(1, OFF);
			}
			break;
		case STATUS_ERR:
			if(gStatus.oldstatus != gStatus.status){
			//	relay(OFF);
			}
		/*	p1 = gStatus.Heater_Top.Comp + 6000;
			p2 = gStatus.Heater_Bottom.Comp + 6000;
			if((p1 > gStatus.Heater_Top.Temp) && (p2 > gStatus.Heater_Bottom.Temp)){
				gStatus.status = STATUS_READY;
				relay(ON);
			}*/
			break;
		case STATUS_READY:
		case STATUS_ON:
		default:
			p1 = gStatus.Heater_Top.Comp + 6000;
			p2 = gStatus.Heater_Bottom.Comp + 6000;
			if((p1 < gStatus.Heater_Top.Temp) || (p2 < gStatus.Heater_Top.Temp)){
				gStatus.status = STATUS_ERR;
			//	relay(OFF);
			}
			/*p1 = gStatus.Heater_Top.Temp * 100 / gStatus.Heater_Top.Comp;
			p2 = gStatus.Heater_Bottom.Temp * 100 / gStatus.Heater_Bottom.Comp;
			if((p1 < 95) || (p2 < 95)){
				pBeep(5, 0);
				gStatus.status = STATUS_IDLE;
			}*/
	}
	gStatus.oldstatus = gStatus.status;
}
void apMain(void)
{
	uint32_t pre_time,pre_time2;
	uint8_t check = 0;
	//Hardware check and setting
	// 0. Inverter Set
	pre_time = millis();
	while(gModBus.status == MODBUS_RECEIVE)
	{
		if(millis() - pre_time > 30)break;
		clear_WDT();
	}
	gSetCommand(MODBUS_INVERTER, 0x0005, 0x0001, true);
	gModBus.p500msFlag = 1;
	gModBus.status = MODBUS_READY;
	gModBus.mode = MODBUS_MODE_COMMAND;
	modbusTransmit();
	gModBus.mode = 0;
	delay(50);
	// 1. SSR1 output check
	pre_time = millis();
	while(gModBus.status == MODBUS_RECEIVE)
	{
		if(millis() - pre_time > 30)break;
		clear_WDT();
	}
	gSetMonitor(MODBUS_SSR1 , 0x3000, true);
	gModBus.p500msFlag = 1;
	gModBus.status = MODBUS_READY;
	gModBus.mode = MODBUS_MODE_MONITOR;
	modbusTransmit();
	gModBus.mode = 0;
	delay(50);
	if(Ssr1.voltage < 100)check = 1;
	
	// 2. SSR2 output check
	if(check == 1)
	{
		pre_time = millis();
		while(gModBus.status == MODBUS_RECEIVE)
		{
			if(millis() - pre_time > 30)break;
			clear_WDT();
		}
		gSetMonitor(MODBUS_SSR2 , 0x3000, true);
		//gModBus.p500msFlag = 1;
		gModBus.status = MODBUS_READY;
		gModBus.mode = MODBUS_MODE_MONITOR;
		modbusTransmit();
		gModBus.mode = MODBUS_MODE_COMMAND;
		delay(50);
		if(Ssr2.voltage < 100)check = 1;
		else check = 0;
	}
	
	// 3. not proble SSR4 On
	/*if(check)
	{*/
		ssrControl(4, 1);
		gStatus.status = STATUS_INIT;
//	}
	
	// 4. Welcome message display
	
	
	
	
	delay(300);
	gSetCommand(MODBUS_INVERTER , 0x0004, gIParameter.F_Freq, true);
	gModBus.p500msFlag = 1;
	gModBus.status = MODBUS_READY;
	gModBus.mode = MODBUS_MODE_COMMAND;
	modbusTransmit();
	gModBus.mode = 0;
	pre_time = millis();
	pre_time2 = millis();
	uartPrintf(_DEF_SERIAL1, "Welcome to Waffle machine \n");
	pBeep(3, 0);
	
	while(1)
	{
	    /* WDT reset */
		clear_WDT();			/* reset WDT binary counter */
		if(millis() - pre_time > 300){
			pre_time = millis();
		//	pTriacCheck();
		}
		if((gModBus.status == MODBUS_RECEIVE) && (gModBus.pre_time - 100 >= millis()))
		{
			gModBus.status = MODBUS_READY;
			if(++gModBus.no >= MAX_485_CH){
				gModBus.no = MODBUS_INVERTER;
				if(++gModBus.mode >= MODBUS_MODE_END)gModBus.mode = MODBUS_MODE_COMMAND;
			}
		}
		if(gModBus.p500msFlag == 1)
		{
			modbusCheck();
			modbusTransmit();
		}
		adc0Read();
		pLocKeyRead();
		pLocKeyProcess();
		pLocKeyRepeatProcess();
		if(System.EmergencyOn == 0)
		{
			gMotorKeyScan();
			pReverseKeyScan();
			pConvayorKeyScan();
			gHeaterKeyScan();
			if(gStatus.OldStart != gStatus.Start){
				if(gStatus.Start == 1){
					gStatus.pre_time = millis();
				}
				gStatus.OldStart = gStatus.Start;
			}
			pHeaterCheck();
			ssrUpdate();
		
			pn_EEP_SaveCheck();
		}
		gEmgKeyScan();
		pLcd_UpdateDisplay();
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
	if(gStatus.Heater_Top.Comp <= gStatus.Heater_Top.Temp)TRIAC_TOP = 0;
	else TRIAC_TOP = 1;
	if(gStatus.Heater_Bottom.Comp <= gStatus.Heater_Bottom.Temp)TRIAC_BOTTOM = 0;
	else TRIAC_BOTTOM = 1;
	if(gStatus.status > STATUS_IDLE){
		if(TRIAC_TOP && TRIAC_BOTTOM)
		{
			gStatus.status = STATUS_ON;
		}
		else gStatus.status = STATUS_READY;
	}
}
void spiISR(void *arg)
{
	readTemperature();
}
