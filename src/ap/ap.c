#include "global.h"
#include "ap.h"
#include "swtimer.h"

#define BASE_TEMP 10000
#define _DEF_OPTIME 15
void spiISR(void *arg);
uint8_t spi_ch;
static uint8_t cFlag = 0;
void apInit(void)
{
	cliOpen(_DEF_SERIAL1, 115200);
	spi_ch = swTimerGetHandle();
	swtimerSet(spi_ch, 250, 1, spiISR, NULL);
	swtimerStart(spi_ch);
}
void apMain(void)
{
	uint32_t pre_time,pre_time2;
	uint8_t check = 0;
	static pNo = 0;
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
	gModBus.rxMode = gModBus.mode;
	gModBus.mode = MODBUS_MODE_COMMAND;
	modbusTransmit();
	gModBus.mode = 0;;
	delay(50);
	gSetMonitor(MODBUS_INVERTER, INV_MONITOR_ADDR, 1);
	gSetCommand(MODBUS_INVERTER, 0x0004, gIParameter.F_Freq, 1);
	// 1. SSR1 output check
	
	pre_time = millis();
	while(gModBus.status == MODBUS_RECEIVE)
	{
		if(millis() - pre_time > 30)break;
		clear_WDT();
	}
	ssrOff(MODBUS_SSR_TOP);
	gModBus.p500msFlag = 1;
	gModBus.no = MODBUS_SSR_TOP;
	gModBus.status = MODBUS_READY;
	gModBus.mode = MODBUS_MODE_COMMAND;
	modbusTransmit();
	delay(30);
	pre_time = millis();
	while(gModBus.status == MODBUS_RECEIVE)
	{
		if(millis() - pre_time > 30)break;
		clear_WDT();
	}
	gSetMonitor(MODBUS_SSR_TOP , 0x0000, true);
	gModBus.p500msFlag = 1;
	gModBus.status = MODBUS_READY;
	gModBus.mode = MODBUS_MODE_MONITOR;
	modbusTransmit();
	gModBus.mode = 0;
	delay(50);
	if(SSR_TOP.voltage < 100)check = 1;
	
	// 2. SSR2 output check
	if(check == 1)
	{
		pre_time = millis();
		while(gModBus.status == MODBUS_RECEIVE)
		{
			if(millis() - pre_time > 30)break;
			clear_WDT();
		}
		ssrOff(MODBUS_SSR_BOTTOM);
		gModBus.p500msFlag = 1;
		gModBus.no = MODBUS_SSR_BOTTOM;
		gModBus.status = MODBUS_READY;
		gModBus.mode = MODBUS_MODE_COMMAND;
		modbusTransmit();
		delay(30);
		pre_time = millis();
		while(gModBus.status == MODBUS_RECEIVE)
		{
			if(millis() - pre_time > 30)break;
			clear_WDT();
		}
		gSetMonitor(MODBUS_SSR_BOTTOM , 0x0000, true);
		//gModBus.p500msFlag = 1;
		gModBus.status = MODBUS_READY;
		gModBus.mode = MODBUS_MODE_MONITOR;
		modbusTransmit();
		gModBus.mode = MODBUS_MODE_COMMAND;
		delay(50);
		if(SSR_BOTTOM.voltage < 100)check = 1;
		else check = 0;
	}
	
	// 3. not proble SSR4 On
	/*if(check)
	{*/
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
		
		/*if((gModBus.status == MODBUS_RECEIVE) && (millis() - gModBus.pre_time >= 100))
		{
			gModBus.status = MODBUS_READY;
			if(++gModBus.no >= MAX_485_CH){
				gModBus.no = MODBUS_INVERTER;
				if(++gModBus.mode >= MODBUS_MODE_END)gModBus.mode = MODBUS_MODE_COMMAND;
			}
		}*/
		adc0Read();
		pLocKeyRead();
		pLocKeyProcess();
		pLocKeyRepeatProcess();
		if(System.EmergencyOn == 0)
		{
			if(cFlag)
			{
				cFlag = 0;
				readTemperature();
				gSetStatus();
	
				uartWrite(_DEF_SERIAL1,gStatus.Heater_Top.Temp/100);
				uartWrite(_DEF_SERIAL1,gStatus.Heater_Bottom.Temp/100);
			}
			if(gModBus.p500msFlag == 1)
			{
				modbusCheck();
				modbusTransmit();
			}
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
			ssrUpdate();
		
			pn_EEP_SaveCheck();
		}
		else
		{
			if(gModBus.p500msFlag == 1)
			{
				gModBus.no = MODBUS_INVERTER;
				if(pNo == 0)
				{
					gModBus.mode = MODBUS_MODE_ERROR;
					modbusTransmit();
					pNo = 1;
				}
				else
				{
					if(gInverterStatus.bit.stop == 0){
						gSetCommand(MODBUS_INVERTER, 0x0005, 1, 1);
						gModBus.mode = MODBUS_MODE_COMMAND;
						modbusTransmit();
					}
					pNo = 0;
				}
				gModBus.status = 0;
			}
		}
		
		gEmgKeyScan();
		pLcd_UpdateDisplay();
		pLcd_Init_Seq();
		ledStatus();
		cliMain();
	}
}
void spiISR(void *arg)
{
	cFlag = 1;
}
