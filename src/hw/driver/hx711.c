
// hx711.c
#include "hx711.h"
extern void enable_irq(void);
extern void disable_irq(void);


void delayMicroseconds(uint32_t t)
{
	uint32_t i,j;
	for(i=0;i<t;i++){
		for(j=0;j<6;j++){
			asm(" NOP");
		}
		clear_WDT();
	}
}
void HX711_ADC_Init(HX711_ADC* adc, _SPI spiSck, _SPI spiDout) {
	int i;
    adc->spiSck = spiSck;
	adc->spiDout = spiDout;
	*adc->spiSck.Dir |= adc->spiSck.pin;
	*adc->spiDout.Dir &= ~adc->spiDout.pin;
    adc->GAIN = 3;
    adc->calFactor = 1.0f;
    adc->calFactorRecip = 1.0f;//1.035f;
    adc->tareOffset = 0;
    adc->lastDoutLowTime = 0;
    adc->conversionStartTime = 0;
    adc->conversionTime = 0;
    adc->tareStatus = false;
    adc->tareTimeoutFlag = 0;
    adc->tareTimeoutDisable = false;
    adc->signalTimeoutFlag = false;
    adc->reverseVal = false;
    adc->readIndex = 0;
    adc->doTare = 0;
    adc->convRslt = 0;
    adc->tareTimes = 0;
    adc->dataWaiting = false;
    adc->dataOutOfRange = 0;
    adc->samplesInUse = SAMPLES;
    adc->divBit = 3;
    adc->isFirst = 1;
    adc->startStatus = 0;
    adc->startMultipleTimeStamp = 0;
    adc->startMultipleWaitTime = 0;
    adc->lastSmoothedData = 0;
    
    for(i = 0; i < SAMPLES + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE; i++) {
        adc->dataSampleSet[i] = 0;
    }
}

void HX711_ADC_setGain(HX711_ADC* adc, uint8_t gain) {
    if(gain < 64) adc->GAIN = 2;      // 32, channel B
    else if(gain < 128) adc->GAIN = 3; // 64, channel A
    else adc->GAIN = 1;                // 128, channel A
}

void HX711_ADC_begin(HX711_ADC* adc) {
	*adc->spiSck.Dir |= adc->spiSck.pin;
	*adc->spiDout.Dir &= ~adc->spiDout.pin;
    HX711_ADC_setGain(adc, 128);
    HX711_ADC_powerUp(adc);
}

void HX711_ADC_beginWithGain(HX711_ADC* adc, uint8_t gain) {
    *adc->spiSck.Dir |= adc->spiSck.pin;
	*adc->spiDout.Dir &= ~adc->spiDout.pin;
    HX711_ADC_setGain(adc, gain);
    HX711_ADC_powerUp(adc);
}

void HX711_ADC_start(HX711_ADC* adc, uint32_t t) {
    t += 400;
    adc->lastDoutLowTime = millis();
    while(millis() < t) {
        HX711_ADC_update(adc);
        clear_WDT();
    }
    HX711_ADC_tare(adc);
    adc->tareStatus = 0;
}

void HX711_ADC_startWithTare(HX711_ADC* adc, uint32_t t, uint8_t dotare) {
    t += 400;
    adc->lastDoutLowTime = millis();
    while(millis() < t) {
        HX711_ADC_update(adc);
        clear_WDT();
    }
    if (dotare) {
        HX711_ADC_tare(adc);
        adc->tareStatus = 0;
    }
}

void HX711_ADC_conversion24bit(HX711_ADC* adc) {
	uint8_t i;
  
    uint32_t data = 0;
    uint8_t dout;
    adc->convRslt = 0;
    
#if(SCK_DISABLE_INTERRUPTS == 1)
//	disable_irq();
#endif
    for (i = 0; i < (24 + adc->GAIN); i++) {
        *adc->spiSck.Out |= adc->spiSck.pin;
#if(SCK_DELAY > 0)
	delayMicroseconds(1);
#endif
        *adc->spiSck.Out &= ~adc->spiSck.pin;
		
		delayMicroseconds(1);
        if (i < 24) {
			data <<= 1;
            dout = *adc->spiDout.In & adc->spiDout.pin ? 1 : 0;
            data |= dout;
        } else {
#if(SCK_DELAY > 0)
//	delayMicroseconds(500);
#endif
        }
    }
	
#if(SCK_DISABLE_INTERRUPTS) 
//	enable_irq();
#endif
    data = data ^ 0x800000;
    
    if (data > 0xFFFFFF) {
        adc->dataOutOfRange = 1;
    }
    if (adc->reverseVal) {
        data = 0xFFFFFF - data;
    }
    if (adc->readIndex == adc->samplesInUse + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE - 1) {
        adc->readIndex = 0;
    }
    else {
        adc->readIndex++;
    }
    if(data > 0) {
        adc->convRslt++;
        adc->dataSampleSet[adc->readIndex] = (int)data;
        if(adc->doTare) {
            if (adc->tareTimes < DATA_SET) {
                adc->tareTimes++;
            }
            else {
                adc->tareOffset = HX711_ADC_smoothedData(adc);
                adc->tareTimes = 0;
                adc->doTare = 0;
                adc->tareStatus = 1;
                adc->convRslt++;
            }
        }
    }
}

float HX711_ADC_getData(HX711_ADC* adc) {
    int data = 0;
	float x;
    adc->lastSmoothedData = HX711_ADC_smoothedData(adc);
    data = adc->lastSmoothedData - adc->tareOffset;
    x = (float)data * adc->calFactorRecip;
    return x;
}

int HX711_ADC_smoothedData(HX711_ADC* adc) {
	uint8_t r;
    int data = 0;
    int L = 0xFFFFFF;
    int H = 0x00;
    
    for (r = 0; r < (adc->samplesInUse + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE); r++) {
        #if IGN_LOW_SAMPLE
        if (L > adc->dataSampleSet[r]) L = adc->dataSampleSet[r];
        #endif
        #if IGN_HIGH_SAMPLE
        if (H < adc->dataSampleSet[r]) H = adc->dataSampleSet[r];
        #endif
        data += adc->dataSampleSet[r];
    }
    
    #if IGN_LOW_SAMPLE 
    data -= L;
    #endif
    #if IGN_HIGH_SAMPLE 
    data -= H;
    #endif
    
    return (data >> adc->divBit);
}

uint8_t HX711_ADC_update(HX711_ADC* adc) {
    uint8_t dout = *adc->spiDout.In & adc->spiDout.pin;
    if (!dout) {
        HX711_ADC_conversion24bit(adc);
        adc->lastDoutLowTime = millis();
        adc->signalTimeoutFlag = 0;
    }
    else {
        if (millis() - adc->lastDoutLowTime > SIGNAL_TIMEOUT) {
            adc->signalTimeoutFlag = 1;
        }
        adc->convRslt = 0;
    }
    return adc->convRslt;
}

void HX711_ADC_tare(HX711_ADC* adc) {
    uint8_t rdy = 0;
	uint32_t timeout;
    adc->doTare = 1;
    adc->tareTimes = 0;
    adc->tareTimeoutFlag = 0;
    timeout = millis() + 300;  // tareTimeOut = 300
    
    while(rdy != 2) {
        rdy = HX711_ADC_update(adc);
        if (!adc->tareTimeoutDisable) {
            if (millis() > timeout) { 
                adc->tareTimeoutFlag = 1;
                break;
            }
        }
        clear_WDT();
    }
}

void HX711_ADC_setCalFactor(HX711_ADC* adc, float cal) {
    adc->calFactor = cal;
    adc->calFactorRecip = 1/cal;
}

float HX711_ADC_getCalFactor(HX711_ADC* adc) {
    return adc->calFactor;
}

void HX711_ADC_powerDown(HX711_ADC* adc) {
    *adc->spiSck.Out &= ~adc->spiSck.pin;
    *adc->spiSck.Out |= adc->spiSck.pin;
}

void HX711_ADC_powerUp(HX711_ADC* adc) {
    *adc->spiSck.Out &= ~adc->spiSck.pin;
}
