#include "global.h"

#include "adc.h"
#include "cli.h"
//#include "pwm.h"
//#include "pwm.h"
#include "swtimer.h"
#include "led.h"
#define ADC_MAX_CH 4
#ifdef _USE_HW_CLI
static void cliAdc(cli_args_t *args);
static uint8_t adc_ch;
typedef struct{
	uint8_t Cnt;
	uint16_t Temp;
}temp_tbl_t;
_AD gAD;
temp_tbl_t pAD[2];
uint8_t gTempCnt;
uint16_t gTemperature1[16],gTemperature2[16];
void adcISR(void *arg);
extern uint32_t uartPrintf(uint8_t ch, uint8_t *fmt, ...);
#endif
void adcInit(void)
{
	PCMD = 0x11110000;
	PDMD = 0x11000000;
	adc_ch = swTimerGetHandle();
	swtimerSet(adc_ch, 10, 1, adcISR, NULL);
	swtimerStart(adc_ch);
#ifdef _USE_HW_CLI
	cliAdd("adc", cliAdc);
#endif
}
void pTempSampling(void)
{
	if(pAD[0].Cnt != 0xff){
		gTemperature1[pAD[0].Cnt] = pAD[0].Temp;
		if(++pAD[0].Cnt == 16)	pAD[0].Cnt = 0xff;
	}
	if(pAD[1].Cnt != 0xff){
		gTemperature2[pAD[1].Cnt] = pAD[1].Temp;
		if(++pAD[1].Cnt >= 16)	pAD[1].Cnt = 0xff;
	}
}
void int_AD0(void)
{	
	uint16_t pTemp,pTemp2;
	AN0CTR1A = 0x7605;
	AN0CTR0 = 0x473;
	
	while(AN0CTR1A & 0x0001);
	pTemp = AN0BUF01;
//	if(pTemp >= 50){
	pAD[0].Temp = (pTemp + 258) * 1000 / 115;
/*	}
	else if(pTemp > 8){
		pAD[0].AD.Temp1 = pTemp *1000 / 25;
	}
	else pAD[0].AD.Temp1 = 0*/;
	AN1CTR1A = 0x4505;
//	AN1CUTA = 0x10;
	AN1CTR0 = 0x473;
	
	while(AN1CTR1A & 0x0001);
	
	pTemp2 = AN1BUF05 + pTemp;
	pAD[1].Temp = (pTemp2 + 203) * 1000 / 113;
	pTempSampling();
}
void adcRead(_AD *p_adc)
{
	uint32_t pVar= 0;
	uint8_t i;
	
	for(i=0;i!=16;i++)pVar += gTemperature1[i];
	p_adc->Temp1 = pVar >> 4;
	pAD[0].Cnt = 0;
	pVar = 0;
	for(i=0;i!=16;i++)pVar += gTemperature2[i];
	p_adc->Temp2 = pVar >> 4;
	pAD[1].Cnt = 0;
}

uint16_t adcReadCH(uint8_t ch)
{
	uint16_t ret = 0;
	
	switch(ch)
	{
		case 0:
			ret = AN0BUF00;
			break;
		case 1:
			ret = AN0BUF01;
			break;
		case 2:
			ret = AN0BUF02;
			break;
		case 3:
			ret = AN1BUF05;
			break;
	}
	return ret;
}
short adcReadVoltage(uint8_t ch)
{
	uint16_t ret = 0;
	switch(ch)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
	}
	return ret;
}

void adcISR(void *arg)
{
	int_AD0();
	
}
uint8_t adcCheck(_AD *p_obj)
{
	uint16_t data;
	uint8_t status = 0;
	
	data = p_obj->Temp1;
	if(data > 28000)status = STATUS_ERR;
	data = p_obj->Temp2;
	if(data > 28000)status = STATUS_ERR;
	if(status == STATUS_INIT)status = STATUS_IDLE;
	return status;
}
#ifdef _USE_HW_CLI
void cliAdc(cli_args_t *args)
{
  uint8_t ret = true,i;


  if (args->argc == 1)
  {
    if (args->isStr(0, "show") == true)
    {
      while(cliKeepLoop())
      {
        for (i=0; i<ADC_MAX_CH; i++)
        {
          uartPrintf(_DEF_SERIAL1, (uint8_t *)"%d ", adcReadCH(i));
        }
        uartPrintf(_DEF_SERIAL1, (uint8_t *)"\r\n");
        delay(50);
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (args->argc == 2)
  {
    if (args->isStr(0, "show") == true && args->isStr(1, "voltage") == true)
    {
      while(cliKeepLoop())
      {
        for (i=0; i<ADC_MAX_CH; i++)
        {
          uint32_t adc_data;

          adc_data = adcReadVoltage(i);

          uartPrintf(_DEF_SERIAL1, (uint8_t *)"%d ", adc_data);
        }
        uartPrintf(_DEF_SERIAL1, (uint8_t *)"\r\n");
        delay(50);
      }
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    uartPrintf(_DEF_SERIAL1, (uint8_t *)"adc show\n");
    uartPrintf(_DEF_SERIAL1, (uint8_t *)"adc show voltage\n");
  }
}
#endif
