/* 호스트 테스트용 스텁. modbus.c가 참조하는 하드웨어 의존 심볼을 대신 정의한다.
 * _HW_MAIN_ 을 정의하면 global.h가 전역 변수(gError, gModBus, gStatus 등)를 실제로 만든다. */
#define _HW_MAIN_
#include "hw_def.h"
#include "global.h"

void buzzer_start(void) {}
void clear_WDT(void) {}
void delay(uint32_t ms) { (void)ms; }
uint32_t millis(void) { return 0; }
void pStoreErrorMessage(uint8_t msg) { (void)msg; }
void setSSRError(void) {}
void setSSROutVoltage(void) {}
void setSSROutput(void) {}
void setSSRTemperature(void) {}
void setSSRVoltage(void) {}
void ssrControl(void) {}
uint32_t uartWrite(uint8_t ch, uint8_t data) { (void)ch; (void)data; return 1; }
