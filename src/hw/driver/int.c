#include "global.h"
#include "int.h"


void ext_int(unsigned char pos)
{

	unsigned char OldMode,OldZone,TempMode,NewPos;
	unsigned char *Ptr;



	if(gMotorStatus == MOTOR_OFF){
		return;
	}

	OldZone = gMotorStatus;
	OldMode = gOpMode;

	/* if half mode, half clear */
	if(pos == 3){
		MotorStatus.HalfStart = 0;
	}



	//Prevent to chattering
	if(MotorStatus.Direction == DIR_DOWN)	NewPos = pos | 0x80;
	else									NewPos = pos;
	if(gOldPos == NewPos){
		return;
	}
	if(MotorStatus.Direction == DIR_DOWN)	gOldPos = pos | 0x80;
	else									gOldPos = pos;


	/* Forward Direction */
	if(MotorStatus.Direction == DIR_DOWN)	/*Motor Up*/
	{
		if(MotorStatus.SlowMode){
			if(gOption.LimitType == 0)Ptr = (unsigned char *)&Abnormal_NextSensor_FWD[OldMode][pos][0];
			else Ptr = (unsigned char *)&Abnormal_NextSensor_FWD2[OldMode][pos][0];
		}
		else{
			if(gOption.LimitType == 0)Ptr = (unsigned char *)&NextSensor_FWD[OldMode][pos][0];
			else Ptr = (unsigned char *)&NextSensor_FWD2[OldMode][pos][0];
		}
	}
	/* Reverse Direction */
	else{									/*Motor Down*/
		if(MotorStatus.SlowMode){
				if(gOption.LimitType == 0)Ptr = (unsigned char *)&Abnormal_NextSensor_RVS[OldMode][pos][0];
				else Ptr = (unsigned char *)&Abnormal_NextSensor_RVS2[OldMode][pos][0];
		}
		else{
				if(gOption.LimitType == 0)Ptr = (unsigned char *)&NextSensor_RVS[OldMode][pos][0];
				else Ptr = (unsigned char *)&NextSensor_RVS2[OldMode][pos][0];
		}
	}
	
	Sensor.AllData = *Ptr++;
	gOpMode = *Ptr++;
	MotorStatus.EEP_WriteReq = 1;


	if(*Ptr != CANCEL_MODE)
	{
		TempMode = *Ptr;
		if(TempMode != OldZone)
		{
			/* Setup Mode : Cancel */
			if(System.SetupModeOn);
			else{
				pn_MotorZoneSet(TempMode);
			}
		}
	}



	if(gDirection == 0){
		if(pos != 1){
			pFaultTimeSetting();
		}
	}
	else{
		if(pos != 2){
			pFaultTimeSetting();
		}
	}


	MotorStatus.SlowMode = 0;
}

