#ifndef _SW_TIMER_H_
#define _SW_TIMER_H_


#include "hw_def.h"

#ifdef _USE_HW_SWTIMER

#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif

#ifndef NULL
#define NULL 0
#endif



#define SWTIMER_MAX_CH       HW_SWTIMER_MAX_CH


typedef enum
{
  ONE_TIME,
  LOOP_TIME
} swtimer_mode_t;


//typedef int16_t             swtimer_handle_t;



uint8_t swtimerInit(void);
void swtimerSet  (uint8_t TmrNum, uint32_t time_ms, swtimer_mode_t mode, void (*Fnct)(void *),void *arg);
void swtimerStart(uint8_t TmrNum);
void swtimerStop (uint8_t TmrNum);
void swtimerReset(uint8_t TmrNum);
void swtimerISR(void);


uint8_t  swtimerGetHandle(void);
uint32_t swtimerGetCounter(void);

#endif
#endif
