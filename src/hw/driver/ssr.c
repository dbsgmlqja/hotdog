#include "global.h"
#include "ssr.h"
#include "modbus.h"
#include "MAX6675.h"
#include "gpio.h"
#define FAN_OUT ((*(RB_BTF*)&P8OUT).p3)
void ssrOn(uint8_t ch)
{
	gSetCommand(ch, 0x0002, 1, 1);
	gModBus.mode = MODBUS_MODE_COMMAND;
}
void ssrOff(uint8_t ch)
{
	gSetCommand(ch, 0x0002, 0, 1);
	gModBus.mode = MODBUS_MODE_COMMAND;
}
void ssrUpdate(void)
{
	static uint32_t pre_time;
	static uint8_t error1= 0,error2 = 0;
	uint16_t pTemperature;
	if(millis() - pre_time > 2000)
	{
		pre_time = millis();
		if(gStatus.status > STATUS_INIT)
		{
			if(gStatus.status == STATUS_IDLE)
			{
				SSR_TOP.PID.value = 400;
				SSR_BOTTOM.PID.value = 400;
			}
			else
			{
				SSR_TOP.PID.value = pidControlTop();
				SSR_BOTTOM.PID.value = pidControlBottom();
			}
			if(SSR_TOP.output == 1)	gSetCommand(MODBUS_SSR_TOP, 0x0000, SSR_TOP.PID.value, 1);
			else gSetCommand(MODBUS_SSR_TOP, 0x0002, 1, 1);
			if(SSR_BOTTOM.output == 1) gSetCommand(MODBUS_SSR_BOTTOM, 0x0000, SSR_BOTTOM.PID.value, 1);
			else gSetCommand(MODBUS_SSR_BOTTOM, 0x0002, 1, 1);
		}
		else{
			if(SSR_TOP.output == 1)ssrOff(MODBUS_SSR_TOP);
			if(SSR_BOTTOM.output == 1)ssrOff(MODBUS_SSR_BOTTOM);
		}
		if(SSR_TOP.temperature >= SSR_BOTTOM.temperature)pTemperature = SSR_TOP.temperature;
		else pTemperature = SSR_BOTTOM.temperature;
		
		if(gFan == 1)
		{
			if(pTemperature < 31)
			{
				gFan = 0;
				ssrControl(2, gFan);
			}
		}
		else
		{
			if(pTemperature >= 32)
			{
				gFan = 1;
				ssrControl(2, gFan);
			}
		}
	}
	
	//Error Check
	/*if(System.ssrTopError == 0)
	{
		if((SSR_TOP.output == 0) && (SSR_TOP.voltage > 100))error1++;
		else if((SSR_TOP.output == 0) && (SSR_TOP.voltage < 100))error1 = 0;
		if(error1 > 10){
			System.ssrTopError = 1;
			error1 = 0;
			ssrControl(4, gFan);
		}
	}
	if(System.ssrBottomError == 0)
	{
		if((SSR_BOTTOM.output == 0) && (SSR_BOTTOM.voltage > 100))error2++;
		else if((SSR_BOTTOM.output == 0) && (SSR_BOTTOM.voltage < 100))error2 = 0;
		if(error2 > 10){
			System.ssrBottomError = 1;
			error2 = 0;
			ssrControl(4, gFan);
		}
	}*/
}
