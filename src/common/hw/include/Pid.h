#ifndef _PID_H_
#define _PID_H_
typedef struct{
	short measurepoint;
	unsigned short basicLevel;
	unsigned short value;
	double P;
	double I;
	double D;
	double setpoint;
	double integral;
	double prev_error;
}_PIDS;
void getPidControl(_PIDS *pid);
#endif
