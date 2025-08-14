#include "global.h"
#include "motor.h"
#include "gpio.h"
#include "Pid.h"
static _PIDS MOTOR_PID;
extern void buzzer_start(void);
extern void buzzer_stop(void);
extern void pStoreErrorMessage(uint8_t Msg);
void mototInit(void)
{
	
	// PWM Initialize

	P9DIR |= 0x01;
	P9MD |= 0x01;
//	AN0ST = 0x08;	//Trigger PWM1 Overflow interrupt

	
	
	//PWM1 Initialize
	/*
	 * 	stop PWM0 counter
	 */
	PWMMD1 &= ~0x0002;	/* stop PWM0 counter				*/

	/*	 * set duty	 */
	PWMSET1 = PWM_VALUE - 1;	/* carrier frequency(8kHz)			*/
	PWMCMP1A = 0;		/* V phase 50% duty (for sample)  		*/
	PWMDTMSET1 = 0;		/* set deadtime ((50*2 + 1)*50ns = 5.05us	*/
	PWMSEL1 = 0x0fff;	/* PWM non active				*/
	PWMOUTMD1 = 0x0001;	/* negative mode				*/

	PWMMD1 = 0x03c9;
	/* start AD convert at PWM0 counter underflow	*/
	/* OUTMD0,PWMSEL0 single buffer 		*/
	/* load double buffer at PWM0 counter underflow	*/
	/* disable interrupt neither overflow nor underflow */
	/* with deadtime				*/
	/* active low, saw tooth modulation(asymmetry)	*/
	/* stop PWM0 counter				*/
	/* Dead time insertion = 1(L active) */
	PWMOFF1A |= 0x0001;	
	PWMCMP1A = 0;//(PWM_VALUE  * 20 / 100) - 1;
	PWMSEL1 = 0x003f;
	PWMMD1 |= 0x0002;	/* start PWM1 counter				*/
	
	//FG Setting
	P6MD |= 0x0010;
	TM18MD1 = 0x0086; //0x0006-both edge, 0x0007-one edge
	TM18MD1 |= 0x0040;
	TM18MD1 &= ~0x0040;
	
	//PID Setting
	MOTOR_PID.P = 19.0;
	MOTOR_PID.I = 10.0;
	MOTOR_PID.D = 0.05;
	MOTOR_PID.basicLevel = (PWM_VALUE - 1);
	MOTOR_PID.setpoint = (PWM_VALUE - 1);
}
void gMotorCheck(void)
{
	short TargetSpeed;
	if(gError.All)return;
	switch(gMotorStatus)
	{
		case MAX_ZONE:
			if(millis() - gMotorTime > 3000)
			{
				TargetSpeed = (gIParameter.Freq * 45) * 0.7;
				if(gMotorSpeed < TargetSpeed)
				{
					if(gError.bit.Motor == 0)
					{
						gMotorStatus = MOTOR_OFF;
						ssrControl(SSR_SSR, OFF);
						P24RELAY = 0;
						gError.bit.Motor = 1;
						buzzer_start();
						pStoreErrorMessage(Error_Motor);
					}
				}
			}
			break;
		case MOTOR_OFF:
			if(millis() - gMotorTime > 3000)
			{
				if(gMotorSpeed > 100)
				{
					if(gError.bit.Motor == 0)
					{
						ssrControl(SSR_SSR, OFF);
						P24RELAY = 0;
						gError.bit.Motor = 1;
						buzzer_start();
						pStoreErrorMessage(Error_Motor);
					}
				}
			}
			break;
	}
}
uint16_t gGetPwmValue(void)
{
	return PWM_VALUE;
}
void gMotorControl(uint16_t target)
{
	uint16_t pVar;
	MOTOR_PID.setpoint = target;
	MOTOR_PID.measurepoint = gMotorSpeed;
	getPidControl(&MOTOR_PID);
	
	pVar = MOTOR_PID.value * 1.2;
	
	if(pVar < 0)PWMCMP1A = 0;
	else if(pVar < PWM_VALUE)PWMCMP1A = pVar;
	else PWMCMP1A = PWM_VALUE - 1;
}
void gResetMotorPid(void)
{
	MOTOR_PID.integral = 0;
	MOTOR_PID.prev_error = 0;
}
