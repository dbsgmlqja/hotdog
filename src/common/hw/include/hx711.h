#ifndef HX711_H
#define HX711_H

#include "hw_def.h"

// Configuration
#define SAMPLES 8
#define IGN_HIGH_SAMPLE 1
#define IGN_LOW_SAMPLE 1
#define DATA_SET 8
#define SIGNAL_TIMEOUT 300
#define SCK_DISABLE_INTERRUPTS 1
#define SCK_DELAY 5
typedef struct{
	volatile uint8_t *Dir;
	volatile uint8_t *In;
	volatile uint8_t *Out;
	uint8_t pin;
}_SPI;
typedef struct {
	_SPI spiSck;
	_SPI spiDout;
    uint8_t GAIN;
    float calFactor;
    float calFactorRecip;
    int tareOffset;
    uint32_t lastDoutLowTime;
    uint32_t conversionStartTime;
    float conversionTime;
    uint8_t tareStatus;
    uint8_t tareTimeoutFlag;
    uint8_t tareTimeoutDisable;
    uint8_t signalTimeoutFlag;
    uint8_t reverseVal;
    int dataSampleSet[SAMPLES + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE];
    uint16_t readIndex;
    uint8_t doTare;
    uint8_t convRslt;
    uint8_t tareTimes;
    uint8_t dataWaiting;
    uint8_t dataOutOfRange;
    short samplesInUse;
    uint8_t divBit;
    uint8_t isFirst;
    uint8_t startStatus;
    uint32_t startMultipleTimeStamp;
    uint32_t startMultipleWaitTime;
    int32_t lastSmoothedData;
} HX711_ADC;

// Function declarations
void HX711_ADC_Init(HX711_ADC* adc, _SPI spiSck, _SPI spiDout);
void HX711_ADC_setGain(HX711_ADC* adc, uint8_t gain);
void HX711_ADC_begin(HX711_ADC* adc);
void HX711_ADC_beginWithGain(HX711_ADC* adc, uint8_t gain);
void HX711_ADC_start(HX711_ADC* adc, uint32_t t);
void HX711_ADC_startWithTare(HX711_ADC* adc, uint32_t t, uint8_t dotare);
int HX711_ADC_startMultiple(HX711_ADC* adc, uint32_t t);
int HX711_ADC_startMultipleWithTare(HX711_ADC* adc, uint32_t t, uint8_t dotare);
void HX711_ADC_tare(HX711_ADC* adc);
void HX711_ADC_tareNoDelay(HX711_ADC* adc);
void HX711_ADC_setCalFactor(HX711_ADC* adc, float cal);
uint8_t HX711_ADC_getTareStatus(HX711_ADC* adc);
float HX711_ADC_getCalFactor(HX711_ADC* adc);
uint8_t HX711_ADC_update(HX711_ADC* adc);
uint8_t HX711_ADC_dataWaitingAsync(HX711_ADC* adc);
uint8_t HX711_ADC_updateAsync(HX711_ADC* adc);
float HX711_ADC_getData(HX711_ADC* adc);
int HX711_ADC_smoothedData(HX711_ADC* adc);
void HX711_ADC_conversion24bit(HX711_ADC* adc);
void HX711_ADC_powerDown(HX711_ADC* adc);
void HX711_ADC_powerUp(HX711_ADC* adc);
int HX711_ADC_getTareOffset(HX711_ADC* adc);
void HX711_ADC_setTareOffset(HX711_ADC* adc, int newoffset);
int HX711_ADC_getReadIndex(HX711_ADC* adc);
float HX711_ADC_getConversionTime(HX711_ADC* adc);
float HX711_ADC_getSPS(HX711_ADC* adc);
uint8_t HX711_ADC_getTareTimeoutFlag(HX711_ADC* adc);
void HX711_ADC_disableTareTimeout(HX711_ADC* adc);
int HX711_ADC_getSettlingTime(HX711_ADC* adc);
void HX711_ADC_setSamplesInUse(HX711_ADC* adc, int samples);
int HX711_ADC_getSamplesInUse(HX711_ADC* adc);
void HX711_ADC_resetSamplesIndex(HX711_ADC* adc);
uint8_t HX711_ADC_refreshDataSet(HX711_ADC* adc);
uint8_t HX711_ADC_getDataSetStatus(HX711_ADC* adc);
float HX711_ADC_getNewCalibration(HX711_ADC* adc, float known_mass);
uint8_t HX711_ADC_getSignalTimeoutFlag(HX711_ADC* adc);
void HX711_ADC_setReverseOutput(HX711_ADC* adc);

#endif
