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
	swtimerSet(spi_ch, 1000, 1, spiISR, NULL);
	swtimerStart(spi_ch);
}
static uint16_t pData[4];
void apMain(void)
{
	uint32_t pre_time,pre_time2;
	uint8_t check = 0;
	static uint8_t readyTop = 0;
	static uint8_t readyBottom = 0;
	static pNo = 0;
	float pTemperature;
	float R;
  	float T;
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
	delay(100);
	gSetMonitor(MODBUS_INVERTER, INV_MONITOR_ADDR, 1);
	gSetCommand(MODBUS_INVERTER, 0x0004, gIParameter.F_Freq, 1);
	modbusTransmit();
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
	delay(50);
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
#ifdef _USE_HW_BOTTOM_HEATER		
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
#endif		
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
	
	uartPrintf(_DEF_SERIAL1, "Welcome to Bakery machine \n");
	pBeep(3, 0);
	gStatus.mainLoop = 1;
	while(1)
	{
	    /* WDT reset */
		clear_WDT();			/* reset WDT binary counter */
#if(_USE_TEMP_SENSOR == PT100)
		updateSpi();
#endif
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
		if(gError.bit.EmergencyOn == 0)
		{
			
			if(gModBus.p500msFlag == 1)
			{
				modbusCheck();
				modbusTransmit();
			}
			gKeyScan();
			if(gStatus.OldStart != gStatus.Start){
				if(gStatus.Start == 1){
					gStatus.pre_time = millis();
				}
				gStatus.OldStart = gStatus.Start;
			}
			if(ssrUpdate() == 1){
				pData[0] = gStatus.Heater_Bottom.Temp;
				pData[1] = (uint16_t)SSR_BOTTOM.PID.integral;
				pData[2] = SSR_BOTTOM.PID.value;
				transInformation((uint16_t *)pData);
			}
		
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
		if(cFlag)
		{
			cFlag = 0;
			gSetStatus();

//			uartPrintf(_DEF_SERIAL1,"%d ",(gStatus.Heater_Top.Temp/100));
//			uartPrintf(_DEF_SERIAL1,"%d \n",(gStatus.Heater_Bottom.Temp/100));
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
	static uint8_t num = 0;
#if(_USE_TEMP_SENSOR == RT02)	
	pSetTemperature();
#endif
	if(++num >= 5){
		num = 0;
		cFlag = 1;
	}
}
