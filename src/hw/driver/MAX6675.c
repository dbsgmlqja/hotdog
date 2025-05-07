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
extern void pStoreErrorMessage(uint8_t Msg);
// PID 제어를 위한 변수
double Kp = 14.8;         // P 값
double Ki = 0.2;         // I 값
double Kd = 5;        // D 값
const uint8_t numReadings = 8;
/*static uint16_t readings[2][8];
static uint8_t readIndex = 0;
static uint32_t total1 = 0;
static uint32_t total2 = 0;*/
#ifndef OFF
#define OFF 0
#endif
_TempArr pTempArrTop;
_TempArr pTempArrBottom;
typedef struct{
	uint8_t cnt;
	uint16_t value;
}_Value;
_Value findMostFrequent(uint16_t arr[], uint8_t size) {
	uint8_t i,j;
	_Value pValue;
    int maxCount = 0;  // 가장 높은 빈도 수
    int mostFrequent = arr[0];  // 가장 자주 등장하는 숫자
    int count;

    // 배열의 각 요소에 대해 빈도를 계산합니다.
    for (i = 0; i < size; i++) {
        count = 1;  // 현재 요소를 기준으로 빈도 카운트
        for (j = i + 1; j < size; j++) {
            if (arr[i] == arr[j]) {
                count++;  // 동일한 요소가 있으면 카운트 증가
            }
        }

        // 최대 빈도보다 더 많이 나온 숫자가 있으면 업데이트
        if (count > maxCount) {
            maxCount = count;
            mostFrequent = arr[i];
			pValue.cnt = count;
			pValue.value = arr[i];
        }
    }

    return pValue;
}
extern void pStoreErrorMessage(uint8_t ErroMsg);
extern void ssrOff(void);
uint16_t findClosest(uint16_t arr[], uint8_t size, uint16_t target) {
	uint8_t i;
    uint16_t closest = arr[0];  // 초기값을 배열의 첫 번째 값으로 설정
    int minDifference;
	int currentDifference;
	if(arr[0] >= target)minDifference = arr[0] - target;  // 첫 번째 요소와 타겟 값의 차이
	else minDifference = target - arr[0];

    // 배열의 나머지 요소들에 대해 차이를 계산
    for (i = 1; i < size; i++) {
		if(arr[i] >= target) currentDifference = arr[i] - target;  // 현재 값과 타겟 값의 차이 계산
		else currentDifference = target - arr[i];  // 현재 값과 타겟 값의 차이 계산

        // 만약 현재 차이가 최소 차이보다 작다면, 갱신
        if (currentDifference < minDifference) {
            minDifference = currentDifference;
            closest = arr[i];  // 가장 가까운 값 갱신
        }
    }

    return closest;  // 가장 가까운 값 반환
}
uint16_t pGetMinMax(_TempArr pArr)
{
	uint8_t i;
	uint16_t Min,Max;
	Min = pArr.Arr[0];
	Max = pArr.Arr[0];
	for(i=1;i!=5;i++)
	{
		if(pArr.Arr[i] < Min)Min = pArr.Arr[i];
		if(pArr.Arr[i] > Max)Max = pArr.Arr[i];
	}
	return Max - Min;
}
void MAX6675_Init(void)
{
	uint8_t i;
	uint16_t pTemp;
	uint32_t pTotal = 0,pTotal2 = 0;
	_Value pVal;
	P4DIR |= 0x0E;
	P4IE |= 0x08;
	MISO = 1;
	clear_WDT();
	
	for(i=0;i!=5;i++)
	{
		readTemperature();
		pTempArrTop.Arr[pTempArrTop.index++] = gStatus.KSensor_Top.Temp2;
		pTempArrBottom.Arr[pTempArrBottom.index++] = gStatus.KSensor_Bottom.Temp2;
		pTotal += gStatus.KSensor_Top.Temp2;
		pTotal2 += gStatus.KSensor_Bottom.Temp2;
		clear_WDT();
	}
	if(pGetMinMax(pTempArrTop) < 300)pTempArrTop.prev_temp = pTotal / 5;
	else
	{
		pVal = findMostFrequent(pTempArrTop.Arr, 5);
		if(pVal.cnt >= 3)pTempArrTop.prev_temp = pVal.value;
		else
		{
			pTempArrTop.prev_temp = findClosest(pTempArrTop.Arr, 5, pTotal/5);
		}
	}
	pTemp = pGetMinMax(pTempArrBottom);
	if(pTemp < 300)pTempArrBottom.prev_temp = pTotal / 5;
	else
	{
		pVal = findMostFrequent(pTempArrBottom.Arr, 5);
		if(pVal.cnt >= 3)pTempArrBottom.prev_temp = pVal.value;
		else
		{
			pTempArrBottom.prev_temp = findClosest(pTempArrBottom.Arr, 5, pTotal2/5);
		}
	}
	pTempArrTop.index = 0;
	pTempArrBottom.index = 0;
	gStatus.KSensor_Top.Temp = pTempArrTop.prev_temp;
	gStatus.KSensor_Bottom.Temp = pTempArrBottom.prev_temp;
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
			if(gError.bit.TopSensor == 0)
			{
				errorProccess();
				gError.bit.TopSensor = 1;
				pStoreErrorMessage(Error_TopSensor);
			}
			pCnt1 = 0;
			gStatus.KSensor_Top.Temp = 0xffff;
		}
		return;
	}
	delayUsecond(100);
	v >>= 3;
	gStatus.KSensor_Top.Temp2 = v * 25;

	CS_BOTTOM = 0;
	delayUsecond(10);
	
	v = spiread();
	v <<= 8;
	v |= spiread();
	CS_BOTTOM = 1;
	if(v & 0x04){
		if(++pCnt2 > 2){
			gStatus.status = STATUS_ERR;
			if(gError.bit.BottomSensor == 0)
			{
				errorProccess();
				gError.bit.BottomSensor = 1;
				pStoreErrorMessage(Error_BottomSensor);
			}
			gStatus.KSensor_Bottom.Temp = 0xffff;
			pCnt2 = 0;
		}
		return;
	}
	v >>= 3;
	gStatus.KSensor_Bottom.Temp = v * 25;

	/*total2 -= readings[1][readIndex];
	
	readings[1][readIndex] = v * 25;
	total2 += readings[1][readIndex];
	gStatus.KSensor_Bottom.Temp = total2 >> 3;
	
	if(++readIndex >= numReadings)readIndex = 0;*/
}

double myFabs(double n)
{
	int var = n;
	if(var > 0)
		return n;
	else
		return n * (-1);
}
static int pDispVar = 0;
static int pDispVar2 = 0;
// PID 제어 함수
int pidControlTop(void)
{
	double output,setpoint,error,derivate;
	int num;
	if(gStatus.status < STATUS_IDLE)return 0;
	setpoint = gStatus.Comp;
    error = (setpoint - gStatus.KSensor_Top.Temp)/100;
	pDispVar2 = (int)error;
	SSR.PID.integral += error;
	num = SSR.PID.integral;
		
	if(gStatus.Comp > gStatus.KSensor_Top.Temp)
	{
		if(num < 20)SSR.PID.integral = 20;
		else if(num > 50)SSR.PID.integral = 50;
	}
	else
	{
		if(num > 40)SSR.PID.integral = 40;
		else if(num < 30)SSR.PID.integral = 30;
	}
	derivate = error - SSR.PID.prev_error;
    // PID 제어 계산
    output = Kp * error + Ki * SSR.PID.integral + Kd * derivate;
	num = (int)output*10;
	pDispVar = num;
    SSR.PID.prev_error = error;
	// 음수 출력이면 0으로 처리
	if(num < 2)return 0;	
    if (num > 100) {
        num = 100;
    }
	num = num * 420 / 100;
    return (int)num;
}
void gSetStatus(void)
{
	uint16_t pTemp = 0;
	uint16_t comp,comp2;
	switch(gStatus.status)
	{
		case STATUS_IDLE:
			if(gStatus.warmupComplete == 1)
			{
				comp = gParameter.Temperature * 0.8;
				comp2 = gParameter.Temperature * 0.8;
			
				if(gStatus.KSensor_Top.Temp > comp)pTemp = 1;
				if(gStatus.KSensor_Bottom.Temp > comp2)pTemp &= 1;
				if(pTemp)gStatus.status = STATUS_ON;
			}
			break;
		case STATUS_READY:
		case STATUS_ON:
			comp = gParameter.Temperature + 3000;
			comp2 = gParameter.Temperature + 3000;
			if(gStatus.KSensor_Top.Temp > comp)pTemp = 1;
			if(gStatus.KSensor_Bottom.Temp > comp2)pTemp |= 1;
			if(pTemp){
				gStatus.status = STATUS_ERR;
				//TODO Error Action
				gError.bit.OverTemp = 1;
				ssrOff();
				pStoreErrorMessage(Error_Over_Temp);
				return;
			}
			comp = gParameter.Temperature * 0.8;
			comp2 = gParameter.Temperature * 0.8;
//			if(gStatus.KSensor_Top.Temp < comp)pTemp = 1;
			if(gStatus.KSensor_Bottom.Temp < comp2)pTemp |= 1;
			if(pTemp){
				gStatus.status = STATUS_IDLE;
			}
			break;
	}
	if(gStatus.warmupArrived == 0){
		comp = gParameter.Temperature * 0.8;
		comp2 = gParameter.Temperature * 0.8;
	
		if(gStatus.KSensor_Top.Temp > comp)pTemp = 1;
		if(gStatus.KSensor_Bottom.Temp > comp2)pTemp &= 1;
		if(pTemp)
		{
			gStatus.warmupArrived = 1;
			gStatus.warmupComplete = 1;
			gStatus.warmupTimer = gWarmupTimer;
		}
	}
}
static short TempArr[256] = {0};
static uint8_t tIdx = 0;
void pSetTemperature(void)
{
	static uint8_t num = 0;
	uint8_t i,j = 0;
	uint16_t val,basic;
	
	switch(num)
	{
		case 0:
		case 1:
		case 2:
			num++;
			break;
		case 3:
			readTemperature();
			pTempArrTop.Rec[0] = gStatus.KSensor_Top.Temp2;
			pTempArrBottom.Rec[0] = gStatus.KSensor_Bottom.Temp2;
			num++;
			j = 1;
			break;
		default:
			readTemperature();
			j = 1;
			pTempArrTop.Rec[1] = gStatus.KSensor_Top.Temp2;
			pTempArrBottom.Rec[1] = gStatus.KSensor_Bottom.Temp2;
			pTempArrTop.index = 0;
			pTempArrBottom.index = 0;
			for(i=0;i!=2;i++)
			{
				if(pTempArrTop.prev_temp <= pTempArrTop.Rec[i])
				{
					val = pTempArrTop.Rec[i] - pTempArrTop.prev_temp;
				}
				else
				{
					val = pTempArrTop.prev_temp - pTempArrTop.Rec[i];
				}
				basic = pTempArrTop.prev_temp * -178 / 10000 + 1035;
				if(val < basic){
					pTempArrTop.Arr[pTempArrTop.index++] = pTempArrTop.Rec[i];
				}
				if(pTempArrBottom.prev_temp <= pTempArrBottom.Rec[i])
				{
					val = pTempArrBottom.Rec[i] - pTempArrBottom.prev_temp;
				}
				else
				{
					val = pTempArrBottom.prev_temp - pTempArrBottom.Rec[i];
				}
				basic = pTempArrBottom.Rec[i] * -178 / 10000 + 1035;
				if(val < basic)pTempArrBottom.Arr[pTempArrBottom.index++] = pTempArrBottom.Rec[i];
			}
			if(i == 2)
			{
				pTempArrTop.prev_temp = pTempArrTop.Rec[1];
				gStatus.KSensor_Top.Temp = pTempArrTop.Rec[1];
				pTempArrBottom.prev_temp = pTempArrBottom.Rec[1];
				gStatus.KSensor_Bottom.Temp = pTempArrBottom.Rec[1];
			}
			else if(i == 1)
			{
				pTempArrTop.prev_temp = pTempArrTop.Arr[0];
				pTempArrBottom.prev_temp = pTempArrBottom.Arr[0];
			}
			else
			{
				pTempArrTop.prev_temp = (pTempArrTop.Rec[0] + pTempArrTop.Rec[1]) / 2;
				pTempArrBottom.prev_temp = (pTempArrBottom.Rec[0] + pTempArrBottom.Rec[1]) / 2;
			}
			num = 0;
			break;
	}
	if(j == 1)
	{
		for(i=0;i<0xff;i++)
		{
			TempArr[i] = TempArr[i+1];
		}
		TempArr[i] = gStatus.KSensor_Top.Temp;
	}
}
