#include "Pid.h"

void getPidControl(_PIDS *pid)
{
	double output,setpoint,error,derivate;
	int num;
	setpoint = pid->setpoint;
    error = (setpoint - pid->measurepoint)/100;
	if(pid->value < pid->basicLevel)
	{
		pid->integral += error;
	}
	
	derivate = error - pid->prev_error;
    // PID 제어 계산
    output = pid->P * error + pid->I * pid->integral + pid->D * derivate;
	num = (int)output;
    pid->prev_error = error;
	// 음수 출력이면 0으로 처리
	if(num < 2)pid->value = 0;	
    else if (num > pid->basicLevel) {
        pid->value = pid->basicLevel;
    }
	else pid->value = num;
}
