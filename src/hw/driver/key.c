#include "global.h"
#include "key.h"
#include "eeprom.h"
#include "led.h"
#include "modbus.h"
#include "gpio.h"
#include "ssr.h"
#define REVERSE_KEY_IN 	((*(RB_BTF*)&PCIN).p1)
#define CONVAYOR_KEY_IN ((*(RB_BTF*)&PCIN).p2)
#define EMG_KEY_IN 		((*(RB_BTF*)&PCIN).p3)
#define MOTOR_KEY_IN	((*(RB_BTF*)&PDIN).p0)
#define HEATER_KEY_IN	((*(RB_BTF*)&PDIN).p1)
#define MOTOR2_KEY_IN	((*(RB_BTF*)&P6IN).p5)
#define AM_KEY_IN		((*(RB_BTF*)&P6IN).p1)


extern void pBeep(uint8_t Cnt, uint8_t sl);
extern uint8_t adcCheck(void);
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

const key_tbl_t key_TBL[7] = {
	{&PCIN, 0x02, 0x00}, 	//reverse
	{&PCIN, 0x04, 0x00},	//convayor
	{&PCIN, 0x08, 0x08},	//emg
	{&PDIN, 0x01, 0x00},	//motor
	{&PDIN, 0x02, 0x00},	//heater
	{&P6IN, 0x20, 0x00},	//motor2
	{&P6IN, 0x02, 0x00},	//AM
};
void keyInit(void)
{
	keyObjCreate(&btn_reverse, _DEF_REVERSE_KEY, 5000);
	keyObjCreate(&btn_convayor, _DEF_CONVAYOR_KEY, 100);
	keyObjCreate(&btn_emg, _DEF_EMG_KEY, 100);
	keyObjCreate(&btn_motor, _DEF_MOTOR_KEY, 100);
	keyObjCreate(&btn_heater, _DEF_HEATER_KEY, 1500);
	keyObjCreate(&btn_motor2, _DEF_MOTOR2_KEY, 100);
	keyObjCreate(&btn_am, _DEF_AM_KEY, 100);
	PCIE |= 0x0E;
	PDIE |= 0x03;
	P6IE |= 0x22;
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
void pAMKeyScan(void)
{
	static uint8_t RevKeyOn = 0;
	uint8_t pKeyIn = 0;
	
	if(keyObjGetPressed(&btn_am, 60) == true)
	{
		if(btn_am.state == 2)
		{
			gSwitch.AutoPreTime = millis();
			btn_am.pre_time = millis();
			if(RevKeyOn == 0)
			{
				RevKeyOn = 1;
				if(gInverterStatus.bit.stop == 0)
				{
					pKeyIn = 0x01;
				}
				if(gStatus.status > STATUS_INIT)
				{
					pKeyIn |= 0x02;
				}
				if(SSR3_OUT == 1)
				{
					pKeyIn |= 0x08;
				}
				if(SSR4_OUT == 1)
				{
					pKeyIn |= 0x10;
				}
				if(pKeyIn == 0x00)
				{
					// Inverter On
					gInverterStatus.bit.stop = 0;
					gModBus.no = MODBUS_INVERTER;
					gModBus.rxMode = gModBus.mode;
					if((gModBus.status > MODBUS_READY) && (gModBus.rxMode == 0xFF)) gModBus.rxMode = gModBus.mode;
					gModBus.mode = MODBUS_MODE_COMMAND;
					gSetCommand(MODBUS_INVERTER, 0x0005, 0x0002, 1);
					ledOn(LED_AM);
					ledOn(LED_CONVAYOR);
					
					// Heater On
					gStatus.status = STATUS_IDLE;
					ssrOn(MODBUS_SSR_TOP);
					ssrOn(MODBUS_SSR_BOTTOM);
					
					// Motor1 On
					ssrControl(3, 1);
					ledOn(_DEF_LED4);
					
					// Motor2 On
					ssrControl(4, 1);
					ledOn(LED_MOTOR2);
				}
				else
				{
					if(pKeyIn & 0x01)
					{
						gModBus.no = MODBUS_INVERTER;
						if((gModBus.status > MODBUS_READY) && (gModBus.rxMode == 0xFF)) gModBus.rxMode = gModBus.mode;
						gModBus.mode = MODBUS_MODE_COMMAND;
						gSetCommand(MODBUS_INVERTER, 0x0005, 0x0001, 1);
						ledOff(_DEF_LED3);
					}
					if(pKeyIn & 0x02)
					{
						ssrOff(MODBUS_SSR_TOP);
						ssrOff(MODBUS_SSR_BOTTOM);
						gStatus.status = STATUS_INIT;
					}
					if(pKeyIn & 0x08)
					{
						ssrControl(3, 0);
						ledOff(_DEF_LED4);
					}
					if(pKeyIn & 0x10)
					{
						ssrControl(4, 0);
						ledOff(LED_MOTOR2);
					}
					ledOff(_DEF_LED2);
				}
			}
		}
	}
	else
	{
		RevKeyOn = 0;
	}
}
void pReverseKeyScan(void)
{
	static uint8_t RevKeyOn = 0;
	uint8_t KeyIn = 0;
	uint32_t pre_time;
	
	if(keyObjGetPressed(&btn_reverse, 60) == true)
	{
		if(btn_reverse.state == 2)
		{
			if(millis() - btn_convayor.pre_time < 1000)
			{
				pBeep(3, 0);
				return;
			}
			btn_reverse.pre_time = millis();
			if(RevKeyOn == 0)
			{
				RevKeyOn = 1;
				if(gInverterStatus.bit.fowardRun == 1)
				{
					pBeep(3, 0);
					return;
				}
				KeyIn = 1;
				if(gInverterStatus.bit.stop == 1){
					/*pre_time = millis();
					while(gModBus.status == MODBUS_RECEIVE)
					{
						if(millis() - pre_time > 20)break;
						clear_WDT();
					}*/
					gModBus.no = MODBUS_INVERTER;
					gModBus.mode = MODBUS_MODE_COMMAND;
					//gModBus.p500msFlag = 1;
					gSetCommand(MODBUS_INVERTER, 0x0005, 0x0004, 1);
					/*pre_time = millis();
					while(gModBus.status == MODBUS_RECEIVE)
					{
						if(millis() - pre_time > 20)break;
						clear_WDT();
					}*/
					modbusTransmit();
					ledOn(_DEF_LED2);
				}
				else
				{
					
				}
			}
		}
	}
	else
	{
		RevKeyOn = 0;
		if(gInverterStatus.bit.reverseRun == 1)
		{
			gSetCommand(MODBUS_INVERTER, 0x0005, 0x0001, 1);
			ledOff(_DEF_LED2);
			
		}
		else
		{
			ledOff(_DEF_LED2);
		}
		
	}
	
		
	if(KeyIn == 0)return;
	
}
void pConvayorKeyScan(void)
{
	static uint8_t IncKeyOn = 0;
	uint8_t check;
	if(keyObjGetPressed(&btn_convayor, 60) == true)
	{
		if(btn_convayor.state == 2)
		{
			gSwitch.InvPreTime = millis();
			btn_convayor.pre_time = millis();
			if(IncKeyOn == 0)
			{
				IncKeyOn = 1;
				if(gInverterStatus.bit.reverseRun == 1)
				{
					pBeep(3, 0);
					return;
				}
				if((gInverterStatus.bit.stop == 1) || (gInverterStatus.All == 0))
				{
					ledOn(_DEF_LED3);
					gSetCommand(MODBUS_INVERTER, 0x0005, 0x0002, 1);
				}
				else
				{
					check = gGetCommand(MODBUS_INVERTER);
					if(check == 0){
						ledOff(_DEF_LED3);
						gSetCommand(MODBUS_INVERTER, 0x0005, 0x0001, 1);
					}
					else gSetCommand(MODBUS_INVERTER, 0x0000, 0x0000, false);
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
			btn_emg.pre_time = millis();
			if(StartKeyContinue == 0)
			{
				StartKeyContinue = 1;
				gError.bit.EmergencyOn = 1;
				ssrControl(1, 0);
				ssrControl(2, 0);
				ssrControl(3, 0);
				ssrControl(4, 0);
				ledOff(_DEF_LED4);
				ledOff(_DEF_LED2);
				if(gInverterStatus.bit.stop == 0)
				{
					ledOff(_DEF_LED3);
					gSetCommand(MODBUS_INVERTER, 0x0005, 1, 1);
				}
				if(gStatus.status > STATUS_INIT){
					gStatus.status = STATUS_INIT;
					gStatus.Start = 0;
					SSR_TOP.output = 0;
					SSR_BOTTOM.output = 0;
					SSR_TOP.outVoltage = 0;
					SSR_BOTTOM.outVoltage = 0;
					//TODO inverter emergency out on
					
					/*
					gSetCommand(MODBUS_SSR_TOP, 0x0002, 0, 1);
					gSetCommand(MODBUS_SSR_BOTTOM, 0x0002, 0, 1);
					gModBus.no = MODBUS_INVERTER;
					gModBus.mode = MODBUS_MODE_COMMAND;*/
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
				ssrControl(1, 1);
				ssrControl(2, 1);
				delay(500);
				gReleaseError(MODBUS_INVERTER, MODBUS_MODE_COMMAND);
				if(modbusConnectCheck(MODBUS_SSR_TOP) == 1);
				if(modbusConnectCheck(MODBUS_SSR_BOTTOM) == 1);
				ledOff(LED_CONVAYOR);
				ledOff(LED_MOTOR1);
				ledOff(LED_MOTOR2);
				gSetCommand(MODBUS_INVERTER, 0x0004, gIParameter.F_Freq, 1);
			}
		}
		gError.bit.EmergencyOn = 0;
	}
}
void gMotorKeyScan(void)
{
	static uint8_t ModeKeyOn = 0;
	if(keyObjGetPressed(&btn_motor, 60) == true)
	{
		if(btn_motor.state == 2){
			btn_motor.pre_time = millis();
			if(ModeKeyOn == 0)
			{
				ModeKeyOn = 1;
				if(SSR3_OUT == 0)
				{
					ssrControl(3, 1);
					ledOn(LED_MOTOR1);
				}
				else
				{
					ssrControl(3, 0);
					ledOff(LED_MOTOR1);
				}
			}
		}
	}
	else
	{
		ModeKeyOn = 0;
	}
}
void gMotor2KeyScan(void)
{
	static uint8_t ModeKeyOn = 0;
	if(keyObjGetPressed(&btn_motor2, 60) == true)
	{
		if(btn_motor2.state == 2){
			btn_motor2.pre_time = millis();
			if(ModeKeyOn == 0)
			{
				ModeKeyOn = 1;
				if(SSR4_OUT == 0)
				{
					ssrControl(4, 1);
					ledOn(LED_MOTOR2);
				}
				else
				{
					ssrControl(4, 0);
					ledOff(LED_MOTOR2);
				}
			}
		}
	}
	else
	{
		ModeKeyOn = 0;
	}
}
void gHeaterKeyScan(void)
{
	static uint8_t HeaterKeyOn = 0;
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
						gStatus.status = STATUS_IDLE;
						SSR_TOP.PID.integral = 0;
						SSR_TOP.PID.prev_error = 0;
						SSR_BOTTOM.PID.integral = 0;
						SSR_BOTTOM.PID.prev_error = 0;
						
						gModBus.no = MODBUS_SSR_TOP;
						if((gModBus.status > MODBUS_READY) && (gModBus.rxMode == 0xFF)) gModBus.rxMode = gModBus.mode;
						gModBus.mode = MODBUS_MODE_COMMAND;
						gSetCommand(MODBUS_SSR_TOP, 0x0002, 1, 1);
						gSetCommand(MODBUS_SSR_BOTTOM, 0x0002, 1, 1);
					}
					else
					{
						gStatus.status = STATUS_INIT;
						gModBus.no = MODBUS_SSR_TOP;
						if((gModBus.status > MODBUS_READY) && (gModBus.rxMode == 0xFF)) gModBus.rxMode = gModBus.mode;
						gModBus.mode = MODBUS_MODE_COMMAND;
						gSetCommand(MODBUS_SSR_TOP, 0x0002, 0, 1);
						gSetCommand(MODBUS_SSR_BOTTOM, 0x0002, 0, 1);
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
	gMotorKeyScan();
	gMotor2KeyScan();
	gHeaterKeyScan();
	gEmgKeyScan();
	pReverseKeyScan();
	pAMKeyScan();
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
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines = TOP_LINE | BTM_LINE;
			break;
		case LCD_MAIN_HEAT_SET:
		case LCD_MAIN_INV_SET:
			gDispMode = LCD_MAIN_MENU;
			gDispSub = LCD_MAIN_STATUS_0;
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
	uint8_t i;
	uint16_t *Ptr16;
	
	if((gError.bit.inverterCommError == 1) || (gError.bit.ssr1CommError == 1) || (gError.bit.ssr2CommError == 1))
	{
		if(gError.bit.inverterCommError == 1)
		{
			if(modbusConnectCheck(MODBUS_INVERTER) == 1)
			{
				pBeep(2, 0);
				gReleaseError(MODBUS_INVERTER, 0);
				gError.bit.inverterCommError = 0;
			}
			else pBeep(3,0);
		}
		if(gError.bit.ssr1CommError == 1)
		{
			if(modbusConnectCheck(MODBUS_SSR_TOP) == 1)
			{
				pBeep(2, 0);
				gReleaseError(MODBUS_SSR_TOP, 0);
				gError.bit.ssr1CommError = 0;
			}
			else pBeep(3,0);
		}
		if(gError.bit.ssr2CommError == 1)
		{
			if(modbusConnectCheck(MODBUS_SSR_BOTTOM) == 1)
			{
				pBeep(2, 0);
				gReleaseError(MODBUS_SSR_BOTTOM, 0);
				gError.bit.ssr2CommError = 0;
			}
			else pBeep(3,0);
		}
		return;
	}
	
	if(SSR_TOP.error)
	{
		
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
				gDispSub = LCD_SUB_HEAT_TOP;
				break;

			case LCD_SUB_MAIN_INV:
				gDispMode = LCD_MAIN_INV_SET;
				gDispSub = LCD_SUB_INV_F_FREQ;

				gDisplay.Request = DISPLAY_REQUEST_ON;
				gDisplay.Lines = TOP_LINE | BTM_LINE;
				
				break;
		

			case LCD_SUB_MAIN_INITIALIZE:
				gDispMode = LCD_MAIN_INITIALIZE;
			
				break;

			case LCD_SUB_MAIN_FAULTSCAN:
				gDispMode = LCD_MAIN_FAULT;
				gDispSub = LCD_SUB_HEAT_TOP;
				break;

		}
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
	else if(gDispMode == LCD_MAIN_HEAT_SET){
		pBeep(1,0);
		gDispMode = LCD_MAIN_HEAT_SET_SUB;

		
		Ptr16 = (uint16_t *)&gParameter;
		Ptr16 += gDispSub;
 

		
		gDispParameter.Value = *Ptr16;
				//@150423 by Bang
		gDispParameter.Min = EEP_HEAT_ParaTBL[gDispSub][0];
		gDispParameter.Max = EEP_HEAT_ParaTBL[gDispSub][1];
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDispMode = LCD_MAIN_HEAT_SET_SUB;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
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
	else if(gDispMode == LCD_MAIN_HEAT_SET_SUB){
		pBeep(1,0);
		/* Parameter Save */
		Ptr16 = (uint16_t *)&gParameter;
		Ptr16 += gDispSub;
		*Ptr16 = gDispParameter.Value;
		
		pn_EEP_WriteBytes(0x10 + gDispSub *2, (uint8_t *)&gDispParameter.Value,/* LCD_SUB_INV_END*/2);
		if(gDispSub == LCD_SUB_HEAT_TOP)
		{
			gStatus.Heater_Top.Comp = gParameter.TopTemp;
		}
		else
		{
			gStatus.Heater_Bottom.Comp = gParameter.BottomTemp;
		}
//		pn_Load_Parameters();
		
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDispMode = LCD_MAIN_HEAT_SET;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
	else if(gDispMode == LCD_MAIN_INV_SET_SUB){
		pBeep(1,0);
		/* Parameter Save */
		Ptr16 = (uint16_t *)&gIParameter;
		Ptr16 += gDispSub;
		*Ptr16 = gDispParameter.Value;
		
		pn_EEP_WriteBytes(0x30 + gDispSub *2, (uint8_t *)&gDispParameter.Value,/* LCD_SUB_INV_END*/2);
		if(gDispSub == LCD_SUB_INV_F_FREQ)
		{
			gSetCommand(MODBUS_INVERTER, 0x0004, gDispParameter.Value, 1);
		}
		else
		{
			if(gInverterStatus.bit.reverseRun == 1)gSetCommand(MODBUS_INVERTER, 0x0004, gDispParameter.Value, 1);
		}
//		pn_Load_Parameters();
		
		gDisplay.Request = DISPLAY_REQUEST_ON;
		gDispMode = LCD_MAIN_INV_SET;
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
	else if(gDispMode == LCD_MAIN_INITIALIZE){
		pBeep(2, 0);
		/* Doortype Save 150428 by Bang */
		
		pBeep(2, 0);
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
				for(i=0;i!=10;i++){
					gMonitorError[i] = 0;
					pn_EepromWriteByte(0x90+i,gMonitorError[i]);
				}
				pBeep(1, 0);
				gDisplay.Request = DISPLAY_REQUEST_ON;
				gDispMode = LCD_MAIN_MENU;
				gDispSub = LCD_SUB_MAIN_FAULTSCAN;
				break;
		
		}
		gDisplay.Lines = TOP_LINE | BTM_LINE;
	}
}
void pLKey_Up(void)
{
	switch(gDispMode)
	{
		case LCD_MAIN_INIT:
			switch(gDispSub)
			{
				case LCD_MAIN_STATUS_0:
					gSetMonitor(MODBUS_INVERTER, INV_MONITOR_ADDR, 1);
					gDispSub++;
					break;
				case LCD_MAIN_STATUS_1:
					gSetMonitor(MODBUS_INVERTER, INV_MONITOR_ADDR, 0);
					gSetMonitor(MODBUS_SSR_TOP, 0x0000, 1);
					gSetMonitor(MODBUS_SSR_BOTTOM, 0x0000, 1);
					gDispSub++;
					break;
				default:
					gDispSub = LCD_MAIN_STATUS_0;
					gSetMonitor(MODBUS_INVERTER, INV_MONITOR_ADDR, 0);
					gSetMonitor(MODBUS_SSR_TOP, 0x0000, 1);
					gSetMonitor(MODBUS_SSR_BOTTOM, 0x0000, 1);
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
			
			if(++gDispSub >= LCD_SUB_HEAT_END)		gDispSub = LCD_SUB_HEAT_TOP;
			
			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines |= BTM_LINE;
			pBeep(1,0);
			break;
		case LCD_MAIN_INV_SET:
			if(++gDispSub >= LCD_SUB_INV_END)		gDispSub = LCD_SUB_INV_F_FREQ;
			
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
		case LCD_MAIN_INV_SET_SUB:
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
	switch(gDispMode)
	{
		case LCD_MAIN_INIT:
		
			switch(gDispSub)
			{
				case LCD_MAIN_STATUS_0:
					gDispSub = LCD_MAIN_STATUS_2;
					gSetMonitor(MODBUS_SSR_TOP, 0x0000, 1);
					gSetMonitor(MODBUS_SSR_BOTTOM, 0x0000, 1);
					break;
				case LCD_MAIN_STATUS_2:
					gDispSub = LCD_MAIN_STATUS_1;
					gSetMonitor(MODBUS_INVERTER, INV_MONITOR_ADDR, 1);
					gSetMonitor(MODBUS_SSR_TOP, 0x0004, 0);
					gSetMonitor(MODBUS_SSR_BOTTOM, 0x0004, 0);
					break;
				default:
					gDispSub = LCD_MAIN_STATUS_0;
					gSetMonitor(MODBUS_INVERTER, INV_MONITOR_ADDR, 0);
					gSetMonitor(MODBUS_SSR_TOP, 0x0000, 1);
					gSetMonitor(MODBUS_SSR_BOTTOM, 0x0000, 1);
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
			
			if(gDispSub == LCD_SUB_HEAT_TOP)	gDispSub = LCD_SUB_HEAT_END-1;
			else									gDispSub--;
			

			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines |= BTM_LINE;
			pBeep(1, 0);
			break;
		case LCD_MAIN_INV_SET:
			if(gDispSub == LCD_SUB_INV_F_FREQ)	gDispSub = LCD_SUB_INV_END - 1;
			else									gDispSub--;
			

			gDisplay.Request = DISPLAY_REQUEST_ON;
			gDisplay.Lines |= BTM_LINE;
			pBeep(1, 0);
			break;
		case LCD_MAIN_HEAT_SET_SUB:
			
			//break;
		case LCD_MAIN_INV_SET_SUB:
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
