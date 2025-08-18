#include "global.h"
#include "display.h"
#include "LCD_CTRL.h"
#include "strings.h"
#include "uart.h"
#include "FlashOption.h"
extern short getTemperature(uint8_t no);
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
	uint32_t Value = Val;
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
			if(Data == 0x30) *Ptr++ = 0x20;
			else{
				IsZero = 0;
				*Ptr++ = Data;
			}
			Value %= 10000;
			Data = Value/1000 + 0x30;
			if(Data == 0x30){
				if(IsZero) *Ptr++ = ' ';
				else *Ptr++ = Data;
				
			}
			else *Ptr++ = Data;
			Value %= 1000;
			Data = Value/100 + 0x30;
			if(Data == 0x30){
				if(IsZero) *Ptr++ = ' ';
				else *Ptr++ = Data;
				
			}
			else *Ptr++ = Data;
			Value %= 100;
			Data = Value/10 + 0x30;
			if(Data == 0x30){
				if(IsZero) *Ptr++ = ' ';
				else *Ptr++ = Data;
				
			}
			else *Ptr++ = Data;
			Value %= 10;
			*Ptr++ = Value + 0x30;
			break;
		case FORMAT10_xxxPx:
			Data = Value/1000 + 0x30;
			if(Data == 0x30) *Ptr++ = 0x20;
			else{
				IsZero = 0;
				*Ptr++ = Data;
			}
			Value %= 1000;
			Data = Value/100 + 0x30;
			if(Data == 0x30){
				if(IsZero) *Ptr++ = ' ';
				else *Ptr++ = Data;
				
			}
			else *Ptr++ = Data;
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
			Value = Val*10;
			Data = Value/100000;
			if(Data){
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else *Ptr++ = ' ';
			Value %= 100000;
			Data = Value/10000;
			if(Data)
			{
				*Ptr++ = Data + 0x30;
				IsZero = 0;
			}
			else
			{
				if(IsZero)*Ptr++ = ' ';
				else *Ptr++ = 0x30;
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
				*Ptr++ = 0x30;
			}
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
	uint16_t pTemp;
	
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
				for(i=0;i!=16;i++)Arr[i] = *Ptr++;
				for(i=1;i!=5;i++)
				{
					Arr[i+11] = Versions[i];
				}
				Ptr = (int8_t *)&Arr[0];
			}
			break;

		case LCD_MAIN_INIT:
			if(gError.bit.EmergencyOn)
			{
				Ptr = (char *)&EMG_DISPLAY[Line][0];
			}
			else if(gError.bit.OverTemp){
				Ptr = (char *)&DISP_MODE_FAULT_OVERTEMP[Line][0];
			}
			else if(gError.bit.Motor)
			{
				Ptr = (char *)&DISP_MODE_FAULT_MOTORERROR[Line][0];
			}
			else if(gError.bit.TopSensor)
			{
				Ptr = (char *)&DISP_MODE_FAULT_UPPERSENSOR[Line][0];
			}
			else if(gError.bit.BottomSensor)
			{
				Ptr = (char *)&DISP_MODE_FAULT_LOWERSENSOR[Line][0];
			}
			else if(gError.bit.ssrCommError)
			{
				Ptr = (char *)&DISP_MODE_FAULT_SSRCOMM[Line][0];
			}
			else if(gError.bit.ssrError)
			{
				Ptr = (char *)&DISP_MODE_FAULT_SSRHWERROR[Line][0];
			}
			
			else if(SSR.error)
			{
				if(Line == 0)Ptr = (char *)&DISP_MODE_FAULT_SSRERRORTOP[0];
				else
				{
					Ptr = (char *)&DISP_MODE_FAULT_SSRERROR[SSR.error][0];
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
		
/*		case LCD_MAIN_HEAT_SET_WARMUP:
			if(Line == 0)	Ptr = (char *)&HEAT_MODE_SUB_TOP[1][0];
			else	{
				Ptr = (char *)&WARMUP_MODE_SUB[gDispSub][0];

			}
			break;*/
		case LCD_MAIN_INV_SET:
			if(Line == 0)	Ptr = (char *)&MAIN_MENU_TOP[gDispMode][0];
			else	{
				Ptr = (char *)&INV_MODE_SUB[gDispSub][0];

			}
			break;

		case LCD_MAIN_INITIALIZE:
			if(Line == 0)	Ptr = (char *)&MAIN_MENU_TOP[3][0];
			else			Ptr = (char *)&INITIALIZE_MODE_SUB[0];
			break;

		case LCD_MAIN_FAULT:
			if(Line == 0)	Ptr = (char *)&MAIN_MENU_TOP[4][0];
			else			Ptr = (char *)&FAULT_MODE_SUB[gDispSub][0];
			break;

		case LCD_MAIN_HEAT_SET_SUB:
			if(Line == 0)	Ptr = (char *)&HEAT_MODE_SUB_TOP[gDispSub][0];
			else			Ptr = (char *)&BLANK[0];
			break;
		case LCD_MAIN_HEAT_SET_TOP_SUB:
			if(Line == 0)	Ptr = (char *)&HEAT_MODE_SUB_TOP[0][0];
			else			Ptr = (char *)&BLANK[0];
			break;
/*		
		case LCD_MAIN_HEAT_SET_WARMUP_SUB:
			if(Line == 0)	Ptr = (char *)&HEAT_WARMUP_SUB_TOP[gDispParameter.prev_sub][0];
			else			Ptr = (char *)&BLANK[0];
			break;*/
		case LCD_MAIN_INV_SET_SUB:
			if(Line == 0)	Ptr = (char *)&INV_MODE_SUB_TOP[gDispSub][0];
			else			Ptr = (char *)&BLANK[0];
			break;
		case LCD_MAIN_FAULT_SUB:
			if(Line == 0)	Ptr = (char *)&FAULT_MODE_TOP[gDispSub][0];
			else			Ptr = (char *)&ERRMSG_LIST_EEP[gMonitorError[gDispSub]];
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
			if(gError.All > 0)break;
			if(SSR.error) break;
			if(Debug == 1){
				
				break;
			}

			if(Line == 0){
				switch(gDispSub)
				{
					case LCD_MAIN_STATUS_0:
						/*if((gStatus.status > STATUS_INIT) && (gStatus.warmupComplete == 0))
						{
							for(i=0;i!=16;i++)Arr[i] = LCD_MAIN1[0][i];
						}
						else
						{*/
							for(i=0;i!=16;i++)Arr[i] = LCD_MAIN0[0][i];
							pLcd_DisplayShort_Format(FORMAT_xxx, gParameter.Temperature/100, (char *)&Arr[2]);
						//}
						pTemp = gMotorSpeed * 10 / 721
							;
						pLcd_DisplayShort_Format(FORMAT10_xxxPx, pTemp, (char *)&Arr[11]);
						//SSR1, SSR2 ���� ǥ��
						if((SSR.output == 1) && (SSR.outVoltage > 0))Arr[7] = '*';
						else Arr[7] = ' ';
						Ptr = (int8_t *)&Arr[0];
						break;
					case LCD_MAIN_STATUS_1:
						if(gMotorStatus == MOTOR_OFF)i = 1;
						else i = 0;
						
						Ptr = (int8_t *)&INVERTER_OP[i][0];
						break;
					case LCD_MAIN_STATUS_2:
						for(i=0;i!=16;i++)Arr[i] = SCR_L3MODE[3][i];
						pTemp = SSR.temperature;
						pLcd_DisplayShort_Format(FORMAT_xxx, pTemp, (char *)&Arr[7]);
						if(gFan == 1)Arr[14] = '*';
						else Arr[14] = ' ';
						Ptr = (int8_t *)&Arr[0];
						break;
				}
				
				for(i=0;i!=16;i++)gLCD_Line[i] = *Ptr++;
			}
			else{
				/*uint8_t Ratio_Voltage,i;
				uint16_t Data,Value;
				uint8_t *Ptr;*/
				short value;

				switch(gDispSub)
				{
					case LCD_MAIN_STATUS_0:		/* Heater Information */
						value = getTemperature(0);
						if(value >= 0)pLcd_DisplayShort_Format(FORMAT_xxx, value/100, (char *)&gLCD_Line[2]);
						else
						{
							gLCD_Line[3] = '-';
							pLcd_DisplayShort_Format(FORMAT_xxx, (value*-1)/100, (char *)&gLCD_Line[2]);
						}
						value = getTemperature(1);
						if(value >= 0)	pLcd_DisplayShort_Format(FORMAT_xxx, value/100, (char *)&gLCD_Line[11]);
						else{
							gLCD_Line[10] = '-';
							pLcd_DisplayShort_Format(FORMAT_xxx, (value*-1)/100, (char *)&gLCD_Line[11]);
						}
						
						break;

					case LCD_MAIN_STATUS_1:		/* Inverter Information */
						pTemp = gMotorSpeed * 10 / 721;
						
							
						pLcd_DisplayShort_Format(FORMAT10_xxxPx,pTemp,(char *)&gLCD_Line[8]);
						break;

					case LCD_MAIN_STATUS_2:	
						if(SSR.output == 0)pTemp = 0;
						else pTemp = SSR.outVoltage*10;
						
						pLcd_DisplayShort_Format(FORMAT10_xxxPx, pTemp,(char *)&gLCD_Line[7]);
						if(gStatus.status > STATUS_INIT)gLCD_Line[14] = '*';
						else gLCD_Line[14] = ' ';
							
						break;
				}
			}
			break;

		case LCD_MAIN_FACTORY: //@150423 by Bang
			
			break;
		
		

		case LCD_MAIN_HEAT_SET_TOP_SUB:
			if(Line == 1){
				pLcd_DisplayShort_Format(FORMAT10_xxxPxx,gDispParameter.Value,(char *)&gLCD_Line[4]);
				for(i=0;i!=5;i++)	gLCD_Line[10+i]=UNIT_HEAT[gDispSub][i];
				break;
			}
			break;
/*		case LCD_MAIN_HEAT_SET_WARMUP_SUB:
			if(Line == 1){
				if(gDispParameter.prev_sub == 0)pLcd_DisplayShort_Format(FORMAT_xxx,gDispParameter.Value,(char *)&gLCD_Line[4]);
				else pLcd_DisplayShort_Format(FORMAT10_xxxPxx,gDispParameter.Value,(char *)&gLCD_Line[4]);
				for(i=0;i!=5;i++)	gLCD_Line[10+i]=UNIT_HEAT[gDispParameter.prev_sub + 5][i];
				break;
			}
			break;*/
		case LCD_MAIN_INV_SET_SUB:
			if(Line == 1){
				if(gDispSub == LCD_SUB_INV_DIR)
				{
					if(gDispParameter.Value == 0)
					{
						gLCD_Line[5] = 'C';
						gLCD_Line[6] = 'W';
					}
					else
					{
						gLCD_Line[4] = 'C';
						gLCD_Line[5] = 'C';
						gLCD_Line[6] = 'W';
					}
				}
				else
				{
					pLcd_DisplayShort_Format(FORMAT_xxx, gDispParameter.Value, (char *)&gLCD_Line[4]);
				//	pLcd_DisplayShort_Format(FORMAT10_xxxPxx,gDispParameter.Value,(char *)&gLCD_Line[4]);
					for(i=0;i!=5;i++)	gLCD_Line[10+i]=UNIT[gDispSub][i];
				}
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



