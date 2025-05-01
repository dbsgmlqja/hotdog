#include "global.h"
#include "ssr.h"
#include "modbus.h"
#include "MAX6675.h"
#include "gpio.h"
#define FAN_TEMPERATURE 50
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
uint8_t ssrUpdate(void)
{
	static uint32_t pre_time;
	static uint8_t error1= 0,error2 = 0;
	uint16_t pTemperature;
	short compare;
	uint8_t ret = 0;
	if(millis() - pre_time > 5000)
	{
		pre_time = millis();
		if(gStatus.status > STATUS_INIT)
		{
			if(gStatus.Heater_Top.rechTarget == 0)
			{
				if(gStatus.Heater_Top.Temp >= (gStatus.Heater_Top.Comp - 150))
				{
					gStatus.Heater_Top.rechTarget = 1;
				}
			}
			else
			{
				compare = gStatus.Heater_Top.Comp - gStatus.Heater_Top.Temp;
				if(compare > 200 || compare < -200)
				{
					gStatus.Heater_Top.rechTarget = 0;
				}
			}
#if 0			
			if(gStatus.Heater_Bottom.rechTarget == 0)
			{
				if(gStatus.Heater_Bottom.Temp > (gStatus.Heater_Bottom.Comp - 150))
				{
					gStatus.Heater_Bottom.rechTarget = 1;
				}
				compare = gStatus.Heater_Bottom.Comp * 1.05;
				if(gStatus.Heater_Bottom.Temp > compare)SSR_BOTTOM.PID.value = 0;
				else SSR_BOTTOM.PID.value = pidControlBottom();
			}
			else	//case reching target temperature
			{
				compare = gStatus.Heater_Bottom.Comp - gStatus.Heater_Bottom.Temp;
				if(compare > 300 || compare < -300)
				{
					gStatus.Heater_Bottom.rechTarget = 0;
				}
				if(compare > 0)	SSR_BOTTOM.PID.value = 65 + (compare * 0.11);
				else SSR_BOTTOM.PID.value = 65 + (compare * 0.12);
			}
#else
			compare = gStatus.Heater_Bottom.Comp * 1.05;
			if(gStatus.Heater_Bottom.Temp > compare)SSR_BOTTOM.PID.value = 0;
			else SSR_BOTTOM.PID.value = pidControlBottom();

#endif
			if((gStatus.Heater_Top.Temp >= gStatus.Heater_Top.Comp) && (gStatus.Heater_Top.Prev_Temp < gStatus.Heater_Top.Comp)){
				SSR_TOP.PID.integral = 20;
			}
			if((gStatus.Heater_Top.Temp <= gStatus.Heater_Top.Comp) && (gStatus.Heater_Top.Prev_Temp > gStatus.Heater_Top.Comp)){
				SSR_TOP.PID.integral = 50;
			}
			gStatus.Heater_Top.Prev_Temp = gStatus.Heater_Top.Temp;
			SSR_TOP.PID.value = pidControlTop();
			
			/*if((gStatus.Heater_Bottom.Temp >= gStatus.Heater_Bottom.Comp) && (gStatus.Heater_Bottom.Prev_Temp < gStatus.Heater_Bottom.Comp)){
				SSR_BOTTOM.PID.integral = 15;
			}
			if((gStatus.Heater_Bottom.Temp <= gStatus.Heater_Bottom.Comp) && (gStatus.Heater_Bottom.Prev_Temp > gStatus.Heater_Bottom.Comp)){
				SSR_BOTTOM.PID.integral = 40;
			}*/
			gStatus.Heater_Bottom.Prev_Temp = gStatus.Heater_Bottom.Temp;
			/*compare = gStatus.Heater_Bottom.Comp * 1.05;
			if(gStatus.Heater_Bottom.Temp > compare)SSR_BOTTOM.PID.value = 0;
			else SSR_BOTTOM.PID.value = pidControlBottom();*/
			if(SSR_TOP.output == 1)	gSetCommand(MODBUS_SSR_TOP, 0x0000, SSR_TOP.PID.value, 1);
			else gSetCommand(MODBUS_SSR_TOP, 0x0002, 1, 1);
			if(SSR_BOTTOM.output == 1) gSetCommand(MODBUS_SSR_BOTTOM, 0x0000, SSR_BOTTOM.PID.value, 1);
			else gSetCommand(MODBUS_SSR_BOTTOM, 0x0002, 1, 1);
			ret = 1;
		}
		else{
			if(SSR_TOP.output == 1)ssrOff(MODBUS_SSR_TOP);
			if(SSR_BOTTOM.output == 1)ssrOff(MODBUS_SSR_BOTTOM);
		}
		if(SSR_TOP.temperature >= SSR_BOTTOM.temperature)pTemperature = SSR_TOP.temperature;
		else pTemperature = SSR_BOTTOM.temperature;
		
		if(gFan == 1)
		{
			if(pTemperature < FAN_TEMPERATURE)
			{
				gFan = 0;
				ssrControl(SSR_FAN, gFan);
			}
		}
		else
		{
			if(pTemperature >= FAN_TEMPERATURE)
			{
				gFan = 1;
				ssrControl(SSR_FAN, gFan);
			}
		}
	}
	
	//Error Check
	if(gError.bit.ssrTopError == 0)
	{
		if((SSR_TOP.output == 0) && (SSR_TOP.voltage > 100))error1++;
		else if((SSR_TOP.output == 0) && (SSR_TOP.voltage < 100))error1 = 0;
		if(error1 > 10){
			gError.bit.ssrTopError = 1;
			error1 = 0;
			ssrControl(SSR_FAN, gFan);
		}
	}
	if(gError.bit.ssrBottomError == 0)
	{
		if((SSR_BOTTOM.output == 0) && (SSR_BOTTOM.voltage > 100))error2++;
		else if((SSR_BOTTOM.output == 0) && (SSR_BOTTOM.voltage < 100))error2 = 0;
		if(error2 > 10){
			gError.bit.ssrBottomError = 1;
			error2 = 0;
			ssrControl(SSR_FAN, gFan);
		}
	}
	return ret;
}
