#include "global.h"
#include "FND.h"
#include "strings.h"

#define	PORT_FND_SEL1	((*(RB_BTF*)&P8OUT).p1)
#define	PORT_FND_SEL2	((*(RB_BTF*)&P8OUT).p2)
#define	PORT_FND_SEL3	((*(RB_BTF*)&P8OUT).p3)
#define	PORT_FND_SEL4	((*(RB_BTF*)&P8OUT).p4)
#define	PORT_FND_G	((*(RB_BTF*)&P6OUT).p0)
#define	PORT_FND_DT	((*(RB_BTF*)&P6OUT).p1)
#define FND_ALL_OFF		(P8OUT &= ~0x1E)

typedef struct{
	uint8_t ch;
	uint16_t time;
	uint16_t temp1;
	uint16_t temp2;
}_VALUE;
static _VALUE pValue; 
static gFndText[4];
void FND_Init(void)
{
	P8DIR |= 0x1E;
	P9DIR |= 0x3f;
	P6DIR |= 0x03;
	
	FND_ALL_OFF;
}
void pFnd_LoadString(void)
{
	uint32_t pre_time;
	uint16_t data,value;
	uint16_t On=0;
	if(gStatus.status == STATUS_ERR)
	{
		pre_time = millis() % 400;
		if(pre_time > 200){
			gFndText[0] = S_BLK;
			gFndText[1] = S_BLK;
			gFndText[2] = S_BLK;
			gFndText[3] = S_BLK;
		}
		else{
			gFndText[0] = S_E;
			gFndText[1] = S_R;
			gFndText[2] = S_R;
			gFndText[3] = S_BLK;
		}
	}
	else{
		switch(gStatus.Mode)
		{
			case 0:
				data = gStatus.RunTime;
				gFndText[0] = S_A;
				break;
			case 1:
				data = gStatus.Heater_Top.Comp/100;
				gFndText[0] = S_B;
				break;
			case 2:
				data = gStatus.Heater_Bottom.Comp/100;
				gFndText[0] = S_C;
				break;
			case 3:
				data = gStatus.Heater_Top.Temp / 100;
				gFndText[0] = S_D;
				break;
			case 4:
				data = gStatus.Heater_Bottom.Temp / 100;
				gFndText[0] = S_E;
				break;
			case 5:
				data = gRoast_No;
				gFndText[0] = S_F;
		}
		gFndText[0] |= 0x80;
		data %= 1000;
		value = data / 100;
		if(value > 0){
			gFndText[1] = Dec2Bcd[value];
			On = 1;
		}
		else gFndText[1] = S_BLK;
		data %= 100;
		value = data / 10;
		if(value > 0)
		{
			gFndText[2] = Dec2Bcd[value];
		}
		else if(On == 1){
			gFndText[2] = Dec2Bcd[0];
		}
		else gFndText[2] = S_BLK;
		data %= 10;
		gFndText[3] = Dec2Bcd[data];
	}
}
void FND_Update(void)
{
	static uint32_t pre_time;
	static uint8_t Cnt = 0;
	if(millis() == pre_time)return;
	pre_time = millis();
	pFnd_LoadString();
	FND_ALL_OFF;
	
	P9OUT = gFndText[Cnt] & 0x3F;
	PORT_FND_G = (gFndText[Cnt] >> 6) & 0x01;
	PORT_FND_DT = (gFndText[Cnt] >> 7)& 0x01;
	switch(Cnt){
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

	}
	if(++Cnt > 3)Cnt = 0;
	
}
void gSetValue(uint8_t ch, uint16_t value)
{
	switch(ch)
	{
		case 0:
			pValue.time = value;
			break;
		case 1:
			pValue.temp1 = value;
			break;
		case 2:
			pValue.temp2 = value;
			break;
		case 3:
			if(pValue.ch < 2)pValue.ch++;
			else pValue.ch = 0;
	}
}
