#define _HW_MAIN_
#include "global.h"
#undef _HW_MAIN_
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
	//Hardware check and setting
	// 0. Inverter Set
	// 1. SSR1 output check
	
	pre_time = millis();
	while(gModBus.status == MODBUS_RECEIVE)
	{
		if(millis() - pre_time > 30)break;
		clear_WDT();
	}
	ssrOff();
	gModBus.p500msFlag = 1;
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
	gSetMonitor(0x0000, true);
	gModBus.p500msFlag = 1;
	gModBus.status = MODBUS_READY;
	gModBus.mode = MODBUS_MODE_MONITOR;
	modbusTransmit();
	gModBus.mode = 0;
	delay(50);
	
	
	gStatus.status = STATUS_INIT;

	
	// 4. Welcome message display
	
	
	
	
	delay(300);
	
	pre_time = millis();
	pre_time2 = millis();
	
	uartPrintf(_DEF_SERIAL1, "Welcome to HOTDOG Frier \n");
	pBeep(3, 0);
	gStatus.mainLoop = 1;
	P24RELAY = 1;
	//P6OUT |= 0x10;
	while(1)
	{
	    /* WDT reset */
		clear_WDT();			/* reset WDT binary counter */
	
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
				getSSRData(pData);
				transInformation((uint16_t *)pData);
			}
		
			pn_EEP_SaveCheck();
		}
		else
		{
			if(gModBus.p500msFlag == 1)
			{
				modbusTransmit();
			
			}
		}
		if(cFlag)
		{
			cFlag = 0;
			gSetStatus();

//			uartPrintf(_DEF_SERIAL1,"%d ",(gStatus.KSensor_Top.Temp/100));
//			uartPrintf(_DEF_SERIAL1,"%d \n",(gStatus.KSensor_Bottom.Temp/100));
		}
		gMotorCheck();
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
