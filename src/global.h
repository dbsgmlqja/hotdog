#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "def.h"
#ifdef _HW_MAIN_
	#define EXT volatile
#else
	#define EXT extern volatile
#endif

#define SSR1_OUT			((*(RB_BTF*)&P2OUT).p2)	//SSR M.C
#define SSR2_OUT			((*(RB_BTF*)&P9OUT).p1)	//SSR FAN
#define MDir				((*(RB_BTF*)&P6OUT).p0)	//CW_CCW
#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif
/**************************************************************/
/*  Fixed Parameter List                                      */
/**************************************************************/



/*===================================================================*/
/* ENUM																 */
/*===================================================================*/
enum{
	STATUS_ERR,
	STATUS_INIT,
	STATUS_IDLE,
	STATUS_READY,
	STATUS_ON
};
enum{
	SSR_DUMMY,
	SSR_SSR,
	SSR_FAN,
	SSR_END
};
enum{
	MOTOR_OFF,
	ACC_ZONE,
	DEC_ZONE,
	MAX_ZONE,
	STATUS_END
};
enum{
	MODBUS_MODE_COMMAND,
	MODBUS_MODE_MONITOR,
	MODBUS_MODE_ERROR,
	MODBUS_MODE_END
};
enum{
	MODBUS_INVERTER,
	MODBUS_SSR_TOP,
	MODBUS_SSR_BOTTOM,
	MODBUS_DEVICE_END
};
enum{
	MODBUS_READY,
	MODBUS_TRANSMIT,
	MODBUS_RECEIVE,
	MODBUS_END
};
enum{
	DISPLAY_REQUEST_OFF,
	DISPLAY_REQUEST_ON
};
enum{
	NO_ERROR,
	ILLEGAL_FUNCTION,
	ILLEGAL_DATA_ADRESS,
	ILLEGAL_DATA_VALUE,
	SLAVE_DEVICE_BUSY = 6
};
enum{
	//Select Mode
	LCD_MAIN_MENU=0,
	LCD_MAIN_HEAT_SET,
	LCD_MAIN_INV_SET,
	LCD_MAIN_INITIALIZE,

	LCD_MAIN_FAULT,
	LCD_MAIN_OPTION,			/*@111128 Add Item*/
	LCD_MAIN_FACTORY,           /*@1504223 By Bang*/
	LCD_MAIN_HEAT_SET_SUB,
	LCD_MAIN_HEAT_SET_TOP_SUB,

//	LCD_MAIN_HEAT_SET_WARMUP,
//	LCD_MAIN_HEAT_SET_WARMUP_SUB,
	LCD_MAIN_INV_SET_SUB,
	LCD_MAIN_FAULT_SUB,
	LCD_MAIN_FACTORY_SUB,     /*@1504223 By Bang*/
	LCD_SYS_INIT,

	LCD_MAIN_INIT,
	LCD_POWER_FAIL,
	LCD_MAIN_DISP_END
};
enum{
	TOP_LINE= 0x01,
	BTM_LINE= 0x02
};

enum{
	LCD_MAIN_STATUS_0 = 0,	//Heater
	LCD_MAIN_STATUS_1,		//Inverter
	LCD_MAIN_STATUS_2,		//SSR
	/*LCD_MAIN_STATUS_3,
	LCD_MAIN_STATUS_4,
	LCD_MAIN_STATUS_5,
	LCD_MAIN_STATUS_6,
	LCD_MAIN_STATUS_7,
	LCD_MAIN_STATUS_8,
	LCD_MAIN_STATUS_9,
	LCD_MAIN_STATUS_10,
	LCD_MAIN_STATUS_11,*/
	LCD_MAIN_STATUS_END
};

enum{
	LCD_SUB_MAIN_HEAT=0,
	LCD_SUB_MAIN_INV,	//@150616 by Bang
	LCD_SUB_MAIN_INITIALIZE,
	LCD_SUB_MAIN_FAULTSCAN,
	LCD_SUB_DISP_END
};
enum{
	LCD_SUB_HEAT_TEMPERATURE,
//	LCD_SUB_HEAT_WARMUP,
	LCD_SUB_HEAT_END
};
enum{
	LCD_SUB_WARMUP_TIME,
	LCD_SUB_WARMUP_TEMP,
	LCD_SUB_WARMUP_END
};
enum{
	DIR_FORWARD,
	DIR_REVERSE,
	DIR_END
};
enum{
	LCD_SUB_INV_F_FREQ,
	LCD_SUB_INV_R_FREQ,
	LCD_SUB_INV_DIR,
	LCD_SUB_INV_END
};

enum{
	LCD_SUB_FAULT_READ,
	LCD_SUB_FAULT_CLEAR,
	LCD_SUB_FAULT_END
};
enum{
	LCD_SUB_INITILIZE,
	LCD_SUB_INITILIZE_END
};
enum{
	Error_No_Error= 0,
	Error_Over_Temp,
	Error_SSR_HW_ERROR,
	Error_SSR_COMM_ERROR,
	Error_Motor,
	Error_TopSensor,
	Error_BottomSensor,
	Error_End
};
/*===================================================================*/
/* Struct, Union													 */
/*===================================================================*/
typedef struct{
	uint16_t Temperature;
//	uint16_t Time;
	uint16_t WarmupTime;
	uint16_t WarmupTemp;
}_Parameter;
typedef struct{
	uint16_t Temp1;
	uint16_t Temp2;
}_AD;
typedef struct{
	uint8_t SensorErr 	: 1;
	uint8_t rechTarget	: 1;
	uint16_t RunTime;
	short Temp;
	uint16_t Temp2;
	uint16_t Prev_Temp;
	uint32_t pre_time;
}_KSensor;
typedef struct{
	uint8_t OldStart;
	uint8_t Start;
	uint8_t mainLoop;
	uint8_t warmupComplete;
	uint8_t warmupArrived;
	uint8_t status;
	uint8_t oldstatus;
	uint16_t OpTime;
	uint16_t RunTime;
	uint16_t Comp;
	uint16_t Temp1;
	uint16_t Temp2;
	uint32_t pre_time;
	uint32_t warmupTimer;
	
	_KSensor KSensor_Top;
	_KSensor KSensor_Bottom;
	
}_Status;
typedef struct{
	uint8_t prev_sub;
	uint16_t Value;
	uint16_t Max;
	uint16_t Min;
}_Para;
typedef union{
	struct{
		uint16_t stop	: 1;
		uint16_t fowardRun: 1;
		uint16_t reverseRun:1;
		uint16_t trip	: 1;
		uint16_t accel	: 1;
		uint16_t decel	: 1;
		uint16_t max		: 1;
		uint16_t dcBraking:1;
		uint16_t stopping: 1;
		uint16_t jogMode : 1;
		uint16_t brakeOut: 1;
		uint16_t fowardRunCommnad : 1;
		uint16_t reverseRunCommnad : 1;
		uint16_t commandSource : 1;
		uint16_t freqSource : 1;
		uint16_t Dummy	: 1;
	} bit;
	uint16_t All;
}_InverterStatus;

typedef union{
	struct{
		uint16_t overCurrent	: 1;
		uint16_t overVoltage	: 1;
		uint16_t EXT_A	: 1;
		uint16_t emgergency	: 1;
		uint16_t inPhaseOpen	: 1;
		uint16_t groundTrip	: 1;
		uint16_t invOverHeat	: 1;
		uint16_t motorOverHeat	: 1;
		uint16_t overLoad	: 1;
		uint16_t HW_DIAG: 1;
		uint16_t EXT_B	: 1;
		uint16_t eeprom	: 1;
		uint16_t FAN	: 1;
		uint16_t outPhaseOpen	: 1;
		uint16_t invOverload	: 1;
		uint16_t lowVoltage	: 1;
	} bit;
	uint16_t All;
}_InverterError;
typedef union{
	struct{
		uint16_t OverTemp : 1;			//Save
		uint16_t EmergencyOn	: 1;
		uint16_t ssrCommError : 1;		//Save
		uint16_t ssrError	: 1;		//Save
		uint16_t Motor		: 1;		//Save
		uint16_t TopSensor	: 1;		//Save
		uint16_t BottomSensor : 1;		//Save
		uint16_t dummy	: 9;
	}bit;
	uint16_t All;
}_Error;
typedef struct{
	uint32_t pre_time;
	uint8_t Request;
	uint8_t Lines;

	uint8_t Status : 3;
	uint8_t OpMode : 2;
	uint8_t Connect : 1;
}_Disp;
typedef struct{
	volatile uint8_t rfg;
	volatile uint8_t tb[50],rb[50];
	volatile uint8_t rc,tc,tcMax;
	uint8_t dc, hi, low;
	uint8_t rxTmr;
	uint8_t deviceError;
	uint8_t commError;
	uint32_t rxTime;
	uint32_t txTime;
}USART_User_InitTypeDef;

typedef struct{
	uint8_t p500msFlag;
	uint8_t no;
	uint8_t mode;
	uint8_t rxMode;
	uint8_t status;
	uint8_t comm[3];
	uint8_t rxTmr[3];
	uint32_t pre_time;
}_ModBus;
typedef struct{
	uint8_t var8_1;
	uint8_t var8_2;
	uint8_t var8_3;
	uint16_t var16_1;
	uint16_t var16_2;
	uint16_t var16_3;
}_DebugVar;
typedef struct{
	uint8_t NewCode;
	uint8_t OldCode;
	uint8_t ErrorMode;
	uint8_t RepeatTimer;
	uint8_t Buf[4];
	uint8_t AMRepeatTimer;
	uint8_t state;
	
	uint8_t PassCode[4];
	uint8_t PassBuffer[4];
	uint8_t PassCount;
	
	uint8_t NewKeyIn : 1;
	uint8_t RepeatKeyIn : 1;
	uint8_t SystemLock: 1;
}_Key;
typedef struct{
	uint32_t InvPreTime;
	uint32_t ReversePreTime;
	uint32_t HeaterPreTime;
	uint32_t Motor1PreTime;
	uint32_t Motor2PreTime;
	uint32_t AutoPreTime;
}_Switch;
typedef struct{
	uint16_t P;
	uint16_t I;
	uint16_t D;
	uint16_t value;
	double prev_error;
	double integral;
}_PID;
typedef struct{
	uint16_t F_Freq;
	uint16_t R_Freq;
	uint16_t Dir;
}_IParameter;
typedef struct{
	uint8_t Direction;
	uint8_t FDirection;
	uint8_t RDirection;
}_MotorStatus;
typedef struct{
	uint8_t error;
	uint8_t output;
	uint16_t voltage;
	uint16_t outVoltage;
	uint16_t temperature;
	_PID PID;
}_SSR;
EXT _MotorStatus MotorStatus;
EXT _Error gError;
EXT _Switch gSwitch;
EXT _InverterError gInverterError;
EXT _InverterStatus gInverterStatus;
EXT _ModBus gModBus;
EXT _Key gKey;
EXT USART_User_InitTypeDef cm3;
EXT _Status Status;
EXT _Status gStatus;
EXT _Parameter gParameter;
EXT _IParameter gIParameter;
EXT _Para gDispParameter;
EXT _Disp gDisplay;

EXT _SSR SSR;
EXT _DebugVar gDebugVar;
//EXT _Status Status;
/*===================================================================*/
/* uint8_t Variable													 */
/*===================================================================*/
EXT uint8_t IIC_TX_Complete;
EXT int8_t Versions[8];
EXT uint8_t gMonitorError[16];
EXT uint8_t gRoast_No;
EXT uint8_t gEEP_save;
EXT uint8_t gLED_Timer,gEMG_Timer,gKeyRepeatTimer,gCurrent_Timer,g10ms_Timer;
EXT uint8_t gLCD_Refresh_Timer,gLCD_Mode_Backup,gLCD_Update_Req;
EXT uint8_t gLCD_Line[16];
EXT uint8_t gDispMode;
EXT uint8_t gDispSub;
EXT uint8_t gSSR1Error,gSSR2Error;
//Key Variable
EXT uint8_t gKeyScanFlag;
EXT uint16_t gKey_AD0;
EXT uint8_t gFan;
EXT uint8_t MotorDirection;
EXT uint8_t gMotorStatus,gMotorFDirection;

//EEPROM
EXT uint8_t gEEP_InitMode;
/*===================================================================*/
/* uint16_t Variable												 */
/*===================================================================*/
EXT uint16_t gWorkFreq,gSSR1output, gSSR2output;
EXT uint16_t gSaveDisplayTimer;
EXT uint16_t gWorkPWM, gSetPWM,gNowPWM,gFW_PWM,gRV_PWM;
EXT uint16_t gFreqCounter;
EXT short gMotorSpeed;
/*===================================================================*/
/* uint32_t Variable												 */
/*===================================================================*/
EXT uint32_t gBakeryCount;
EXT uint32_t gEEP_time;
EXT uint32_t gWarmupTimer;
EXT uint32_t gMotorTime;
#define	P24RELAY	((*(RB_BTF*)&P6OUT).p4)	//READY LED
#endif
