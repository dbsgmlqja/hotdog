#include "global.h"
#include "timer.h"
static uint16_t IntTimer;
void timerInit(void)
{
	TM16MD1 &= ~0xc000;
//	PSW &= ~0x0800;
	G31ICR &= ~0x0400;
	TM16CA = 0x752f;
	TM16MD1 = 0x0002;
	TM16MD1 |= 0x2000;
	TM16MDA &= ~0x0080;
	TM16MD1 |= 0x0040;
	TM16MD1 &= ~0x0040;
	G4ICR |= 0x0200;
	TM16MD1 |= 0x0080;
}
void p1msHandlingTimer(void)
{
	/*uint8_t i;
	if(gModBus.rxTmr[MODBUS_INVERTER] > 1)
	{
		gModBus.rxTmr[gModBus.no]--;
	}
	else if(gModBus.rxTmr[MODBUS_INVERTER] == 1){
		gModBus.status = MODBUS_READY;
		gModBus.rxTmr[MODBUS_INVERTER] = 0;
	}
	else
	{
		
	}
	if(gModBus.status == MODBUS_RECEIVE)
	{
		if(millis() - gModBus.pre_time > 20)
		{
			if(gModBus.comm[gModBus.no] < 10)
			{
				gModBus.comm[gModBus.no]++;
			}
			else if(gModBus.comm[gModBus.no] == 10)
			{
				gModBus.comm[gModBus.no] = 0xff;
				SC3STR &= ~0x02;
			}
		}
	}*/
}
void p10msHandlingTimer(void)
{
	if((IntTimer%10) == 0){
		gKeyScanFlag = 1;
		if(gKey.RepeatTimer != 0) --gKey.RepeatTimer;
		if(gSaveDisplayTimer != 0) --gSaveDisplayTimer;
	}
}
void p100msHandlingTimer(void)
{
	if((IntTimer%100) == 0){
		gModBus.p500msFlag = 1;
		
		gLCD_Update_Req = 1;
	}
}
void p200msHandlingTimer(void)
{
	if((IntTimer%200) == 0){
	
	}
}
void p500msHandlingTimer(void)
{
	if((IntTimer%500) == 0){
		
		if(gDisplay.Request == DISPLAY_REQUEST_OFF)
		{
			if(++gLCD_Refresh_Timer >= 2){	//20160314 by Bang 4->2
				gDispMode = gLCD_Mode_Backup;
				gDisplay.Request = DISPLAY_REQUEST_ON;
				gDisplay.Lines |= (TOP_LINE|BTM_LINE);
				gLCD_Refresh_Timer = 0;
			}
			
		}
	}
}
void p1msTimer(void)
{
	uint32_t pTime,pTime2,s1end,s2end;
	uint16_t pTemp,pTemp2,tempDiff;
	p1msHandlingTimer();
	p10msHandlingTimer();
	p100msHandlingTimer();
	p200msHandlingTimer();
	p500msHandlingTimer();
	if(++IntTimer >= 1000){
		IntTimer = 0;
		//gModBus.p500msFlag = 1;
		if(gStatus.warmupComplete == 0){
			pTime = gParameter.WarmupTime * 60;
			if(gWarmupTimer < pTime){
				if(gStatus.status == STATUS_IDLE){
					gWarmupTimer++;
					if(gStatus.warmupArrived == 1)
					{
						tempDiff = (gParameter.WarmupTemp - gParameter.TopTemp) / 3;
						pTime2 = gWarmupTimer - gStatus.warmupTimer;
						s1end = gStatus.warmupTimer + (pTime - gWarmupTimer) / 3;
						s2end = gStatus.warmupTimer + (pTime - gWarmupTimer) * 2 / 3;
						if(gWarmupTimer < s1end) gStatus.Heater_Top.Comp = gParameter.WarmupTemp - tempDiff;
						else if(gWarmupTimer < s2end)gStatus.Heater_Top.Comp = gParameter.WarmupTemp - tempDiff * 2;
						else gStatus.Heater_Top.Comp = gParameter.WarmupTemp - tempDiff * 3;
					}
				}
			}
			else
			{
				gStatus.warmupComplete = 1;
				gStatus.Heater_Top.Comp = gParameter.TopTemp;
				gStatus.Heater_Bottom.Comp = gParameter.BottomTemp;
				gStatus.status = STATUS_READY;
			}
		}
	}
}
