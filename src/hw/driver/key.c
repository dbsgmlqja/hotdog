#include "global.h"
#include "key.h"
#include "eeprom.h"
#include "led.h"
#define MODE_KEY_IN 	((*(RB_BTF*)&P6IN).p4)
#define DEC_KEY_IN ((*(RB_BTF*)&P6IN).p5)
#define INC_KEY_IN ((*(RB_BTF*)&PCIN).p0)
#define START_KEY_IN	((*(RB_BTF*)&P5IN).p1)

extern void pBeep(uint8_t Cnt, uint8_t sl);
//extern uint16_t EEP_ParaTBL[][3];

extern uint16_t EEP_ParaTBL[][6][3];
typedef struct
{
  volatile uint8_t *port;
  uint8_t      pin;
  uint8_t on_state;
} key_tbl_t;
key_obj_t btn_mode;
key_obj_t btn_inc;
key_obj_t btn_dec;
key_obj_t btn_start_stop;

const key_tbl_t key_TBL[4] = {
	{&P6IN, 0x10, 0x00}, 	//mode
	{&P6IN, 0x20, 0x00},	//dec
	{&PCIN, 0x01, 0x00},	//inc
	{&P5IN, 0x02, 0x00},	//start_stop
};
void keyInit(void)
{
	keyObjCreate(&btn_mode, _DEF_MODE_KEY, 5000);
	keyObjCreate(&btn_dec, _DEF_DEC_KEY, 100);
	keyObjCreate(&btn_inc, _DEF_INC_KEY, 100);
	keyObjCreate(&btn_start_stop, _DEF_START_KEY, 1500);
	PCIE |= 0x01;
	P6IE |= 0x03;
	P5IE |= 0x02;
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
void pDecKeyScan(void)
{
	static uint8_t DecKeyOn = 0;
	uint8_t KeyIn = 0;
	if(keyObjGetPressed(&btn_dec, 60) == true)
	{
		if(btn_dec.state > 1)
		{
			if(DecKeyOn == 0)
			{
				DecKeyOn = 1;
				KeyIn = 1;
			}
		}
	}
	else
	{
		DecKeyOn = 0;
	}
	
		
	if(KeyIn == 0)return;
	switch(gStatus.Mode)
	{
		case 0:
			if(gStatus.OpTime > gDispParameter.Min)gStatus.OpTime--;
			break;
		case 1:
			if(gStatus.Comp1 > gDispParameter.Min)gStatus.Comp1 -= 100;
			break;
		case 2:
			if(gStatus.Comp2 > gDispParameter.Min)gStatus.Comp2 -= 100;
			break;
		case 3:
			if(gRoast_No > 0)gRoast_No--;
			else gRoast_No = 5;
			pn_EEP_Write_Parameter();
			pn_Load_Parameters();
			break;
	}
	gEEP_save = 1;
	gEEP_time = millis();
}
void pIncKeyScan(void)
{
	static uint8_t IncKeyOn = 0;
	if(keyObjGetPressed(&btn_inc, 60) == true)
	{
		if(btn_inc.state == 2)
		{
			if(IncKeyOn == 0)
			{
				IncKeyOn = 1;
				switch(gStatus.Mode)
				{
					case 0:
						if(gStatus.OpTime < gDispParameter.Max)gStatus.OpTime++;
						break;
					case 1:
						if(gStatus.Comp1 < gDispParameter.Max)gStatus.Comp1 += 100;
						break;
					case 2:
						if(gStatus.Comp2 < gDispParameter.Max)gStatus.Comp2 += 100;
						break;
					case 3:
						if(gRoast_No < 5)gRoast_No++;
						else gRoast_No = 0;
						pn_EEP_Read_Parameters();
						pn_Load_Parameters();
						break;
				}
			}
			gEEP_save = 1;
			gEEP_time = millis();
		}
	}
	else
	{
		IncKeyOn = 0;
	}
}
void gStartKeyScan(void)
{
	static uint8_t StartKeyContinue = 0;
	if(keyObjGetPressed(&btn_start_stop, 60) == true)
	{
		if(btn_start_stop.state > 1)
		{
			if(StartKeyContinue == 0)
			{
				StartKeyContinue = 1;
				if(gStatus.status > STATUS_IDLE){
					if(gStatus.Start == 0)
					{
						gStatus.Start = 1;
						pBeep(1, 1);
					}
					else{
						gStatus.Start = 0;
						pBeep(3, 0);
					}
				}
				else{
					pBeep(5, 0);
				}
			}
		}
	}
	else
	{
		if(btn_start_stop.state == 0)StartKeyContinue = 0;
	}
}
void gModeKeyScan(void)
{
	uint16_t *Ptr;
	uint8_t index;
	static uint8_t ModeKeyOn = 0;
	if(keyObjGetPressed(&btn_mode, 60) == true)
	{
		if(ModeKeyOn == 0)
		{
			if(btn_mode.state == 2){
				if(gStatus.Mode < 2)gStatus.Mode++;
				else       gStatus.Mode = 0;
				ModeKeyOn = 1;
				Ptr = (uint16_t *)&gParameter[gRoast_No];
				Ptr += gStatus.Mode;
				index = gStatus.Mode;
				gDispParameter.Value = *Ptr;
				gDispParameter.Min = EEP_ParaTBL[gRoast_No][index][0];
				gDispParameter.Max = EEP_ParaTBL[gRoast_No][index][1];
			}
		}
		else{
			if(millis() - btn_mode.pre_time > 3000){
				gStatus.Mode = 3;
				btn_mode.pre_time = millis();
			}
		}
	}
	else
	{
		ModeKeyOn = 0;
	}
}
void gKeyScan(void)
{
//
}
void gModeKeyReleaseCheck(void)
{
	uint32_t pre_time = millis();
	uint32_t compare,value;
	
	compare = pre_time - btn_mode.pre_time;
	value = compare;
	compare = pre_time - btn_inc.pre_time;
	if(value > compare)value = compare;
	compare = pre_time - btn_dec.pre_time;
	if(value > compare)value = compare;
	
	if(value > 5000){
		gStatus.Mode = 0;
		gDispParameter.Value = gStatus.OpTime;
		gDispParameter.Min = EEP_ParaTBL[gRoast_No][0][0];
		gDispParameter.Max = EEP_ParaTBL[gRoast_No][0][1];
	}
}
static uint8_t var;
uint8_t gAllKeyReleaseCheck(void)
{
	uint32_t pre_time = millis();
	uint32_t compare,value;
	uint8_t ret = 0;
	compare = pre_time - btn_mode.pre_time;
	value = compare;
	compare = pre_time - btn_inc.pre_time;
	if(value > compare)value = compare;
	compare = pre_time - btn_dec.pre_time;
	if(value > compare)value = compare;
	compare = pre_time - btn_mode.pre_time;
	if(value > compare)value = compare;
	if(value > 2500){
		ret = 1;
	}
	var = ret;
	return ret;
}
