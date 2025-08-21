#include "global.h"
#include "display.h"
#include "LCD_CTRL.h"
#include "strings.h"
#include "uart.h"
#include "ssr.h"
#include "key.h"
#include "FlashOption.h"
static _Para dDispParameter;
extern short getTemperature(uint8_t no);
extern uint8_t getMonitorError(uint8_t no);
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
	uint8_t ssrError;
	int8_t Arr2[9];
	uint8_t Debug = 0;
	uint16_t pTemp;
	
	// 1. 안전한 작업을 위한 17바이트 임시 로컬 버퍼 선언 (null 문자 포함)
	uint8_t temp_lcd_line[17] = {0,};
	ssrError = getSSRError();
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
				pRead_FlashVersion((int8_t *)&Arr2[0]);
				for(i=1;i!=5;i++)
				{
					Arr[i+11] = Arr2[i];
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
			
			else if(ssrError)
			{
				if(Line == 0)Ptr = (char *)&DISP_MODE_FAULT_SSRERRORTOP[0];
				else
				{
					Ptr = (char *)&DISP_MODE_FAULT_SSRERROR[ssrError][0];
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
		case LCD_MAIN_INV_SET_SUB:
			if(Line == 0)	Ptr = (char *)&INV_MODE_SUB_TOP[gDispSub][0];
			else			Ptr = (char *)&BLANK[0];
			break;
		case LCD_MAIN_FAULT_SUB:
			if(Line == 0)	Ptr = (char *)&FAULT_MODE_TOP[gDispSub][0];
			else{
							i = getMonitorError(gDispSub);
							Ptr = (char *)&ERRMSG_LIST_EEP[i];
			}
			break;
		
		default:
			// Ptr이 초기화되지 않는 경우를 방지하기 위해 default 케이스 추가
			Ptr = (char *)&BLANK[0];
			break;
	}

	// 2. 모든 문자열 작업을 gLCD_Line 대신 temp_lcd_line 에서 수행
	for(i=0;i!=16;i++){
		temp_lcd_line[i] = *Ptr++;
	}
	
	getDispParameter(&dDispParameter);
	/* Sub Data */
	switch(gDispMode)
	{
		case LCD_MAIN_INIT:
			if(gError.All > 0)break;
			if(ssrError) break;
			if(Debug == 1){
				int TempEncoder;
				if(Line == 0){
					TempEncoder = gWorkPWM;
					if(TempEncoder >= 0){
						temp_lcd_line[0] = '+';
					}
					else{
						temp_lcd_line[0] = '-';
						TempEncoder = (TempEncoder ^ 0xffffffff) + 1;
					}
					temp_lcd_line[1] = TempEncoder / 1000000 + 0x30;
					TempEncoder = TempEncoder % 1000000;
					temp_lcd_line[2] = TempEncoder / 100000 + 0x30;
					TempEncoder = TempEncoder % 100000;
					temp_lcd_line[3] = TempEncoder / 10000 + 0x30;
					TempEncoder = TempEncoder % 10000;
					temp_lcd_line[4] = TempEncoder / 1000 + 0x30;
					TempEncoder = TempEncoder % 1000;
					temp_lcd_line[5] = TempEncoder / 100 + 0x30;
					TempEncoder = TempEncoder % 100;
					temp_lcd_line[6] = TempEncoder / 10 + 0x30;
					TempEncoder = TempEncoder % 10;
					temp_lcd_line[7] = TempEncoder + 0x30;
					/* Encoder Display */
					TempEncoder = gWorkPWM;
					if(TempEncoder >= 0){
						temp_lcd_line[8] = '+';
					}
					else{
						temp_lcd_line[8] = '-';
						TempEncoder = (TempEncoder ^ 0xffffffff) + 1;
					}
					temp_lcd_line[9] = TempEncoder / 1000000 + 0x30;
					TempEncoder = TempEncoder % 1000000;
					temp_lcd_line[10] = TempEncoder / 100000 + 0x30;
					TempEncoder = TempEncoder % 100000;
					temp_lcd_line[11] = TempEncoder / 10000 + 0x30;
					TempEncoder = TempEncoder % 10000;
					temp_lcd_line[12] = TempEncoder / 1000 + 0x30;
					TempEncoder = TempEncoder % 1000;
					temp_lcd_line[13] = TempEncoder / 100 + 0x30;
					TempEncoder = TempEncoder % 100;
					temp_lcd_line[14] = TempEncoder / 10 + 0x30;
					TempEncoder = TempEncoder % 10;
					temp_lcd_line[15] = TempEncoder + 0x30;
				}
				else{
					TempEncoder = gWorkPWM;
					if(TempEncoder >= 0){
						temp_lcd_line[0] = '+';
					}
					else{
						temp_lcd_line[0] = '-';
						TempEncoder = (TempEncoder ^ 0xffffffff) + 1;
					}
					temp_lcd_line[1] = TempEncoder / 1000000 + 0x30;
					TempEncoder = TempEncoder % 1000000;
					temp_lcd_line[2] = TempEncoder / 100000 + 0x30;
					TempEncoder = TempEncoder % 100000;
					temp_lcd_line[3] = TempEncoder / 10000 + 0x30;
					TempEncoder = TempEncoder % 10000;
					temp_lcd_line[4] = TempEncoder / 1000 + 0x30;
					TempEncoder = TempEncoder % 1000;
					temp_lcd_line[5] = TempEncoder / 100 + 0x30;
					TempEncoder = TempEncoder % 100;
					temp_lcd_line[6] = TempEncoder / 10 + 0x30;
					TempEncoder = TempEncoder % 10;
					temp_lcd_line[7] = TempEncoder + 0x30;
					TempEncoder = gWorkPWM;
					if(TempEncoder >= 0){
						temp_lcd_line[8] = '+';
					}
					else{
						temp_lcd_line[8] = '-';
						TempEncoder = (TempEncoder ^ 0xffffffff) + 1;
					}
					temp_lcd_line[9] = TempEncoder / 1000000 + 0x30;
					TempEncoder = TempEncoder % 1000000;
					temp_lcd_line[10] = TempEncoder / 100000 + 0x30;
					TempEncoder = TempEncoder % 100000;
					temp_lcd_line[11] = TempEncoder / 10000 + 0x30;
					TempEncoder = TempEncoder % 10000;
					temp_lcd_line[12] = TempEncoder / 1000 + 0x30;
					TempEncoder = TempEncoder % 1000;
					temp_lcd_line[13] = TempEncoder / 100 + 0x30;
					TempEncoder = TempEncoder % 100;
					temp_lcd_line[14] = TempEncoder / 10 + 0x30;
					TempEncoder = TempEncoder % 10;
					temp_lcd_line[15] = TempEncoder + 0x30;
				}
				break;
			}

			if(Line == 0){
				uint8_t output;
				uint16_t voltage;
				switch(gDispSub)
				{
					case LCD_MAIN_STATUS_1:
						if(gMotorStatus == MOTOR_OFF)i = 1;
						else i = 0;
						
						Ptr = (int8_t *)&INVERTER_OP[i][0];
						break;
					case LCD_MAIN_STATUS_2:
						for(i=0;i!=16;i++)Arr[i] = SCR_L3MODE[3][i];
						pTemp = getSSRTemperature();
						pLcd_DisplayShort_Format(FORMAT_xxx, pTemp, (char *)&Arr[7]);
						if(gFan == 1)Arr[14] = '*';
						else Arr[14] = ' ';
						Ptr = (int8_t *)&Arr[0];
						break;
					/*case LCD_MAIN_STATUS_0:*/
					default:
						for(i=0;i!=16;i++)Arr[i] = LCD_MAIN0[0][i];
						pLcd_DisplayShort_Format(FORMAT_xxx, gParameter.Temperature/100, (char *)&Arr[2]);
						pTemp = gMotorSpeed * 10 / 721;
						pLcd_DisplayShort_Format(FORMAT10_xxxPx, pTemp, (char *)&Arr[11]);
						output = getSSROutput();
						voltage = getSSRVoltage();
						if((output == 1) && (voltage > 0))Arr[7] = '*';
						else Arr[7] = ' ';
						Ptr = (int8_t *)&Arr[0];
						break;
					
				}
				
				for(i=0;i!=16;i++)temp_lcd_line[i] = *Ptr++;
			}
			else{
				short value;
				switch(gDispSub)
				{
					case LCD_MAIN_STATUS_1:	/* Inverter Information */
						pTemp = gMotorSpeed * 10 / 721;
						pLcd_DisplayShort_Format(FORMAT10_xxxPx,pTemp,(char *)&temp_lcd_line[8]);
						break;

					case LCD_MAIN_STATUS_2:	
						i = getSSROutput();
						value = getSSRVoltage();
						if(i == 0)pTemp = 0;
						else pTemp = value*10;
						
						pLcd_DisplayShort_Format(FORMAT10_xxxPx, pTemp,(char *)&temp_lcd_line[7]);
						if(gStatus.status > STATUS_INIT)temp_lcd_line[14] = '*';
						else temp_lcd_line[14] = ' ';
							
						break;
					default:
					/*case LCD_MAIN_STATUS_0:	 Heater Information */
						value = getTemperature(0);
						if(value >= 0)pLcd_DisplayShort_Format(FORMAT_xxx, value/100, (char *)&temp_lcd_line[2]);
						else
						{
							temp_lcd_line[3] = '-';
							pLcd_DisplayShort_Format(FORMAT_xxx, (value*-1)/100, (char *)&temp_lcd_line[2]);
						}
						value = getTemperature(1);
						if(value >= 0)	pLcd_DisplayShort_Format(FORMAT_xxx, value/100, (char *)&temp_lcd_line[11]);
						else{
							temp_lcd_line[10] = '-';
							pLcd_DisplayShort_Format(FORMAT_xxx, (value*-1)/100, (char *)&temp_lcd_line[11]);
						}
						
						break;
				}
			}
			break;

		case LCD_MAIN_FACTORY:
			break;
		
		case LCD_MAIN_HEAT_SET_TOP_SUB:
			if(Line == 1){
				pLcd_DisplayShort_Format(FORMAT10_xxxPxx,dDispParameter.Value,(char *)&temp_lcd_line[4]);
				for(i=0;i!=5;i++)	temp_lcd_line[10+i]=UNIT_HEAT[gDispSub][i];
				break;
			}
			break;
		case LCD_MAIN_INV_SET_SUB:
			if(Line == 1){
				if(gDispSub == LCD_SUB_INV_DIR)
				{
					if(dDispParameter.Value == 0)
					{
						temp_lcd_line[5] = 'C';
						temp_lcd_line[6] = 'W';
					}
					else
					{
						temp_lcd_line[4] = 'C';
						temp_lcd_line[5] = 'C';
						temp_lcd_line[6] = 'W';
					}
				}
				else
				{
					pLcd_DisplayShort_Format(FORMAT_xxPx, dDispParameter.Value, (char *)&temp_lcd_line[4]);
					for(i=0;i!=5;i++)	temp_lcd_line[10+i]=UNIT[gDispSub][i];
				}
				break;
			}
			break;
	}

	// 3. 마지막에 임시버퍼의 내용을 전역변수로 안전하게 복사
	for(i=0; i<16; i++)
	{
	    gLCD_Line[i] = temp_lcd_line[i];
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
