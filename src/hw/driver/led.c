#include "global.h"
#include "led.h"
#include "FND.h"
#define	LED1	((*(RB_BTF*)&P4OUT).p7)
#define	LED2	((*(RB_BTF*)&P5OUT).p0)
//#define	LED3	((*(RB_BTF*)&PDOUT).p7)
typedef struct
{
  uint8_t   on_state;
  uint8_t   off_state;
} led_tbl_t;
led_tbl_t led_tbl[LED_MAX_CH] =
{
    {1, 0}, // LED1
    {1, 0}, // LED2
};
uint8_t ledInit(void)
{
	uint8_t ret = 1;
	P4DIR |= 0x80;
	P5DIR |= 0x01;
	ledOff(0);
	ledOn(1);
	return ret;
}
void ledOn(uint8_t ch)
{
	switch(ch)
	{
		case _DEF_LED1:
			LED1 = led_tbl[ch].on_state;
			break;
		case _DEF_LED2:
			LED2 = led_tbl[ch].on_state;
			break;
		
	}
}
void ledOff(uint8_t ch)
{
	switch(ch)
	{
		case _DEF_LED1:
			LED1 = led_tbl[ch].off_state;
			break;
		case _DEF_LED2:
			LED2 = led_tbl[ch].off_state;
			break;
		
	}
}
void ledToggle(uint8_t ch)
{
	switch(ch)
	{
		case _DEF_LED1:
			LED1 ^= 1;
			break;
		case _DEF_LED2:
			LED2 ^= 1;
			break;
		
	}
}
void ledStatus(void)
{
	uint32_t interval,led_time;;
	if(gStatus.Start)
	{
		interval = millis() % 1000;
		if(interval > 500)ledOff(_DEF_LED2);
		else ledOn(_DEF_LED2);
	}
	else ledOn(_DEF_LED2);
	switch(gStatus.status)
	{
		case STATUS_IDLE:
			led_time = millis() % 1000;
			if(led_time > 500)ledOn(_DEF_LED1);
			else ledOff(_DEF_LED1);
			break;
		case STATUS_READY:
		case STATUS_ON:
			ledOn(_DEF_LED1);
			break;
		case STATUS_ERR:
			led_time = millis() % 100;
			if(led_time > 50)ledOn(_DEF_LED1);
			else ledOff(_DEF_LED1);
			break;
	}
}
