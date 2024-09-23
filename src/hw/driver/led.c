#include "global.h"
#include "led.h"
#define	LED1	((*(RB_BTF*)&P9OUT).p1)	//READY LED
#define	LED2	((*(RB_BTF*)&PCOUT).p4)	//Reverse LED
#define	LED3	((*(RB_BTF*)&PCOUT).p5)	//Convayor LED
#define	LED4	((*(RB_BTF*)&PCOUT).p6)	//Motor LED
#define	LED5	((*(RB_BTF*)&PCOUT).p7)	//Heater LED
#define	LED6	((*(RB_BTF*)&P6OUT).p4)	//Motor2 LED
#define	LED7	((*(RB_BTF*)&P6OUT).p0)	//AM LED
typedef struct
{
  uint8_t   on_state;
  uint8_t   off_state;
} led_tbl_t;
led_tbl_t led_tbl[LED_MAX_CH] =
{
    {0, 1}, // LED1
    {1, 0}, // LED2
    {1, 0}, // LED3
    {1, 0}, // LED4
    {1, 0}, // LED5
	{0, 1}, // LED6
	{0, 1}, // LED7
};
uint8_t ledInit(void)
{
	uint8_t i;
	uint8_t ret = 1;
	P5DIR |= 0x02;
	P6DIR |= 0x11;
	P9DIR |= 0x02;
	PCDIR |= 0xF0;
	for(i=0;i<LED_MAX_CH;i++)ledOff(i);
	ledOn(LED_READY);
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
		case _DEF_LED3:
			LED3 = led_tbl[ch].on_state;
			break;
		case _DEF_LED4:
			LED4 = led_tbl[ch].on_state;
			break;
		case _DEF_LED5:
			LED5 = led_tbl[ch].on_state;
			break;
		case _DEF_LED6:
			LED6 = led_tbl[ch].on_state;
			break;
		case _DEF_LED7:
			LED7 = led_tbl[ch].on_state;
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
		case _DEF_LED3:
			LED3 = led_tbl[ch].off_state;
			break;
		case _DEF_LED4:
			LED4 = led_tbl[ch].off_state;
			break;
		case _DEF_LED5:
			LED5 = led_tbl[ch].off_state;
			break;
		case _DEF_LED6:
			LED6 = led_tbl[ch].off_state;
			break;
		case _DEF_LED7:
			LED7 = led_tbl[ch].off_state;
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
		case _DEF_LED3:
			LED3 ^= 1;
			break;
		case _DEF_LED4:
			LED4 ^= 1;
			break;
		case _DEF_LED5:
			LED5 ^= 1;
			break;
		case _DEF_LED6:
			LED6 ^= 1;
			break;
		case _DEF_LED7:
			LED7 ^= 1;
			break;
	}
}
void ledStatus(void)
{
	uint32_t interval,led_time;;
	if(gError.bit.EmergencyOn)
	{
		interval = millis() % 300;
		if(interval > 150){
			ledOff(_DEF_LED1);
			ledOff(_DEF_LED2);
			ledOff(_DEF_LED3);
			ledOff(_DEF_LED4);
			ledOff(_DEF_LED5);
			ledOff(_DEF_LED6);
			ledOff(_DEF_LED7);
		}
		else{
			ledOn(_DEF_LED1);
			ledOn(_DEF_LED2);
			ledOn(_DEF_LED3);
			ledOn(_DEF_LED4);
			ledOn(_DEF_LED5);
			ledOn(_DEF_LED6);
			ledOn(_DEF_LED7);
		}
	}
	else{
		ledOn(_DEF_LED1);
		switch(gStatus.status)
		{
			case STATUS_IDLE:
				led_time = millis() % 1000;
				if(led_time > 500)ledOn(_DEF_LED5);
				else ledOff(_DEF_LED5);
				break;
			case STATUS_READY:
			case STATUS_ON:
				ledOn(_DEF_LED5);
				break;
			case STATUS_ERR:
				led_time = millis() % 200;
				if(led_time > 100)ledOn(_DEF_LED5);
				else ledOff(_DEF_LED5);
				break;
			default:
				ledOff(_DEF_LED5);
		}
		led_time = millis() - gSwitch.AutoPreTime;
		interval = millis() - gSwitch.InvPreTime;
		if((led_time > 2000) && (interval > 2000) && (gInverterStatus.bit.stop == 1))ledOff(LED_AM);
	}
}


