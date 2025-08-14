#include "global.h"
#include "ssr.h"
#include "modbus.h"
#include "MAX6675.h"
#include "gpio.h"
#include "Pid.h"
#define FAN_TEMPERATURE 50
#define FAN_OUT ((*(RB_BTF*)&P8OUT).p3)

static _PIDS SSR_PID;

extern void buzzer_start(void);
extern void buzzer_stop(void);
extern void pStoreErrorMessage(uint8_t Msg);
void ssrInit(void)
{
	SSR_PID.P = 21.0;
	SSR_PID.I = 0.35;
	SSR_PID.D = 1.0;
	SSR_PID.basicLevel = 420;
	SSR_PID.measurepoint = gStatus.KSensor_Bottom.Temp;
	SSR_PID.setpoint = gStatus.Comp;
	if(gStatus.KSensor_Bottom.Temp > 12000)
	{
		SSR_PID.integral = (gStatus.KSensor_Bottom.Temp / 44);
	}
}
void ssrOn(void)
{
	gSetCommand(0x0002, 1, 1);
	gModBus.mode = MODBUS_MODE_COMMAND;
}
void ssrOff(void)
{
	gSetCommand(0x0002, 0, 1);
	gModBus.mode = MODBUS_MODE_COMMAND;
}
uint8_t ssrUpdate(void)
{
	static uint32_t pre_time;
	static uint8_t error1= 0;
	uint16_t pTemperature;
	short compare;
	uint8_t ret = 0;
	if(millis() - pre_time > 5000)
	{
		pre_time = millis();
		if(gStatus.status > STATUS_INIT)
		{
			if(gStatus.KSensor_Top.rechTarget == 0)
			{
				if(gStatus.KSensor_Top.Temp >= (gStatus.Comp - 150))
				{
					gStatus.KSensor_Top.rechTarget = 1;
				}
			}
			else
			{
				compare = gStatus.Comp - gStatus.KSensor_Top.Temp;
				if(compare > 200 || compare < -200)
				{
					gStatus.KSensor_Top.rechTarget = 0;
				}
			}
			compare = gStatus.Comp * 1.05;
			SSR_PID.measurepoint = gStatus.KSensor_Bottom.Temp;
			if(gStatus.KSensor_Bottom.Temp > compare)SSR_PID.value = 0;
			else getPidControl(&SSR_PID);//SSR.PID.value = pidControl();
			if(SSR_PID.value >= 420)SSR_PID.integral = 0;
			if(SSR.output == 1)	gSetCommand(0x0000, SSR_PID.value, 1);
			else gSetCommand(0x0002, 1, 1);
			ret = 1;
		}
		else{
			if(SSR.output == 1)ssrOff();
		}
		pTemperature = SSR.temperature;
		
		if(gFan == 1)
		{
			if(pTemperature < (FAN_TEMPERATURE - 5))
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
	if(gError.bit.ssrError == 0)
	{
		if((SSR.output == 0) && (SSR.voltage > 100))error1++;
		else if((SSR.output == 0) && (SSR.voltage < 100))error1 = 0;
		if(error1 > 10){
			gError.bit.ssrError = 1;
			error1 = 0;
			buzzer_start();
			ssrControl(SSR_FAN, gFan);
			pStoreErrorMessage(Error_SSR_HW_ERROR);
		}
	}
	return ret;
}
void pidPrameterReset(void)
{
	SSR_PID.integral = 0;
	SSR_PID.prev_error = 0;
}

void getSSRData(uint16_t *data)
{
	data[0] = gStatus.KSensor_Bottom.Temp;
	data[1] = (uint16_t)SSR_PID.integral;
	data[2] = SSR_PID.value;
}
