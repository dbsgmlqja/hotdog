#include "global.h"
#include "display.h"
#include "LCD_CTRL.h"
#include "strings.h"
#include "uart.h"
#include "FlashOption.h"

enum{
	FORMAT_xxx,
	FORMAT_xxxx, //@150508 by Bang
	FORMAT_xPxx,
	FORMAT_xxPx,

	FORMAT10_xPxx,
	FORMAT10_xxPxx,
	FORMAT10_xxxPxx,
	FORMAT10_xxxPx,

	FORMAT_xxxxx,
	FORMAT_xxxxxx
};
uint8_t getDigitPosition(uint16_t data)
{
	uint8_t ret = 0;
	int8_t i;
	for(i=15;i>=0;i--)
	{
		if(data & (1 << i))return (i+1);
	}
	return ret;
}
void pLCD_DisplayInt_Format(uint8_t Format, uint32_t Val, char *Ptr)
{
	uint32_t Value = Val;
	uint32_t Data;
	uint8_t IsZero = 1;
	switch(Format)
	{
		case FORMAT_xxxxx:
			
			Data = Value/10000;
			if(Data){
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else *Ptr++ = ' ';
			Value %= 10000;
			Data = Value/1000;
			if(Data)
			{
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else
			{
				if(IsZero)
				{
					*Ptr++ = ' ';
				}
				else
				{
					*Ptr++ = '0';
				}
			}
			Value %= 1000;
			Data = Value/100;
			if(Data)
			{
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else
			{
				if(IsZero)
				{
					*Ptr++ = ' ';
				}
				else
				{
					*Ptr++ = '0';
				}
			}
			*Ptr++ = Data;
			Value %= 100;
			Data = Value/10;
			if(Data)
			{
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else
			{
				if(IsZero)
				{
					*Ptr++ = ' ';
				}
				else
				{
					*Ptr++ = '0';
				}
			}
			Value %= 10;
			*Ptr++ = Value + 0x30;
		break;
		default:
			Data = Value/100000;
			if(Data)
			{
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else
			{
				*Ptr++ = ' ';
			}
			Value %= 100000;
			Data = Value/10000;
			if(Data)
			{
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else
			{
				if(IsZero)
				{
					*Ptr++ = ' ';
				}
				else
				{
					*Ptr++ = '0';
				}
			}
			Value %= 10000;
			Data = Value/1000;
			if(Data)
			{
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else
			{
				if(IsZero)
				{
					*Ptr++ = ' ';
				}
				else
				{
					*Ptr++ = '0';
				}
			}
			Value %= 1000;
			Data = Value/100;
			if(Data)
			{
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else
			{
				if(IsZero)
				{
					*Ptr++ = ' ';
				}
				else
				{
					*Ptr++ = '0';
				}
			}
			Value %= 100;
			Data = Value/10;
			if(Data)
			{
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else
			{
				if(IsZero)
				{
					*Ptr++ = ' ';
				}
				else
				{
					*Ptr++ = '0';
				}
			}
			Value %= 10;
			*Ptr++ = Value + 0x30;
		break;
	}
}
void pLcd_DisplayShort_Format(uint8_t Format,uint16_t Val,char *Ptr)
{
	uint16_t Value = Val;
	uint16_t Data;
	uint8_t IsZero = 1;
	switch(Format)
	{
		case FORMAT_xxx:
			Data = Value/100;
			if(Data){
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else
			{
				*Ptr++ = ' ';
			}
			Value %= 100;
			Data = Value/10;
			if(Data){
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else
			{
				if(IsZero)
				{
					*Ptr++ = ' ';
				}
				else
				{
					*Ptr++ = '0';
				}
			}
			Value %= 10;
			*Ptr++ = Value + 0x30;
			break;
		//@150508 by Bang
		case FORMAT_xxxx:
			Data = Value/10000 + 0x30;
			*Ptr++ = Data;
			Value %= 10000;
			Data = Value/1000 + 0x30;
			*Ptr++ = Data;
			Value %= 1000;
			Data = Value/100 + 0x30;
			*Ptr++ = Data;
			Value %= 100;
			Data = Value/10 + 0x30;
			*Ptr++ = Data;
			Value %= 10;
			*Ptr++ = Value + 0x30;
			break;
		case FORMAT10_xxxPx:
			Data = Value/1000 + 0x30;
			if(Data == 0x30) Data = 0x20;
			*Ptr++ = Data;
			Value %= 1000;
			Data = Value/100 + 0x30;
			if(Data == 0x30) Data = 0x20;
			*Ptr++ = Data;
			Value %= 100;
			*Ptr++ = Value/10 + 0x30;
			*Ptr++ = '.';
			Value %= 10;
			*Ptr++ = Value + 0x30;
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
			Data = Value/100 + 0x30;
			if(Data == 0x30) Data = 0x20;
			*Ptr++ = Data;
			Value %= 100;
			*Ptr++ = Value/10 + 0x30;
			*Ptr++ = '.';
			Value %= 10;
			*Ptr++ = Value + 0x30;
			break;

		case FORMAT10_xPxx:
		//	Value = Val * 10;
			*Ptr++ = Value/100 + 0x30;
			*Ptr++ = '.';
			Value %= 100;
			*Ptr++ = Value/10 + 0x30;
			Value %= 10;
			*Ptr++ = Value + 0x30;
			break;

		case FORMAT10_xxPxx:
			Value = Val * 10;
			Data = Value/10000 + 0x30;
			if(Data == 0x30) Data = 0x20;
			*Ptr++ = Data;
			Value %= 10000;
			Data = Value/1000 + 0x30;
			*Ptr++ = Data;
			*Ptr++ = '.';
			Value %= 1000;
			*Ptr++ = Value/100 + 0x30;
			Value %= 100;
			*Ptr++ = Value/10 + 0x30;
			break;
		case FORMAT10_xxxPxx:
			Value = Val * 10;
			Data = Value/10000 + 0x30;
			if(Data == 0x30) Data = 0x20;
			*Ptr++ = Data;
			Value %= 10000;
			Data = Value/1000 + 0x30;
			*Ptr++ = Data;
			*Ptr++ = '.';
			Value %= 1000;
			*Ptr++ = Value/100 + 0x30;
			Value %= 100;
			*Ptr++ = Value/10 + 0x30;
			break;
	}
}


void pLcd_DisplayByte_Format(uint8_t Format,uint8_t Val,char *Ptr)
{
	uint16_t Value = Val;
	uint8_t Data;
	
	switch(Format)
	{
		case FORMAT_xxx:
			Data = Value/100 + 0x30;
			if(Data == 0x30) Data = 0x20;
			*Ptr++ = Data;
			Value %= 100;
			if(Data == 0x20){
				Data = Value/10 + 0x30;
				if(Data == 0x30) Data = 0x20;
				*Ptr++ = Data;
			}
			else{
				Data = Value/10 + 0x30;
				*Ptr++ = Data;
			}
			Value %= 10;
			*Ptr++ = Value + 0x30;
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
			Data = Value/100 + 0x30;
			if(Data == 0x30) Data = 0x20;
			*Ptr++ = Data;
			Value %= 100;
			*Ptr++ = Value/10 + 0x30;
			*Ptr++ = '.';
			Value %= 10;
			*Ptr++ = Value + 0x30;
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
			Data = Value/1000 + 0x30;
			if(Data == 0x30) Data = 0x20;
			*Ptr++ = Data;
			Value %= 1000;
			*Ptr++ = Value/100 + 0x30;
			*Ptr++ = '.';
			Value %= 100;
			*Ptr++ = Value/10 + 0x30;
			Value %= 10;
			*Ptr++ = Value + 0x30;
			break;
	}
}


extern uint32_t SS_Position;

void pLcd_LoadString(uint8_t Line)
{
	uint8_t i;
	int8_t *Ptr;
	int8_t Arr[17];
	uint8_t Debug = 0;

	switch(gDispMode)
	{
		case LCD_SYS_INIT:
			if(Line == 0)
			{
				Ptr = (int8_t *)&LCD_RESET0[Line][0];
			}
			else
			{
				Ptr = (char *)&LCD_BOTTOM_RESET0[0];
				Ptr += 11;
				for(i=1;i!=5;i++)
				{
					*Ptr++ = Versions[i];
				}
				Ptr = (int8_t *)&LCD_RESET0[1][0];
			}
			break;

		case LCD_MAIN_INIT:
			if(System.OverTemp){
				Ptr = (char *)&DISP_MODE_FAULT_OVERTEMP[Line][0];
			}
			else if(System.inverterCommError)
			{
				Ptr = (char *)&DISP_MODE_FAULT_INVERTERCOMM[Line][0];
			}
			else if(System.ssr1CommError)
			{
				Ptr = (char *)&DISP_MODE_FAULT_SSR1COMM[Line][0];
			}
			else if(System.ssr2CommError)
			{
				Ptr = (char *)&DISP_MODE_FAULT_SSR2COMM[Line][0];
			}
			else if(Ssr1.error)
			{
				if(Line == 0)Ptr = (char *)&DISP_MODE_FAULT_SSR1ERRORTOP[0];
				else
				{
					Ptr = (char *)&DISP_MODE_FAULT_SSRERROR[Ssr1.error][0];
				}
			}
			else if(Ssr2.error)
			{
				if(Line == 0)Ptr = (char *)&DISP_MODE_FAULT_SSR2ERRORTOP[0];
				else
				{
					Ptr = (char *)&DISP_MODE_FAULT_SSRERROR[Ssr2.error][0];
				}
			}
			else{
				if(Line == 0)	Ptr = (char *)&BLANK[0];
				else{
					Ptr = (char *)&SCR_L3MODE[gDispSub];
				}
			}
			break;
			
		case LCD_MAIN_MENU:
			if(Line == 0)	Ptr = (char *)&MAIN_MENU_TOP[gDispMode][0];
			else {
				Ptr = (char *)&MAIN_MENU_SUB[gDispSub][0];
			}
			break;

		case LCD_MAIN_HEAT_SET:
			if(Line == 0)	Ptr = (char *)&MAIN_MENU_TOP[gDispMode][0];
			else	{
				Ptr = (char *)&HEAT_MODE_SUB[gDispSub][0];

			}
			break;
		case LCD_MAIN_INV_SET:
			if(Line == 0)	Ptr = (char *)&MAIN_MENU_TOP[gDispMode][0];
			else	{
				Ptr = (char *)&INV_MODE_SUB[gDispSub][0];

			}
			break;

		case LCD_MAIN_INITIALIZE:
			if(Line == 0)	Ptr = (char *)&MAIN_MENU_TOP[3][0];
			else			Ptr = (char *)&INITIALIZE_MODE_SUB[gDispSub][0];
			break;

		case LCD_MAIN_FAULT:
			if(Line == 0)	Ptr = (char *)&MAIN_MENU_TOP[4][0];
			else			Ptr = (char *)&FAULT_MODE_SUB[gDispSub][0];
			break;

		case LCD_MAIN_HEAT_SET_SUB:
			if(Line == 0)	Ptr = (char *)&HEAT_MODE_SUB_TOP[gDispSub][0];
			else			Ptr = (char *)&BLANK[0];
			break;
		case LCD_MAIN_INV_SET_SUB:
			if(Line == 0)	Ptr = (char *)&INV_MODE_SUB_TOP[gDispSub][0];
			else			Ptr = (char *)&BLANK[0];
			break;
		case LCD_MAIN_FAULT_SUB:
			if(Line == 0)	Ptr = (char *)&FAULT_MODE_TOP[gDispSub][0];
			else			Ptr = (char *)&ERRMSG_LIST[gMonitorError[gDispSub]];
			break;
		
		default:
			break;
	}
	for(i=0;i!=16;i++){
		gLCD_Line[i] = *Ptr++;
	}

	/* Sub Data */
	switch(gDispMode)
	{
		case LCD_MAIN_INIT:
			if(System.OverTemp == 1) break;
			if(System.inverterCommError == 1) break;
			if(System.ssr1CommError == 1) break;
			if(System.ssr2CommError == 1) break;
			if(Ssr1.error) break;
			if(Ssr2.error) break;
			if(Debug == 1){
				int TempEncoder;
				if(Line == 0){
					TempEncoder = gWorkFreq;
					if(TempEncoder >= 0){
						gLCD_Line[0] = '+';
					}
					else{
						gLCD_Line[0] = '-';
						TempEncoder = (TempEncoder ^ 0xffffffff) + 1;
					}
					gLCD_Line[1] = TempEncoder / 1000000 + 0x30;
					TempEncoder = TempEncoder % 1000000;
					gLCD_Line[2] = TempEncoder / 100000 + 0x30;
					TempEncoder = TempEncoder % 100000;
					gLCD_Line[3] = TempEncoder / 10000 + 0x30;
					TempEncoder = TempEncoder % 10000;
					gLCD_Line[4] = TempEncoder / 1000 + 0x30;
					TempEncoder = TempEncoder % 1000;
					gLCD_Line[5] = TempEncoder / 100 + 0x30;
					TempEncoder = TempEncoder % 100;
					gLCD_Line[6] = TempEncoder / 10 + 0x30;
					TempEncoder = TempEncoder % 10;
					gLCD_Line[7] = TempEncoder + 0x30;
					/* Encoder Display */
				//	gLCD_Line[7] = ' ';
					TempEncoder = gWorkFreq;
					if(TempEncoder >= 0){
						gLCD_Line[8] = '+';
					}
					else{
						gLCD_Line[8] = '-';
						TempEncoder = (TempEncoder ^ 0xffffffff) + 1;
					}
					gLCD_Line[9] = TempEncoder / 1000000 + 0x30;
					TempEncoder = TempEncoder % 1000000;
					gLCD_Line[10] = TempEncoder / 100000 + 0x30;
					TempEncoder = TempEncoder % 100000;
					gLCD_Line[11] = TempEncoder / 10000 + 0x30;
					TempEncoder = TempEncoder % 10000;
					gLCD_Line[12] = TempEncoder / 1000 + 0x30;
					TempEncoder = TempEncoder % 1000;
					gLCD_Line[13] = TempEncoder / 100 + 0x30;
					TempEncoder = TempEncoder % 100;
					gLCD_Line[14] = TempEncoder / 10 + 0x30;
					TempEncoder = TempEncoder % 10;
					gLCD_Line[15] = TempEncoder + 0x30;
				}
				else{
					TempEncoder = gWorkFreq;
					if(TempEncoder >= 0){
						gLCD_Line[0] = '+';
					}
					else{
						gLCD_Line[0] = '-';
						TempEncoder = (TempEncoder ^ 0xffffffff) + 1;
					}
					gLCD_Line[1] = TempEncoder / 1000000 + 0x30;
					TempEncoder = TempEncoder % 1000000;
					gLCD_Line[2] = TempEncoder / 100000 + 0x30;
					TempEncoder = TempEncoder % 100000;
					gLCD_Line[3] = TempEncoder / 10000 + 0x30;
					TempEncoder = TempEncoder % 10000;
					gLCD_Line[4] = TempEncoder / 1000 + 0x30;
					TempEncoder = TempEncoder % 1000;
					gLCD_Line[5] = TempEncoder / 100 + 0x30;
					TempEncoder = TempEncoder % 100;
					gLCD_Line[6] = TempEncoder / 10 + 0x30;
					TempEncoder = TempEncoder % 10;
					gLCD_Line[7] = TempEncoder + 0x30;
					TempEncoder = gWorkFreq;
					if(TempEncoder >= 0){
						gLCD_Line[8] = '+';
					}
					else{
						gLCD_Line[8] = '-';
						TempEncoder = (TempEncoder ^ 0xffffffff) + 1;
					}
					gLCD_Line[9] = TempEncoder / 1000000 + 0x30;
					TempEncoder = TempEncoder % 1000000;
					gLCD_Line[10] = TempEncoder / 100000 + 0x30;
					TempEncoder = TempEncoder % 100000;
					gLCD_Line[11] = TempEncoder / 10000 + 0x30;
					TempEncoder = TempEncoder % 10000;
					gLCD_Line[12] = TempEncoder / 1000 + 0x30;
					TempEncoder = TempEncoder % 1000;
					gLCD_Line[13] = TempEncoder / 100 + 0x30;
					TempEncoder = TempEncoder % 100;
					gLCD_Line[14] = TempEncoder / 10 + 0x30;
					TempEncoder = TempEncoder % 10;
					gLCD_Line[15] = TempEncoder + 0x30;
				}
				break;
			}

			if(Line == 0){
				switch(gDispSub)
				{
					case LCD_MAIN_STATUS_0:
						for(i=0;i!=16;i++)Arr[i] = LCD_MAIN0[0][i];
						pLcd_DisplayShort_Format(FORMAT_xxx, gParameter.TopTemp/100, (char *)&Arr[4]);
						pLcd_DisplayShort_Format(FORMAT_xxx, gParameter.BottomTemp/100, (char *)&Arr[12]);
						if(Ssr1.output == 1)Arr[1] = 0xFF;
						else Arr[1] = ' ';
						if(Ssr2.output == 1)Arr[9] = 0xFF;
						else Arr[9] = ' ';
						Ptr = (int8_t *)&Arr[0];
						break;
					case LCD_MAIN_STATUS_1:
						if(gInverterStatus.bit.fowardRun == 1)i = 0;
						else if(gInverterStatus.bit.reverseRun == 1)i = 1;
						else i = 2;
						
						Ptr = (int8_t *)&INVERTER_OP[i][0];
						break;
					case LCD_MAIN_STATUS_2:
						for(i=0;i!=16;i++)Arr[i] = SCR_L3MODE[3][i];
					
						pLcd_DisplayShort_Format(FORMAT10_xxxPx, Ssr1.voltage, (char *)&Arr[7]);
						if(gStatus.status > STATUS_INIT)Arr[14] = 0xff;
						else Arr[14] = ' ';
						Ptr = (int8_t *)&Arr[0];
				}
				
				for(i=0;i!=16;i++)gLCD_Line[i] = *Ptr++;
			}
			else{
				/*uint8_t Ratio_Voltage,i;
				uint16_t Data,Value;
				uint8_t *Ptr;*/

				switch(gDispSub)
				{
					case LCD_MAIN_STATUS_0:		/* Heater Information */
						pLcd_DisplayShort_Format(FORMAT_xxx, gStatus.Heater_Top.Temp/100, (char *)&gLCD_Line[4]);
						pLcd_DisplayShort_Format(FORMAT_xxx, gStatus.Heater_Bottom.Temp/100, (char *)&gLCD_Line[12]);
						//SSR1, SSR2 동작 표시
						
						break;

					case LCD_MAIN_STATUS_1:		/* Inverter Information */
						if(System.inverterError)
						{
							i = getDigitPosition(gInverterError.All);
							Ptr = (int8_t *)&InverterErrorString[i][0];
						}
						else
						{
							pLcd_DisplayShort_Format(FORMAT10_xxxPxx,gWorkFreq,(char *)&gLCD_Line[8]);
						}
						break;

					case LCD_MAIN_STATUS_2:		/* Voltage */
						/*if(Ssr2.output == 0)i = 0;
						else */i = Ssr2.voltage/38;
						pLcd_DisplayShort_Format(FORMAT10_xxxPx, Ssr2.voltage,(char *)&gLCD_Line[7]);
						if(gStatus.status > STATUS_INIT)gLCD_Line[14] = 0xFF;
						else gLCD_Line[14] = ' ';
						break;
				}
			}
			break;

		case LCD_MAIN_FACTORY: //@150423 by Bang
			
			break;
		
		

		case LCD_MAIN_HEAT_SET_SUB:
			if(Line == 1){
				pLcd_DisplayShort_Format(FORMAT_xxxx,gDispParameter.Value,(char *)&gLCD_Line[4]);
				for(i=0;i!=5;i++)	gLCD_Line[10+i]=UNIT[gDispSub][i];
				break;
			}
			break;
		case LCD_MAIN_INV_SET_SUB:
			if(Line == 1){
				pLcd_DisplayShort_Format(FORMAT10_xxxPxx,gDispParameter.Value,(char *)&gLCD_Line[4]);
				for(i=0;i!=5;i++)	gLCD_Line[10+i]=UNIT[gDispSub][i];
				break;
			}
			break;
	}
}

void pLcd_UpdateDisplay(void)
{
      
	if(gDisplay.Request == DISPLAY_REQUEST_ON){
		if(gDisplay.Connect == 1){
			if(gDisplay.Lines & TOP_LINE){
				pLcd_LoadString(0);
				pLcd_DataOut(0x80);
			}
			if(gDisplay.Lines & BTM_LINE){
				pLcd_LoadString(1);
				pLcd_DataOut(0xc0);
			}
			gDisplay.Lines = 0;
			gDisplay.Request = DISPLAY_REQUEST_OFF;
	
			/*@081021_1*/		
			gLCD_Refresh_Timer = 0;
			gLCD_Mode_Backup = gDispMode;
			
		}
		else pLCD_Connect_Check();
	}
}


void pLcd_SystemLockLoadString(uint8_t Line)
{
	uint8_t i;
	char *Ptr;

	Ptr = (char *)&SystemLock_Top[Line][0];

	for(i=0;i!=16;i++){
		gLCD_Line[i] = *Ptr++;
	}

	if(Line == 1){
	
	}
}

