#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "hw_def.h"

#define	IO_FREQ			(30000000)				/*Hz*/
#define MCLK_FREQ		(120000000)
#define	CARRIER_FREQ	(20000)					/*Hz*/
#define	PWM_VALUE		(MCLK_FREQ/CARRIER_FREQ)

void mototInit(void);
void gMotorCheck(void);
uint16_t gGetPwmValue(void);
void gMotorControl(uint16_t target);
void gResetMotorPid(void);
uint8_t getMotorStatus(void);
void setMotorStatus(uint8_t data);
#endif
