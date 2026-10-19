#ifndef _LCD_CTRL_H_
#define _LCD_CTRL_H_
#include "hw_def.h"
#include "global.h"

#define	C_FUNCTION_SET	0x20
#define	TWO_LINE_MODE		0x08
#define	ONE_LINE_MODE		0x00
#define	DISPLAY_ON_MODE	0x04
#define	DISPLAY_OFF_MODE	0x00

#define	C_DISPLAY_SET		0x08
//	#define	DISPLAY_ON_MODE	0x04
//	#define	DISPLAY_OFF_MODE	0x00
#define	CURSOR_ON_MODE	0x02
#define	CURSOR_OFF_MODE	0x00
#define	BLINK_ON_MODE		0x01
#define	BLINK_OFF_MODE	0x00

#define	C_DISPLAY_CLEAR		0x01

#define	C_ENTRY_MODE_SET		0x04
#define	INCREMENT_MODE		0x02
#define	DECREMENT_MODE		0x00
#define	ENTIRE_SHIFT_ON		0x01
#define	ENTIRE_SHIFT_OFF		0x00



#define	C_PORT_ADDR	0x70

enum{
	LCD_CTRL_RS=0,
	LCD_CTRL_RW,
	LCD_CTRL_EN
};


enum{
	LEVEL_LOW = 0,
	LEVEL_HIGH
};
void pLcd_COMMAND(uint8_t Data);
void pLcd_Data(uint8_t Data);
void pLcd_DataOut(uint8_t Data);
void pLcd_Delay(uint8_t MS);
void pLcd_Init(void);
void pLcd_Init_Seq(void);
void pLCD_Connect_Check(void);
#endif
