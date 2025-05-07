#include "global.h"

#include "adc.h"
#include "cli.h"
//#include "pwm.h"
//#include "pwm.h"
#include "led.h"
#define ADC_MAX_CH 4
#ifdef _USE_HW_CLI
static void cliAdc(cli_args_t *args);
static uint8_t adc_ch;
typedef struct{
	uint8_t Cnt;
	uint16_t Temp;
}temp_tbl_t;
_AD gADC;
temp_tbl_t pAD[2];
uint8_t gTempCnt;
uint16_t gTemperature1[16],gTemperature2[16];
extern uint32_t uartPrintf(uint8_t ch, uint8_t *fmt, ...);
#endif
void adcInit(void)
{
	PCMD = 0x00000001;
//	PDMD = 0x11000000;
//	adc_ch = swTimerGetHandle();
//	swtimerSet(adc_ch, 10, 1, adcISR, NULL);
//	swtimerStart(adc_ch);
	AN0CTR0 = 0x0519;
	AN0CTR1A = 0x2006;
	
//	PDMD |= 0x10000000;
	
	
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
void adcRead(void)
{
	uint32_t pVar= 0;
	uint8_t i;
	
	for(i=0;i!=16;i++)pVar += gTemperature1[i];
	gADC.Temp1 = pVar >> 4;
	pAD[0].Cnt = 0;
	pVar = 0;
	for(i=0;i!=16;i++)pVar += gTemperature2[i];
	gADC.Temp2 = pVar >> 4;
	pAD[1].Cnt = 0;
}
void adc0Read(void)
{
	AN0CTR1A = 0x0001;
	AN0CTR0  = 0x047B;
	
	while(AN0CTR1A & 0x0001);
	gKey_AD0 = AN0BUF00;
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
			ret = AN1BUF04;
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

uint8_t adcCheck(void)
{
	uint16_t data;
	uint8_t status = 0;
	
	data = gStatus.KSensor_Top.Temp;
	if(data > 23600){
		status = STATUS_ERR;
	}
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
