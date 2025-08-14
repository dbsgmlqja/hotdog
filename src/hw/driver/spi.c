#include "spi.h"
#include "hx711.h"
#include "global.h"
#define offst 8389853.16
#define C1 0.0000255417
#define C2  4.26637378
#define  p1  2.9248E-09
#define  p2  -0.0000013962
#define  p3  0.0013477
#define  p4  2.3141
#define  p5  -243.85
HX711_ADC PT100_TOP;
HX711_ADC PT100_BOTTOM;
_SPI spiTopSck;
_SPI spiTopDout;
_SPI spiBottomSck;
_SPI spiBottomDout;
void spiInit(void)
{
	spiTopSck.Dir = (uint8_t *)&P4DIR;
	spiTopSck.Out = (uint8_t *)&P4OUT;
	spiTopSck.pin = 0x04;
	spiTopDout.Dir = (uint8_t *)&P4DIR;
	spiTopDout.In = (uint8_t *)&P4IN;
	spiTopDout.pin = 0x01;
	spiBottomSck.Dir = (uint8_t *)&P4DIR;
	spiBottomSck.Out = (uint8_t *)&P4OUT;
	spiBottomSck.pin = 0x08;
	spiBottomDout.Dir = (uint8_t *)&P4DIR;
	spiBottomDout.In = (uint8_t *)&P4IN;
	spiBottomDout.pin = 0x02;
	HX711_ADC_Init((HX711_ADC*)&PT100_TOP,spiTopSck,spiTopDout);
	HX711_ADC_Init((HX711_ADC*)&PT100_BOTTOM,spiBottomSck, spiBottomDout);
}
double m_pow(double n, double m)
{
    double nn = n;
	int i;
	i = m;
	if (i == 0)       return 1.0;
    for (i = 0; i < (int)(m-1); i++)        n *= nn;
    return n;
}
void updateSpi(void)
{
	static uint32_t pre_time = 0,pre_time2 = 0;
	uint8_t readyTop, readyBottom = 0;
	float R;
	float T;

	float i;
	readyTop = HX711_ADC_update((HX711_ADC *)&PT100_TOP);
	readyBottom = HX711_ADC_update((HX711_ADC *)&PT100_BOTTOM);
	if(readyTop)
	{
		if (millis() - pre_time > 1500) {
		
			i = HX711_ADC_getData((HX711_ADC *)&PT100_TOP);  //ADC value
			R=(i-offst)*C1+C2;  //resistance calibration
			T = p1*m_pow(R,4) + p2*m_pow(R,3) + p3*m_pow(R,2) + p4*R +p5; //T(R) parametrization
			gStatus.KSensor_Top.Temp = (uint16_t)(T * 100);
			readyTop = 0;
			pre_time = millis();
	    }
	}
	if(readyBottom)
	{
		if (millis() > pre_time2 + 1000) {
		
			i = HX711_ADC_getData((HX711_ADC *)&PT100_BOTTOM);  //ADC value
			R=(i-offst)*C1+C2;  //resistance calibration
			T = p1*m_pow(R,4) + p2*m_pow(R,3) + p3*m_pow(R,2) + p4*R +p5; //T(R) parametrization
			
			
			gStatus.KSensor_Bottom.Temp = T*100;
			gStatus.KSensor_Top.Temp = gStatus.KSensor_Bottom.Temp;
			readyBottom = 0;
			pre_time2 = millis();
	    }
	}
}
float getSpiData(uint8_t ch)
{
	if(ch == 1)return HX711_ADC_getData((HX711_ADC *)&PT100_BOTTOM);
	else return HX711_ADC_getData((HX711_ADC *)&PT100_TOP);
}
