#include "global.h"
#include "buzzer.h"
#include "swtimer.h"
#define	BUZZER	((*(RB_BTF*)&P3OUT).p3)
#define BUZZER_PORT P3OUT
#define BUZZER_PIN 0x08
void buzzerISR(void *arg);
void buzzerShortISR(void *arg);
static uint8_t bzCntShort = 0;
static uint8_t bzCntLong = 0;
void buzzerInit(void)
{
	uint8_t bzCH;
	P3DIR |= 0x08;
	PCDIR |= 0x80;
	BUZZER = 0;
	bzCH = swTimerGetHandle();
	swtimerSet(bzCH, 500, 1, buzzerISR, NULL);
	swtimerStart(bzCH);
	bzCH = swTimerGetHandle();
	swtimerSet(bzCH, 100, 1, buzzerShortISR, NULL);
	swtimerStart(bzCH);
}

void pBeep(uint8_t Cnt, uint8_t sl)
{
	if(sl == 0){	//short
		bzCntShort += (Cnt * 2);
		if(bzCntShort > 20)bzCntShort = 20;
	}
	else{	//Long
		if(bzCntShort == 0)
		{
			bzCntLong += (Cnt * 2);
			if(bzCntLong > 20)bzCntLong = 20;
		}
	}
}
void buzzerISR(void *arg)
{
	if(bzCntShort == 0){
		
		if(bzCntLong > 0){
			BUZZER = ~(bzCntLong % 2);
			bzCntLong--;
		}
	}
}
void buzzerShortISR(void *arg)
{
	if(bzCntLong == 0){
		BUZZER = ~(bzCntShort % 2);
		if(bzCntShort > 0){
			bzCntShort--;
		}
		else BUZZER = 0;
	}
	else{
		if(bzCntShort > 0){
			bzCntLong = 0;
			BUZZER = bzCntShort % 2;
			if(bzCntShort > 0){
				bzCntShort--;
			}
			else BUZZER = 0;
		}
		else
		{
			if(bzCntLong == 0)BUZZER = 0;
		}
	}
	
}
#if(CUSTOMER == AJUN)
#define SBIT 7
#else
#define SBIT 4
#endif
// 부저 활성화/비활성화 함수
void buzzer_start(void)
{
    PCOUT |= (1 << SBIT);
}

void buzzer_stop(void)
{
    PCOUT &= ~(1 << SBIT);
//    PCOUT &= ~(1 << 7);
//    buzzerState = 0;
}
