#include "global.h"
#include "LCD_CTRL.h"
static uint8_t last_error = 0;
/**************************************************************/
/*                                                                                                         */
/* PORT BIT DEFINE                                                                              */
/*                                                                                                         */
/**************************************************************/
#define	PORT_LCD_RS	((*(RB_BTF*)&PDOUT).p6)
#define	PORT_LCD_EN	((*(RB_BTF*)&PDOUT).p7)
void pLcd_Control_Port(uint8_t CTRL_REG,uint8_t Data);
void pLcd_Wait(void);
void pLcd_Data(uint8_t Data){
	
	P0OUT = Data >> 4;
	pLcd_Control_Port(LCD_CTRL_EN,LEVEL_HIGH);
	pLcd_Wait();
	pLcd_Control_Port(LCD_CTRL_EN,LEVEL_LOW);
	pLcd_Wait();
	P0OUT = Data & 0x0f;
	pLcd_Control_Port(LCD_CTRL_EN,LEVEL_HIGH);
	pLcd_Wait();
	pLcd_Control_Port(LCD_CTRL_EN,LEVEL_LOW);
	pLcd_Wait();
}

void pLCD_Connect_Check(void)
{
	if(millis() - gDisplay.pre_time > 200)
	{
		
			gDisplay.Connect = 1;
			last_error = 0;
		
	}
}
void pLcd_Control_Port(uint8_t CTRL_REG,uint8_t Data)
{
	switch(CTRL_REG)
	{
		case LCD_CTRL_RS:
			PORT_LCD_RS = Data;
			break;
		case LCD_CTRL_RW:
//			PORT_LCD_RW = Data;
			break;
		case LCD_CTRL_EN:
			PORT_LCD_EN = Data;
			break;
	}
}

void pLcd_Wait(void){
	uint16_t Delay;

	for(Delay = 0;Delay!=150;Delay++){
	}

}

/* Clock 8MHz, 1ms */
void pLcd_Delay(uint8_t MS){
	uint16_t Delay,j;

	for(j=0;j!=MS;j++){
		for(Delay = 0;Delay!=100;Delay++){
//			for(i=0;i!=10;i++);
		}
	}
}

void pLcd_COMMAND(uint8_t Data)
{
	pLcd_Control_Port(LCD_CTRL_RS,LEVEL_LOW);
	P0OUT = Data >> 4;
	pLcd_Control_Port(LCD_CTRL_EN,LEVEL_HIGH);
	pLcd_Wait();
	pLcd_Control_Port(LCD_CTRL_EN,LEVEL_LOW);
	pLcd_Wait();
	P0OUT = Data & 0x0f;
	pLcd_Control_Port(LCD_CTRL_EN,LEVEL_HIGH);
	pLcd_Wait();
	pLcd_Control_Port(LCD_CTRL_EN,LEVEL_LOW);
	pLcd_Wait();
	pLcd_Wait();
}

void pLcd_DataOut(uint8_t Data){
	uint8_t i;
	pLcd_COMMAND(Data);

	pLcd_Wait();
	pLcd_Control_Port(LCD_CTRL_RS,LEVEL_HIGH);
	pLcd_Wait();
	for(i=0;i!=16;i++)
	{
		pLcd_Data(gLCD_Line[i]);
	
	}
	
	pLcd_Wait();
	pLcd_Control_Port(LCD_CTRL_RS,LEVEL_LOW);
}

void pLcd_Init(void){
	
	P0DIR |= 0x0F;
	PDDIR |= 0xC0;
	PDOUT &= ~0xC0;
	
	/*delay(30);
	pLcd_COMMAND(0x03); // function set
	delay(30);
	pLcd_COMMAND(0x03); // function set
	delay(30);
	pLcd_COMMAND(0x03); // function set*/
	delay(30);
	pLcd_COMMAND(0x02); // function set
	delay(1);
	pLcd_COMMAND(0x28); // function set
	delay(1);
	pLcd_COMMAND(0x0C); // display on/off
	delay(1);
	pLcd_COMMAND(0x06); // display clear
	delay(1);
	pLcd_COMMAND(0x01); // entry mode set


	gDisplay.Connect = 1;
	gDispSub = 0;
	gDisplay.Request = DISPLAY_REQUEST_ON;
	gDispMode = LCD_SYS_INIT;
	gDisplay.Lines = TOP_LINE | BTM_LINE;
}


/*@081021_6 */
void pLcd_Init_Seq(void)
{
	static uint8_t Seq = 0;
	
	if(gLCD_Update_Req == 0) return;
	if(gDisplay.Connect == 0)return;
	gLCD_Update_Req = 0;

	switch(Seq)
	{
		case 0:
//			pLcd_COMMAND(0x03);
			break;
		case 1:
			pLcd_COMMAND(0x02);
			//pLcd_COMMAND(C_FUNCTION_SET | TWO_LINE_MODE);
			break;
		case 2:
			pLcd_COMMAND(C_FUNCTION_SET | TWO_LINE_MODE);
			break;
		case 3:
			pLcd_COMMAND(C_DISPLAY_SET | DISPLAY_ON_MODE | CURSOR_OFF_MODE | BLINK_OFF_MODE);
			break;
		case 4:
			pLcd_COMMAND(C_ENTRY_MODE_SET | INCREMENT_MODE |ENTIRE_SHIFT_OFF);
			break;
	}
	if(++Seq >= 5) Seq = 0;
}


