#include "global.h"
#include "display.h"
#include "strings.h"


static uint8_t gFndText[4];
enum{
	FORMAT_xxx,
	FORMAT_xxxx, //@150508 by Bang
	FORMAT_xPxx,
	FORMAT_xxPx,

	FORMAT10_xPxx,
	FORMAT10_xxPxx,
	FORMAT__xxx //@150820
};
static void pFnd_DisplayByte_Format(uint8_t Format,uint8_t Val,char *Ptr)
{
	unsigned short Value = Val;
	unsigned char Data;
	
	switch(Format)
	{
		case FORMAT_xxx:

			*Ptr++ = 0x00;
			Data = Value/100;
			if(Data == 0) *Ptr++ = 0x00;
			else *Ptr++ = Dec2Bcd[Data];
			Value %= 100;
			if(Data == 0){
				Data = Value/10;
				if(Data == 0) *Ptr++ = 0x00;
				else *Ptr++ = Dec2Bcd[Data];
			}
			else{
				Data = Value/10;
				*Ptr++ = Dec2Bcd[Data];
			}
			Value %= 10;
			*Ptr++ = Dec2Bcd[Value];
			
			break;
		case FORMAT_xxxx:
			
			
			Data = Value/1000;
			if(Data == 0) *Ptr++ = 0x00;
			else *Ptr++ = Dec2Bcd[Data];
			Value %= 1000;
			if(Data == 0){
				Data = Value/100;
				if(Data == 0) *Ptr++ = 0x00;
				else *Ptr++ = Dec2Bcd[Data];
			}
			else{
				Data = Value/100;
				*Ptr++ = Dec2Bcd[Data];
			}
			Value %= 100;
			if(Data == 0){
				Data = Value/10;
				if(Data == 0) *Ptr++ = 0x00;
				else *Ptr++ = Dec2Bcd[Data];
			}
			else{
				Data = Value/10;
				*Ptr++ = Dec2Bcd[Data];
			}
			Value %= 10;
			*Ptr++ = Dec2Bcd[Value];
			
			break;
		case FORMAT_xPxx:
			*Ptr++ = Value/100 + 0x30;
			*Ptr++ = '.';
			Value %= 100;
			*Ptr++ = Value/10 + 0x30;
			Value %= 10;
			*Ptr++ = Value + 0x30;
			break;
		case FORMAT_xxPx:
			Data = Value/100;
			if(Data == 0) *Ptr++ = 0x00;
			else *Ptr++ = Dec2Bcd[Data];
			Value %= 100;
			Data = Value/10;
			*Ptr = Dec2Bcd[Data];
			*Ptr++ |= 0x80;
			Value %= 10;
			Data = Value;
			*Ptr++ = Dec2Bcd[Data]; 
			break;

		case FORMAT10_xPxx:
			Value = Val * 10;
			*Ptr++ = Value/100 + 0x30;
			*Ptr++ = '.';
			Value %= 100;
			*Ptr++ = Value/10 + 0x30;
			Value %= 10;
			*Ptr++ = Value + 0x30;
			break;

		case FORMAT10_xxPxx:
			Value = Val * 10;
			Data = Value/1000;
			if(Data == 0) *Ptr++ = 0x00;
			else *Ptr++ = Dec2Bcd[Data];
			Value %= 1000;
			*Ptr++ = Dec2Bcd[Value/100] | 0x80;
			
			Value %= 100;
			*Ptr++ = Dec2Bcd[Value/10];
			Value %= 10;
			*Ptr++ = Dec2Bcd[Value];

			break;
	}
}
static void pFnd_DisplayInt_Format(int Val)
{
	unsigned int Value;
	
	
	
	unsigned char i;
	signed char Dot2;
	unsigned int check = 10, su = 1;
	if(Val>=0) Value = Val;
	else Value = (Val ^  0xffffffff)+1;
	while(Value / check)
    {
		su++;
		check *= 10;
    }
	Dot2 = 3 - su;
	
	if(Val >= 0){
		for(i=0;i!=4;i++){
			if(Dot2 >= i) gFndText[i] = S_BLK;
			else {
				Value %= check;
				check /= 10;
				gFndText[i] = Dec2Bcd[Value/check];
			}
		}
		if((signed char)(su - 4) >= 0)	gFndText[su - 4] |= 0x80;
	}
	else {
		gFndText[0] = S_DS;
		for(i=1;i!=4;i++){
			if(Dot2 >= i) gFndText[i] = S_BLK;
			else {
				Value %= check;
				check /= 10;
				gFndText[i] = Dec2Bcd[Value/check];
			}
		
		}
		if((signed char)(su - 4) >= 0)	gFndText[su - 3] |= 0x80;
	}
}

static void pFnd_DisplayShortDot_Format(uint16_t Val)
{
	uint16_t Value;
	uint8_t i = 0;
	Value = Val;
	gFndText[i++] = 0x00;
	Value %= 1000;
	gFndText[i++] = Dec2Bcd[Value/100];
	
	Value %= 100;
	gFndText[i++] = Dec2Bcd[Value/10] | 0x80;
	Value %= 10;
	gFndText[i++] = Dec2Bcd[Value];
	
}
static void pFnd_LoadString(void)
{
	uint8_t i;
	
	char *Ptr;

	switch(gDispMode)
	{
		case LCD_SYS_INIT:
			
			/*Ptr = (char *)&LCD_RESET1[0];
			for(i=0;i!=4;i++)gFndText[i] = *Ptr++;*/
			gFndText[0] = S_S;
			gFndText[1] = Dec2Bcd[Versions[1] - 0x30] | 0x80;
			gFndText[2] = Dec2Bcd[Versions[3] - 0x30];
			gFndText[3] = Dec2Bcd[Versions[4] - 0x30];
			break;

		case LCD_MAIN_INIT:
			if(System.OverTemp){
				Ptr = (char *)&DISP_MODE_FAULT_OVERTEMP[0];
				for(i=0;i!=4;i++)gFndText[i] = *Ptr++;
			}
		
			else if((System.SetupModeOn) && (System.SetupSaveDisplay)){
				Ptr = (char *)&LCD_SAVEMODE[0];
			}
			else if(System.TestModeOn){
				Ptr = (char *)&LCD_TESTMODE[0];
			}
			else if(System.OverCurrent == 1){
				Ptr = (char *)&ERRMSG_OVERCURRENT[0];
				for(i=0;i!=4;i++)gFndText[i] = *Ptr++;
			}
			
			else if(System.OverVoltage == 1){
				Ptr = (char *)&ERRMSG_OVERVOLTAGE[0];
				for(i=0;i!=4;i++)gFndText[i] = *Ptr++;
			}
			else if(System.LowVoltage == 1){
				Ptr = (char *)&ERRMSG_UNDERVOLTAGE[0];
				for(i=0;i!=4;i++)gFndText[i] = *Ptr++;
			}
/*@090115*/
			else if(System.InputError == 1){
				Ptr = (char *)&ERRMSG_INPUTERROR[0];
				for(i=0;i!=4;i++)gFndText[i] = *Ptr++;
			}
		/*	else if(System.LimitError == 1){
				Ptr = (char *)&ERRMSG_LIMITERROR[0];
				for(i=0;i!=4;i++)gFndText[i] = *Ptr++; 
			}*/
			else if(System.IGBT_FO == 1){
				Ptr = (char *)&ERRMSG_IGBT_FO[0];
				for(i=0;i!=4;i++)gFndText[i] = *Ptr++;
			}
/*@090115*/

			

			else if(System.FaultTimeError){
				if(gFaultTimeErrNo == 1)	Ptr = (char *)&DISP_MODE_FAULT_FTIMEHIGH[0];
				else						Ptr = (char *)&DISP_MODE_FAULT_FTIMELOW[0];
				for(i=0;i!=4;i++)gFndText[i] = *Ptr++;
			}

			else if(System.P24VError){
				Ptr = (char *)&ERRMSG_P24V[0];
				for(i=0;i!=4;i++)gFndText[i] = *Ptr++;
			}
			else{
					//pFnd_DisplayInt_Format(gDispTimer);
#if 1				
				if(gDispTimer > 1){
					if(System.TestModeOn) Ptr = (char *)&LCD_TESTMODE[0];
					else Ptr = (char *)&SCR_L2MODE[gDispSub];
					for(i=0;i!=4;i++){
						gFndText[i] = *Ptr++;
					}
					
				}
				else {
					/* Sub Data */
					
					switch(gDispMode)
					{
						case LCD_MAIN_INIT:
							if(System.OverCurrent == 1) break;
							if(System.OverTemp == 1) break;
							if(System.OverVoltage == 1) break;
							if(System.LowVoltage == 1) break;
							if(System.InputError == 1) break; //@150429 by Bang
							if(System.FaultTimeError == 1) break;
							if(System.P24VError == 1)break;
						
							
							
							
							if(System.TestModeOn){
								gFndText[0] = Dec2Bcd[gParameter.Counter[2]];
								gFndText[1] = Dec2Bcd[gParameter.Counter[3]];
								gFndText[2] = Dec2Bcd[gParameter.Counter[4]];
								gFndText[3] = Dec2Bcd[gParameter.Counter[5]];
								
								break;
							}
						
							{
								unsigned short Value;
								
								switch(gDispSub)
								{
									case LCD_MAIN_STATUS_0: /* System Mode  Auto/Hand,Safty,Loop,Press */
										
										if(System.Mode == 0)gFndText[0] = S_A;
										else if(System.Mode == OPERATION_MODE_SOFT_MANUAL)gFndText[0] = S_H;
										else if(System.Mode == OPERATION_MODE_SOFT_MANUAL2)gFndText[0] = S_H | 0x80;
										else gFndText[0] = 0x74;
										if(fSS_PC7_IN==1)gFndText[1] = S_S;
										else gFndText[1] = S_DS;
										if(System.LoopSensorOn)gFndText[2] = S_L;
										else gFndText[2] = S_DS;
										
										if((gMotorStatus != MOTOR_OFF) && (MotorStatus.Direction == DIR_UP) && (fSS_PD1_IN_SUB == 1))gFndText[3] = S_P;
										else if(fSS_PD1_IN == 1)gFndText[3] = S_P;
										else gFndText[3] = S_DS;
										
										break;
									case LCD_MAIN_STATUS_1:
										Ptr = (int8_t *)&gFndSensor[0];
										for(i=0;i!=4;i++)gFndText[i] = *Ptr++;
										
										break;
									
									case LCD_MAIN_STATUS_2: /* gWork Frequency */
										pFnd_DisplayShortDot_Format(gWorkFreq);
										
										break;
									
									case LCD_MAIN_STATUS_3: /* Current */
										Value = gMonitorCurrent;
										pFnd_DisplayInt_Format(Value);
										
										break;
									
									case LCD_MAIN_STATUS_4: /* Out Level : Voltage */
										pFnd_DisplayByte_Format(FORMAT_xxxx,gAD.Voltage,(char *)&gFndText[0]);
										break;
									
									case LCD_MAIN_STATUS_5: /* Counter */
										pFnd_DisplayInt_Format((int)gDoorCount);
										break;
									case LCD_MAIN_STATUS_6: /* IGBT Temperature */
										if((IntTimer%500)==0)pFnd_DisplayInt_Format(gAD.Temperature);
										break;
								}
								
							}
							break;
					
					}
				}
#endif				
			}
			
			break;
			
		case LCD_MAIN_MENU:
			
			if(gDispTimer != 0)	{
				Ptr = (char *)&MAIN_MENU_TOP[0];
				for(i=0;i!=4;i++)gFndText[i] = *Ptr++;
			}
			else{
				gFndText[0] = S_P;
				gFndText[1] = S_BLK;
				i = gDispSub / 10;
				if(i == 0)gFndText[2] = S_BLK;
				else {
					gFndText[2] = Dec2Bcd[i];
				}
				gFndText[3] = Dec2Bcd[gDispSub%10];
			}
			
			break;
		case LCD_MAIN_FACTORY:
			if(gDispTimer != 0)	{
				Ptr = (char *)&MAIN_FMENU_TOP[0];
				for(i=0;i!=4;i++)gFndText[i] = *Ptr++;
			}
			else{
				gFndText[0] = S_F;
				gFndText[1] = S_P;
				i = gDispSub / 10;
				if(i == 0)gFndText[2] = S_BLK;
				else {
					gFndText[2] = Dec2Bcd[i];
				}
				gFndText[3] = Dec2Bcd[gDispSub%10];
			}
			
			break;
	
		case LCD_MAIN_PARA_SUB:
		case LCD_MAIN_FPARA_SUB:	
				switch(gDispSub)
				{
					case LCD_SUB_PARA_ACC:
					case LCD_SUB_PARA_DEC:
					case LCD_SUB_PARA_AIR:
					case LCD_SUB_PARA_LL1DEC:
						gFndText[0] = 0x00;
						pFnd_DisplayByte_Format(FORMAT_xxPx,gDispParameter.Value,(char *)&gFndText[1]);
						
						break;


					case LCD_SUB_PARA_FAULT:
						Ptr = (char *)&ERRMSG_LIST[gMonitorError[gDispParameter.Value]];
						for(i=0;i!=4;i++)gFndText[i] = *Ptr++;
					
						break;
					default:
						pFnd_DisplayByte_Format(FORMAT_xxx,gDispParameter.Value,(char *)&gFndText[0]);
						
						break;
				}
			
		
		default:
			break;
	}
}
void pLcd_UpdateDisplay(void)
{
    unsigned char i,j;
	static uint32_t pre_timer = 0;
	if(IntTimer != pre_timer)
	{
		pre_timer = IntTimer;
		if((IntTimer%2) == 0){
			
			pFnd_LoadString();
			j = (IntTimer / 2) %4;
	/*		
			PORT_FND_SEL1 = 0;
			PORT_FND_SEL2 = 0;
			PORT_FND_SEL3 = 0;
			PORT_FND_SEL4 = 0;
			PORT_FND_LATCH = 0;*/
			
			for (i = 0; i < 8; i++)
			{
				
				//   digitalWrite(dataPin, !!(val & (1 << i)));
				
	/*			PORT_FND_DATA = !!(gFndText[j] & (1 << (7 - i)));
				
				PORT_FND_CLKPIN = 1;
				PORT_FND_CLKPIN = 0;*/
			} 
//			PORT_FND_LATCH = 1;
				
/*			switch(j){
				case 0:
					PORT_FND_SEL1 = 1;
				break;
		
				case 1:
					PORT_FND_SEL2 = 1;
				break;
		
				case 2:
					PORT_FND_SEL3 = 1;
				break;
		
				case 3:
					PORT_FND_SEL4 = 1;
				break;
		
			}*/
		}
	}
}

void pLcd_UpdateDisplay2(void)
{
	if((IntTimer%100) == 0)pFnd_LoadString();	
}
