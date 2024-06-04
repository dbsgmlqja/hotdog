#include "global.h"
#include "MAX6675.h"
#include "gpio.h"
#define CS_TOP		((*(RB_BTF*)&P4OUT).p1)
#define CS_BOTTOM	((*(RB_BTF*)&P4OUT).p2)
#define CLK		((*(RB_BTF*)&P4OUT).p3)
#define MISO	((*(RB_BTF*)&P4IN).p0)
#ifndef ON
#define ON 1
#endif
// PID 제어를 위한 변수
double Kp = 1.0;         // P 값
double Ki = 0.1;         // I 값
double Kd = 0.01;        // D 값
const uint8_t numReadings = 8;
static uint16_t readings[2][8];
static uint8_t readIndex = 0;
static uint32_t total1 = 0;
static uint32_t total2 = 0;
#ifndef OFF
#define OFF 0
#endif
void MAX6675_Init(void)
{
	P4DIR |= 0x0E;
	P4IE |= 0x08;
	MISO = 1;
}
static void delayUsecond(uint32_t us)
{
	uint16_t i,j;
	for(j=0;j!=us;j++){
		for(i=0;i!=5;i++){
			clear_WDT();
		}
	}
}

uint16_t spiread(void)
{
	int8_t i;
	uint8_t d = 0;
	
	for(i=7;i>=0;i--) {
		CLK = 0;
		delayUsecond(10);
		if(MISO){
			d |= (1 << i);
		}
		CLK = 1;
		delayUsecond(10);
	}
	return d;
}

void readTemperature(void)
{
	static uint8_t pCnt1 = 0,pCnt2 = 0;
	uint16_t v = 0;
	
	CS_TOP = 0;
	delayUsecond(10);
	
	v = spiread();
	v <<= 8;
	v |= spiread();
	
	CS_TOP = 1;
	
	if(v & 0x04){
		if(++pCnt1 > 2){
			gStatus.status = STATUS_ERR;
			gStatus.Heater_Top.Temp = 0xffff;
		}
		return;
	}
	pCnt1 = 0;
	delayUsecond(100);
	v >>= 3;
	gStatus.Heater_Top.Temp = v * 25;
	/*total1 -= readings[0][readIndex];
	
	readings[0][readIndex] = v * 25;
	total1 += readings[0][readIndex];
	gStatus.Heater_Top.Temp = total1 >> 3;*/
	CS_BOTTOM = 0;
	delayUsecond(10);
	
	v = spiread();
	v <<= 8;
	v |= spiread();
	
	CS_BOTTOM = 1;
	if(v & 0x04){
		if(++pCnt2 > 2){
//			gStatus.status = STATUS_ERR;
			gStatus.Heater_Bottom.Temp = 0xffff;
		}
		return;
	}
	pCnt2 = 0;
	v >>= 3;
	gStatus.Heater_Bottom.Temp = v * 25;
	/*total2 -= readings[1][readIndex];
	
	readings[1][readIndex] = v * 25;
	total2 += readings[1][readIndex];
	gStatus.Heater_Bottom.Temp = total2 >> 3;
	
	if(++readIndex >= numReadings)readIndex = 0;*/
}

// PID 제어 함수
int pidControlTop(void)
{
	double output,setpoint,error,derivate;
	int num;
	if(gStatus.status < STATUS_IDLE)return 0;
	setpoint = gStatus.Heater_Top.Comp/100;
    error = setpoint - gStatus.Heater_Top.Temp/100;
    SSR_TOP.PID.integral += error;
	
	
	derivate = error - SSR_TOP.PID.prev_error;
    // PID 제어 계산
    output = Kp * error + Ki * SSR_TOP.PID.integral + Kd * derivate;
	num = (int)output*10;
    SSR_TOP.PID.prev_error = error;
		// 음수 출력이면 0으로 처리
    if (num < 50) {
        num = 0;
    }
	else if(num > 1000)num = 400;
	else num = num*400/1000;
    return (int)num;
}
int pidControlBottom(void)
{
	double output,setpoint,error,derivate;
	int num;
	if(gStatus.status < STATUS_IDLE)return 0;
	setpoint = gStatus.Heater_Bottom.Comp/100;
    error = setpoint - gStatus.Heater_Bottom.Temp/100;
    SSR_BOTTOM.PID.integral += error;
	
	
	derivate = error - SSR_BOTTOM.PID.prev_error;
    // PID 제어 계산
    output = Kp * error + Ki * SSR_BOTTOM.PID.integral + Kd * derivate;
	num = (int)output*10;
    SSR_BOTTOM.PID.prev_error = error;
		// 음수 출력이면 0으로 처리
    if (num < 50) {
        num = 0;
    }
	else if(num > 1000)num = 400;
	else num = num*400/1000;
    return (int)num;
}
void gSetStatus(void)
{
	uint16_t pTemp = 0;
	uint16_t comp,comp2;
	switch(gStatus.status)
	{
		case STATUS_IDLE:
			comp = gParameter.TopTemp * 0.8;
			comp2 = gParameter.BottomTemp * 0.8;
			if(gStatus.Heater_Top.Temp > comp)pTemp = 1;
			if(gStatus.Heater_Bottom.Temp > comp2)pTemp &= 1;
			if(pTemp)gStatus.status = STATUS_ON;
			break;
		case STATUS_READY:
		case STATUS_ON:
			comp = gParameter.TopTemp + 6000;
			comp2 = gParameter.BottomTemp + 6000;
//			if(gStatus.Heater_Top.Temp > comp)pTemp = 1;
			if(gStatus.Heater_Bottom.Temp > comp2)pTemp |= 1;
			if(pTemp){
				gStatus.status = STATUS_ERR;
				//TODO Error Action
				
				return;
			}
			comp = gParameter.TopTemp * 0.8;
			comp2 = gParameter.BottomTemp * 0.8;
//			if(gStatus.Heater_Top.Temp < comp)pTemp = 1;
			if(gStatus.Heater_Bottom.Temp < comp2)pTemp |= 1;
			if(pTemp){
				gStatus.status = STATUS_IDLE;
			}
			break;
	}
}
