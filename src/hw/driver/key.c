#include "global.h"
#include "key.h"
#include "eeprom.h"
#include "led.h"
#include "modbus.h"
#include "gpio.h"
#include "ssr.h"
#if(CUSTOMER == AJUN)
//#define REVERSE_KEY_IN 	((*(RB_BTF*)&PCIN).p1)
#define CONVAYOR_KEY_IN ((*(RB_BTF*)&PCIN).p1)
#define EMG_KEY_IN 		((*(RB_BTF*)&PCIN).p3)
#define HEATER_KEY_IN	((*(RB_BTF*)&PDIN).p2)
#else
#define REVERSE_KEY_IN 	((*(RB_BTF*)&PCIN).p1)
#define CONVAYOR_KEY_IN ((*(RB_BTF*)&PCIN).p2)
#define EMG_KEY_IN 		((*(RB_BTF*)&PCIN).p3)
#define HEATER_KEY_IN	((*(RB_BTF*)&PDIN).p1)
#endif
extern void buzzer_start(void);
extern void buzzer_stop(void);
extern void pLcd_UpdateDisplay(void);
extern void pBeep(uint8_t Cnt, uint8_t sl);
extern short getTemperature(uint8_t no);
//extern uint16_t EEP_ParaTBL[][3];

extern uint16_t EEP_HEAT_ParaTBL[][3];
extern uint16_t EEP_Inverter_ParaTBL[][3];
typedef struct
{
  volatile uint8_t *port;
  uint8_t      pin;
  uint8_t on_state;
} key_tbl_t;
key_obj_t btn_reverse;
key_obj_t btn_convayor;
key_obj_t btn_emg;
key_obj_t btn_heater;
key_obj_t btn_motor;
key_obj_t btn_motor2;
key_obj_t btn_am;
static _Para gDispParameter;
#if(CUSTOMER == AJUN)
const key_tbl_t key_TBL[3] = {
	{&PCIN, 0x02, 0x00},	//convayor
	{&PCIN, 0x08, 0x08},	//emg
	{&PCIN, 0x04, 0x00},	//heater
};
#else
const key_tbl_t key_TBL[3] = {
	{&PCIN, 0x04, 0x00},	//convayor
	{&PCIN, 0x08, 0x08},	//emg
	{&PDIN, 0x02, 0x00},	//heater
};
#endif
void keyInit(void)
{
	keyObjCreate(&btn_convayor, _DEF_CONVAYOR_KEY, 100);
	keyObjCreate(&btn_emg, _DEF_EMG_KEY, 1000);
	keyObjCreate(&btn_heater, _DEF_HEATER_KEY, 1500);
	PCIE |= 0x0E;
}

void keyObjCreate(key_obj_t *p_obj, uint8_t ch, uint32_t repeat_time)
{
	p_obj->ch = ch;
	p_obj->state = 0;
	p_obj->pre_time = millis();
	p_obj->repeat_time = repeat_time;
}

uint8_t keyObjGetPressed(key_obj_t *p_obj, uint32_t pressed_time)
{
	uint8_t ret = false;
	
	switch(p_obj->state)
	{
		case 0:
			if (keyGetPressed(p_obj->ch) == true)
			{
			  p_obj->state = 1;
			  p_obj->pre_time = millis();
			}
			break;
	
	    case 1:
			if (keyGetPressed(p_obj->ch) == true)
			{
				if (millis()-p_obj->pre_time >= pressed_time)
				{
					ret = true; // ? ????
					p_obj->state = 2;
					p_obj->pre_time = millis();
				}
			}
			else
			{
				p_obj->state = 0;
			}
			break;
	
	    case 2:
	      if (keyGetPressed(p_obj->ch) == true)
	      {
	        ret = true;
			if (millis()-p_obj->pre_time >= p_obj->repeat_time)
	        {
	          p_obj->state = 1;
	          p_obj->pre_time = millis();
	        }
	      }
	      else
	      {
	        p_obj->state = 0;
	      }
	      break;
		  
	}
	return ret;
}
uint8_t keyGetPressed(uint8_t ch)
{
	uint8_t ret = false;
		
	if(ch >= KEY_MAX_CH)return false;
	if((*key_TBL[ch].port & key_TBL[ch].pin) == key_TBL[ch].on_state){
		ret = true;
	}
	return ret;
}
void pConvayorKeyScan(void)
{
	static uint8_t IncKeyOn = 0;
	
	if(keyObjGetPressed(&btn_convayor, 60) == true)
	{
		if(btn_convayor.state == 2)
		{
			gSwitch.InvPreTime = millis();
			btn_convayor.pre_time = millis();
			if(IncKeyOn == 0)
			{
				IncKeyOn = 1;
				switch(gMotorStatus)
				{
					case MOTOR_OFF:
						gFreqCounter = 0;
						gMotorTime = millis();
						gMotorStatus = MAX_ZONE;
						ledOn(LED_CONVAYOR);
						break;
					case ACC_ZONE:
					case DEC_ZONE:
					case MAX_ZONE:
						gFreqCounter = 0;
						gNowPWM = gWorkPWM;
						gMotorStatus = DEC_ZONE;
						ledOff(LED_CONVAYOR);
						break;
				}
				
			}
		}
	}
	else
	{
		IncKeyOn = 0;
	}
}
void gEmgKeyScan(void)
{
	static uint8_t StartKeyContinue = 0;
	if(keyObjGetPressed(&btn_emg, 60) == true)
	{
		if(btn_emg.state > 1)
		{
			btn_emg.pre_time = millis();
			if(StartKeyContinue == 0)
			{
				StartKeyContinue = 1;
				gError.bit.EmergencyOn = 1;
				buzzer_start();
				errorProccess();
				ledOff(LED_HEATER);
				ledOff(LED_CONVAYOR);
				if(gMotorStatus != MOTOR_OFF)
				{
					ledOff(LED_CONVAYOR);
					gMotorStatus = DEC_ZONE;
				}
				if(gStatus.status > STATUS_INIT){
					gStatus.status = STATUS_INIT;
					gStatus.Start = 0;
					gResetSSR();
					gSetCommand(0x0002, 0, 1);
					gModBus.mode = MODBUS_MODE_COMMAND;
					pBeep(3, 0);
					
				}
				else if(gStatus.status == STATUS_ERR)
				{
					pBeep(3, 0);
				}
				else{
					pBeep(5, 0);
				}
			}
		}
	}
	else
	{
		if(btn_emg.state == 0){
			StartKeyContinue = 0;
			if(gError.bit.EmergencyOn){
				gError.bit.EmergencyOn = 0;
				ledOff(LED_HEATER);
				ledOff(LED_CONVAYOR);
				
				if(gError.All == 0)
				{
					errorRelease();
					buzzer_stop();
					/*display reflash*/
					gDisplay.Request = DISPLAY_REQUEST_ON;
					gDisplay.Lines = TOP_LINE | BTM_LINE;
					pLcd_UpdateDisplay();
					delay(2000);
					gError.bit.ssrCommError = modbusConnectCheck();
				}
			}
		}
		gError.bit.EmergencyOn = 0;
	}
}
void gHeaterKeyScan(void)
{
	static uint8_t HeaterKeyOn = 0;
	uint16_t pVar1;
	short value;
	if(keyObjGetPressed(&btn_heater, 60) == true)
	{
		if(btn_heater.state == 2)
		{
			btn_heater.pre_time = millis();
			if(HeaterKeyOn == 0)
			{
				HeaterKeyOn = 1;
				if(gStatus.status != STATUS_ERR)
				{
					if(gStatus.status == STATUS_INIT){
						pVar1 = gStatus.Comp - READY_TEMPERATURE;
						value = getTemperature(1);
						if(pVar1 > value)	gStatus.status = STATUS_IDLE;
						else gStatus.status = STATUS_READY;
						pidPrameterReset();
						
						gModBus.no = MODBUS_SSR_TOP;
						if((gModBus.status > MODBUS_READY) && (gModBus.rxMode == 0xFF)) gModBus.rxMode = gModBus.mode;
						gModBus.mode = MODBUS_MODE_COMMAND;
						gSetCommand(0x0002, 1, 1);//gSetCommand(MODBUS_SSR_TOP, 0x0002, 1, 1);
					}
					else
					{
						gStatus.status = STATUS_INIT;
						if((gModBus.status > MODBUS_READY) && (gModBus.rxMode == 0xFF)) gModBus.rxMode = gModBus.mode;
						gModBus.mode = MODBUS_MODE_COMMAND;
						gSetCommand( 0x0002, 0, 1);
					}
				}
			}
		}
	}
	else
	{
		HeaterKeyOn = 0;
	}
}
void gKeyScan(void)
{
	if(gError.All > 0)return;
	gHeaterKeyScan();
	gEmgKeyScan();
	pConvayorKeyScan();
}


void 	pLKey_Menu(void);
void	pLKey_Up(void);
void	pLKey_Set(void);
void	pLKey_Down(void);

#define	VOLUME_STEP_1	500		/*0.05*/ 
#define	VOLUME_STEP_2	1600	/*1.46 */
#define	VOLUME_STEP_3	2600	/*2.44 */
#define	VOLUME_STEP_4	3300	


unsigned char pLevelSlice(unsigned short Value){
	
	if(Value < VOLUME_STEP_1){
		return 1;	
	}
	else if(Value < VOLUME_STEP_2){
		return 2;	
	}
	else if(Value < VOLUME_STEP_3){
		return 3;	
	}
	else if(Value < VOLUME_STEP_4){
		return 4;	
	}
	else{
		return 0;
	}
}
unsigned char pKeyCodeGeneration(void)
{
	unsigned char Offset,Code;
	Offset = 0;
	Code = pLevelSlice(gKey_AD0);
	
	return Code;
}

void pLocKeyRead(void)
{
	static unsigned char KeyCnt;

		
	if(gKeyScanFlag == 0) return;
	gKeyScanFlag = 0;
	
	gKey.Buf[KeyCnt++] = pKeyCodeGeneration();
	KeyCnt %= 4;

	if(	(gKey.Buf[0]==gKey.Buf[1])&&
		(gKey.Buf[1]==gKey.Buf[2])&&
		(gKey.Buf[2]==gKey.Buf[3]))
	{
		gKey.OldCode = gKey.NewCode;
		gKey.NewCode = gKey.Buf[0];
		if(gKey.NewCode == 0){
			gKey.NewKeyIn = 0;
			gKey.RepeatKeyIn = 0;
		}
		else if((gKey.OldCode != gKey.NewCode)){
			gKey.NewKeyIn = 1;			
		}
		else{
			gKey.RepeatKeyIn = 1;
			
		}
	}
}

void pLocKeyRepeatProcess(void)
{
	if(gKey.NewKeyIn == 0){
        if((gKey.RepeatTimer == 0)&&(gKey.RepeatKeyIn));
        else	return;
    }
	if(gKey.RepeatKeyIn)
	{
		if((gKey.NewCode != LKEY_DOWN)&&(gKey.NewCode != LKEY_UP)&&(gKey.NewCode != LKEY_MENU)){ //@150423->150430 Right Repeat key by Bang LKEY_RIGHT -> LKEY_LEFT 160513
			gKeyRepeatTimer = 0xff;
		}
 	}
	else gKeyRepeatTimer = 30;

}

void pLocKeyProcess(void)
{
	
	if(gKey.NewKeyIn == 0){
		if((gKey.RepeatTimer == 0)&&(gKey.RepeatKeyIn));
		else return;
	}
	/******************************************************/
	/*                                                                                           */
	/* Repeat Key Cancel                                                               */
	/*                                                                                           */
	/******************************************************/
	if(gKey.RepeatKeyIn){
		switch(gKey.NewCode){
			case LKEY_MENU:
			case LKEY_SET:
			gKey.NewKeyIn = 0;
			return;
		}
	}

	/******************************************************/
	/*                                                                                           */
	/* Repeat time set                                                                   */
	/*                                                                                           */
	/******************************************************/
	if(gKey.NewKeyIn)	gKey.RepeatTimer = 100;
	else					gKey.RepeatTimer = 10;
	
	
	/******************************************************/
	/*                                                                                           */
	/* Key Jump                                                                            */
	/*                                                                                           */
	/******************************************************/
	
	switch(gKey.NewCode)
	{
		case LKEY_MENU:
		pLKey_Menu();
		break;
		
		case LKEY_SET:
		pLKey_Set();
		break;
		
		case LKEY_UP:
		pLKey_Up();
		break;
		
		case LKEY_DOWN:
		pLKey_Down();
		break;
	
	
	}
	gKey.NewKeyIn = 0;
}
void pLKey_Menu(void)
{
	/*if(gStatus.status > STATUS_INIT)return;
	if(gMotorStatus > MOTOR_OFF)return;*/
	
	switch(gDispMode)
	{
		case LCD_MAIN_INIT:
			pBeep(1, 0);
			gDispMode = LCD_MAIN_MENU;
			gDispSub = LCD_SUB_MAIN_HEAT;
	
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines = TOP_LINE | BTM_LINE;
			break;
		case LCD_MAIN_MENU:
			gDispMode = LCD_MAIN_INIT;
			gDispSub = LCD_MAIN_STATUS_0;
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines = TOP_LINE | BTM_LINE;
			break;
		case LCD_MAIN_HEAT_SET:
		case LCD_MAIN_INV_SET:
		case LCD_MAIN_FAULT:
		case LCD_MAIN_INITIALIZE:
			gDispSub = gDispMode - 1;
			gDispMode = LCD_MAIN_MENU;
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines = TOP_LINE | BTM_LINE;
			break;
			
		default:
			gDispMode = LCD_MAIN_INIT;
			gDispSub = LCD_MAIN_STATUS_0;
	
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
}

void pLKey_Set(void)
{
	uint16_t *Ptr16;
	/*if(gStatus.status > STATUS_INIT)return;
	if(gMotorStatus > MOTOR_OFF)return;*/
	if(gDispMode == LCD_MAIN_INIT)
	{
		if(gError.bit.EmergencyOn == 0)
		{
			if(gError.bit.ssrCommError == 1)
			{
				if(modbusConnectCheck() == 0)
				{
					pBeep(2, 0);
					gReleaseError(0);
					gError.bit.ssrCommError = 0;
				}
				else pBeep(3,0);
				
				return;
			}
			
		}
	}
	if(gDispMode == LCD_MAIN_INIT){
		pBeep(3,0);
	}
	else if(gDispMode == LCD_MAIN_MENU){
		pBeep(1,0);
		switch(gDispSub)
		{
			case LCD_SUB_MAIN_HEAT:
				gDispMode = LCD_MAIN_HEAT_SET;
				gDispSub = LCD_SUB_HEAT_TEMPERATURE;
				break;

			case LCD_SUB_MAIN_INV:
				gDispMode = LCD_MAIN_INV_SET;
				gDispSub = LCD_SUB_INV_FREQ;

				gDisplay.Request = DISPLAY_REQUEST_ON;
				gDisplay.Lines = TOP_LINE | BTM_LINE;
				
				break;
		

			case LCD_SUB_MAIN_INITIALIZE:
				gDispMode = LCD_MAIN_INITIALIZE;
				gDispSub = LCD_SUB_INITILIZE;
				break;

			case LCD_SUB_MAIN_FAULTSCAN:
				gDispMode = LCD_MAIN_FAULT;
				gDispSub = LCD_SUB_HEAT_TEMPERATURE;
				break;

		}
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
	else if(gDispMode == LCD_MAIN_HEAT_SET){
		pBeep(1,0);
		/*if(gDispSub == LCD_SUB_HEAT_WARMUP)
		{
			gDispMode = LCD_MAIN_HEAT_SET_WARMUP;
			gDispSub = LCD_SUB_HEAT_TEMPERATURE;
		}
		else
		{*/
			gDispMode = LCD_MAIN_HEAT_SET_TOP_SUB;
			gDispSub = LCD_SUB_HEAT_TEMPERATURE;
			Ptr16 = (uint16_t *)&gParameter;
	 		
			gDispParameter.Value = *Ptr16;
					//@150423 by Bang
			gDispParameter.Min = EEP_HEAT_ParaTBL[0][0];
			gDispParameter.Max = EEP_HEAT_ParaTBL[0][1];
			
//		}
		/*gDispMode = LCD_MAIN_HEAT_SET_SUB;

		
		Ptr16 = (uint16_t *)&gParameter;
		Ptr16 += gDispSub;
 

		
		gDispParameter.Value = *Ptr16;
				//@150423 by Bang
		gDispParameter.Min = EEP_HEAT_ParaTBL[gDispSub][0];
		gDispParameter.Max = EEP_HEAT_ParaTBL[gDispSub][1];*/
		gDisplay.Request = DISPLAY_REQUEST_ON;
//		gDispMode = LCD_MAIN_HEAT_SET_SUB;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
/*	else if(gDispMode == LCD_MAIN_HEAT_SET_WARMUP)
	{
		gDispParameter.prev_sub = gDispSub;
		gDispMode = LCD_MAIN_HEAT_SET_WARMUP_SUB;
		gDispSub = LCD_SUB_WARMUP_TIME;
		Ptr16 = (uint16_t *)&gParameter;
		Ptr16 += (gDispParameter.prev_sub + 1);
		
		gDispParameter.Value = *Ptr16;
		gDispParameter.Min = EEP_HEAT_ParaTBL[gDispParameter.prev_sub+1][0];
		gDispParameter.Max = EEP_HEAT_ParaTBL[gDispParameter.prev_sub+1][1];
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}*/
	else if(gDispMode == LCD_MAIN_INV_SET){
		pBeep(1,0);
		gDispMode = LCD_MAIN_INV_SET_SUB;

		
		Ptr16 = (uint16_t *)&gIParameter;
		Ptr16 += gDispSub;
 

		
		gDispParameter.Value = *Ptr16;
		
		gDispParameter.Min = EEP_Inverter_ParaTBL[gDispSub][0];
		gDispParameter.Max = EEP_Inverter_ParaTBL[gDispSub][1];
		
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
	else if(gDispMode == LCD_MAIN_HEAT_SET_TOP_SUB){
		pBeep(1,0);
		/* Parameter Save */
		gParameter.Temperature = gDispParameter.Value;
		
		pn_EEP_WriteBytes(0x10, (uint8_t *)&gDispParameter.Value,/* LCD_SUB_INV_END*/2);
		gStatus.Comp = gParameter.Temperature;
		gSetSetpoint(gStatus.Comp);
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDispMode = LCD_MAIN_HEAT_SET;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
#if 0
	else if(gDispMode == LCD_MAIN_HEAT_SET_WARMUP_SUB)
	{
		pBeep(1,0);
		if(gDispParameter.prev_sub == LCD_SUB_WARMUP_TEMP)
		{
			/* Parameter Save */
			gParameter.WarmupTemp = gDispParameter.Value;
			if((gStatus.warmupComplete == 0) && (gStatus.warmupArrived == 0)){
				gStatus.Comp = gDispParameter.Value;
			}
			pn_EEP_WriteBytes(0x10 + (EEP_WARMUP_TEMP * 2), (uint8_t *)&gDispParameter.Value,/* LCD_SUB_INV_END*/2);
		}
		else
		{
			/* Parameter Save */
			gParameter.WarmupTime = gDispParameter.Value;
			
			pn_EEP_WriteBytes(0x10 + (EEP_WARMUP_TIME * 2), (uint8_t *)&gDispParameter.Value,/* LCD_SUB_INV_END*/2);
		}
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDispMode = LCD_MAIN_HEAT_SET_WARMUP;
		gDispSub = gDispParameter.prev_sub;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
#endif	
	else if(gDispMode == LCD_MAIN_INV_SET_SUB){
		pBeep(1,0);
		/* Parameter Save */
		Ptr16 = (uint16_t *)&gIParameter;
		Ptr16 += gDispSub;
		*Ptr16 = gDispParameter.Value;
		
		pn_EEP_WriteBytes(0x30 + gDispSub *2, (uint8_t *)&gDispParameter.Value,/* LCD_SUB_INV_END*/2);
		
		pn_Load_Parameters();
		gMotorTime = millis();
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDispMode = LCD_MAIN_INV_SET;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
	else if(gDispMode == LCD_MAIN_INITIALIZE){
		pBeep(2, 0);
		//ID ERASE
		pn_EepromWriteByte(0x00,0x00);
		pn_EEP_Load();

		gDispMode = LCD_MAIN_MENU;
		gDispSub = LCD_SUB_MAIN_INITIALIZE;
				
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
		pBeep(2,0);
	}
	else if(gDispMode == LCD_MAIN_FAULT)
	{
		switch(gDispSub)
		{
			case LCD_SUB_FAULT_READ:
				pBeep(1, 0);
				gDispMode = LCD_MAIN_FAULT_SUB;
				gDispSub = 0;
				gDisplay.Request = DISPLAY_REQUEST_ON;
				break;
						
			case LCD_SUB_FAULT_CLEAR:
				/*for(i=0;i!=10;i++){
					gMonitorError[i] = 0;
					pn_EepromWriteByte(0x90+i,gMonitorError[i]);
					delay(5);
				}*/
				gEraseFault();
				pBeep(1, 0);
				gDisplay.Request = DISPLAY_REQUEST_ON;
				gDispMode = LCD_MAIN_MENU;
				gDispSub = LCD_SUB_MAIN_FAULTSCAN;
				break;
		
		}
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
	else if(gDispMode == LCD_MAIN_FAULT_SUB)
	{
		pBeep(1, 0);
		gDispMode = LCD_MAIN_FAULT;
		gDispSub = LCD_SUB_FAULT_READ;
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
}
void pLKey_Up(void)
{
	/*if(gStatus.status > STATUS_INIT)return;
	if(gMotorStatus > MOTOR_OFF)return;*/
	switch(gDispMode)
	{
		case LCD_MAIN_INIT:
			switch(gDispSub)
			{
				case LCD_MAIN_STATUS_0:
					gDispSub++;
					break;
				case LCD_MAIN_STATUS_1:
					gSetMonitor(0x0000, 1);
					gDispSub++;
					break;
				default:
					gDispSub = LCD_MAIN_STATUS_0;
					break;
			}
			
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDispMode = LCD_MAIN_INIT;
			gDisplay.Lines = TOP_LINE | BTM_LINE;
			pBeep(1, 0);
			break;
			
		case LCD_MAIN_MENU:
			if(++gDispSub == LCD_SUB_DISP_END)	gDispSub = LCD_SUB_MAIN_HEAT;
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDispMode = LCD_MAIN_MENU;
			gDisplay.Lines = TOP_LINE | BTM_LINE;
			pBeep(1, 0);
			break;

		case LCD_MAIN_HEAT_SET:
			
			if(++gDispSub >= LCD_SUB_HEAT_END)		gDispSub = LCD_SUB_HEAT_TEMPERATURE;
			
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines |= BTM_LINE;
			pBeep(1,0);
			break;
/*		case LCD_MAIN_HEAT_SET_WARMUP:
			if(++gDispSub >= LCD_SUB_WARMUP_END)	gDispSub = LCD_SUB_WARMUP_TIME;
			
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines |= BTM_LINE;
			pBeep(1,0);
			break;*/
		case LCD_MAIN_INV_SET:
			if(++gDispSub >= LCD_SUB_INV_END)		gDispSub = LCD_SUB_INV_FREQ;
			
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines |= BTM_LINE;
			pBeep(1, 0);
			break;
		case LCD_MAIN_HEAT_SET_SUB:
			/*if(gDispParameter.Value < gDispParameter.Max){
				if(gDispParameter.Value < gDispParameter.Min){
					gDispParameter.Value = gDispParameter.Min;
				}
				gDisplay.Request = DISPLAY_REQUEST_ON;
				gDisplay.Lines |= BTM_LINE;
			}
			if((gKey.RepeatTimer == 0) && (!gKey.RepeatKeyIn))	pBeep(1,0);
			break;*/
		/*case LCD_MAIN_HEAT_SET_WARMUP_SUB:
			if(gDispParameter.prev_sub == 0)
			{
				if(gDispParameter.Value < gDispParameter.Max){
					if(gDispParameter.Value < gDispParameter.Min){
						gDispParameter.Value = gDispParameter.Min;
					}
					else gDispParameter.Value++;
						
					gDisplay.Request = DISPLAY_REQUEST_ON;
					gDisplay.Lines |= BTM_LINE;
				}
				if((gKey.RepeatTimer) && (!gKey.RepeatKeyIn))	pBeep(1,0);
			}
			else
			{
				if(gDispParameter.Value <= (gDispParameter.Max - 100)){
					gDispParameter.Value += 100;
					
					if(gDispParameter.Value < gDispParameter.Min){
						gDispParameter.Value = gDispParameter.Min;
					}
					gDisplay.Request = DISPLAY_REQUEST_ON;
					gDisplay.Lines |= BTM_LINE;
				}
				if((gKey.RepeatTimer) && (!gKey.RepeatKeyIn))	pBeep(1,0);
			}
			break;*/
		case LCD_MAIN_HEAT_SET_TOP_SUB:
			if(gDispParameter.Value <= (gDispParameter.Max - 100)){
				gDispParameter.Value += 100;
				
				if(gDispParameter.Value < gDispParameter.Min){
					gDispParameter.Value = gDispParameter.Min;
				}
				gDisplay.Request = DISPLAY_REQUEST_ON;
				gDisplay.Lines |= BTM_LINE;
			}
			if((gKey.RepeatTimer) && (!gKey.RepeatKeyIn))	pBeep(1,0);
		
			break;
		case LCD_MAIN_INV_SET_SUB:
			if(gDispParameter.Value < gDispParameter.Max){
				gDispParameter.Value++;
				
				if(gDispParameter.Value < gDispParameter.Min){
					gDispParameter.Value = gDispParameter.Min;
				}
				gDisplay.Request = DISPLAY_REQUEST_ON;
				gDisplay.Lines |= BTM_LINE;
			}
			if((gKey.RepeatTimer) && (!gKey.RepeatKeyIn))	pBeep(1,0);
			break;
		//@150427  Edit by Bang
		case LCD_MAIN_INITIALIZE:
			pBeep(3, 0);

//@150427 by Bang				pBeep(3);
			break;

		case LCD_MAIN_FAULT:
			if(gDispSub == LCD_SUB_FAULT_CLEAR)		gDispSub = LCD_SUB_FAULT_READ;
			else											gDispSub = LCD_SUB_FAULT_CLEAR;
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDispMode = LCD_MAIN_FAULT;
			gDisplay.Lines |= BTM_LINE;
			pBeep(1, 0);
			break;

		case LCD_MAIN_FAULT_SUB:
			if(gDispSub != 9){
				gDispSub += 1;
				gDisplay.Request = DISPLAY_REQUEST_ON;
				gDispMode = LCD_MAIN_FAULT_SUB;
				gDisplay.Lines = TOP_LINE | BTM_LINE;
				pBeep(1, 0);
			}
			else{
				pBeep(3, 0);
			}
			break;

		default:
			pBeep(3, 0);
			break;
	}

}
void pLKey_Down(void)
{
	/*if(gStatus.status > STATUS_INIT)return;
	if(gMotorStatus > MOTOR_OFF)return;*/
	switch(gDispMode)
	{
		case LCD_MAIN_INIT:
		
			switch(gDispSub)
			{
				case LCD_MAIN_STATUS_0:
					gDispSub = LCD_MAIN_STATUS_2;
					gSetMonitor(0x0000, 1);
					break;
				case LCD_MAIN_STATUS_2:
					gDispSub = LCD_MAIN_STATUS_1;
					break;
				default:
					gDispSub = LCD_MAIN_STATUS_0;
					break;
			}
			
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDispMode = LCD_MAIN_INIT;
			gDisplay.Lines = TOP_LINE | BTM_LINE;
			pBeep(1, 0);
			break;
					
		case LCD_MAIN_MENU:
			if(gDispSub == LCD_SUB_MAIN_HEAT)	gDispSub = LCD_SUB_DISP_END - 1;
			else										gDispSub--;
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines |= BTM_LINE;
			pBeep(1, 0);
			break;
					
		case LCD_MAIN_HEAT_SET:
			
			if(gDispSub == LCD_SUB_HEAT_TEMPERATURE)	gDispSub = LCD_SUB_HEAT_END-1;
			else									gDispSub--;
			

			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines |= BTM_LINE;
			pBeep(1, 0);
			break;
/*		case LCD_MAIN_HEAT_SET_WARMUP:
			if(gDispSub == LCD_SUB_WARMUP_TIME)	gDispSub = LCD_SUB_WARMUP_END-1;
			else									gDispSub--;
			

			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines |= BTM_LINE;
			pBeep(1, 0);
			break;*/
		case LCD_MAIN_INV_SET:
			if(gDispSub == LCD_SUB_INV_FREQ)	gDispSub = LCD_SUB_INV_END - 1;
			else									gDispSub--;
			

			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines |= BTM_LINE;
			pBeep(1, 0);
			break;
		/*case LCD_MAIN_HEAT_SET_WARMUP_SUB:
			if(gDispParameter.prev_sub == 0)
			{
				if(gDispParameter.Value > gDispParameter.Min){
					if(gDispParameter.Value > gDispParameter.Max){
						gDispParameter.Value = gDispParameter.Max;
					}
					else gDispParameter.Value--;
						
					gDisplay.Request = DISPLAY_REQUEST_ON;
					gDisplay.Lines |= BTM_LINE;
				}
				if((gKey.RepeatTimer) && (!gKey.RepeatKeyIn))	pBeep(1,0);
			}
			else
			{
				if(gDispParameter.Value >= (gDispParameter.Min + 100)){
					gDispParameter.Value -= 100;
					
					if(gDispParameter.Value > gDispParameter.Max){
						gDispParameter.Value = gDispParameter.Max;
					}
					gDisplay.Request = DISPLAY_REQUEST_ON;
					gDisplay.Lines |= BTM_LINE;
				}
				if((gKey.RepeatTimer) && (!gKey.RepeatKeyIn))	pBeep(1,0);
			}
			break;*/
		case LCD_MAIN_HEAT_SET_SUB:
			
			//break;
		case LCD_MAIN_HEAT_SET_TOP_SUB:
			if(gDispParameter.Value >= (gDispParameter.Min + 100)){
				gDispParameter.Value -= 100;
				
				if(gDispParameter.Value > gDispParameter.Max){
					gDispParameter.Value = gDispParameter.Max;
				}
				gDisplay.Request = DISPLAY_REQUEST_ON;
				gDisplay.Lines |= BTM_LINE;
			}
			if((gKey.RepeatTimer) && (!gKey.RepeatKeyIn))	pBeep(1,0);
			break;
		
		case LCD_MAIN_INV_SET_SUB:
			if(gDispParameter.Value > gDispParameter.Min){
				gDispParameter.Value--;
				
				if(gDispParameter.Value > gDispParameter.Max){
					gDispParameter.Value = gDispParameter.Max;
				}
				gDisplay.Request = DISPLAY_REQUEST_ON;
				gDisplay.Lines |= BTM_LINE;
			}
			if((gKey.RepeatTimer) && (!gKey.RepeatKeyIn))	pBeep(1,0);
			break;
		case LCD_MAIN_FAULT:
			if(gDispSub == LCD_SUB_FAULT_CLEAR)		gDispSub = LCD_SUB_FAULT_READ;
			else											gDispSub = LCD_SUB_FAULT_CLEAR;
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines |= BTM_LINE;
			pBeep(1, 0);
			break;
		case LCD_MAIN_FAULT_SUB:
			if(gDispSub != 0){
				gDispSub -= 1;
				gDisplay.Request = DISPLAY_REQUEST_ON;
				gDispMode = LCD_MAIN_FAULT_SUB;
				gDisplay.Lines = TOP_LINE | BTM_LINE;
				pBeep(1, 0);
			}
			else{
				pBeep(3, 0);
			}
			break;
		default:
			pBeep(3, 0);
			break;
	}
}
void setDispParameter(_Para *para)
{
	gDispParameter.prev_sub = para->prev_sub;
	gDispParameter.Value = para->Value;
	gDispParameter.Max = para->Max;
	gDispParameter.Min = para->Min;
}
void getDispParameter(_Para *para)
{
	para->prev_sub = gDispParameter.prev_sub;
	para->Value = gDispParameter.Value;
	para->Max = gDispParameter.Max;
	para->Min = gDispParameter.Min;
}
