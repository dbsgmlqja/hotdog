#ifndef _STRINGS_H_
#define _STRINGS_H_


#define	COLUMN_MAX	17

#define	B2_1		'H'
#define	B2_2		'e'
#define	B2_3		'a'
#define	B2_4		't'
#define	B2_5		'i'
#define	B2_6		'n'
#define	B2_7		'g'
#define	B2_8		' '
#define	B2_9		'S'
#define	B2_a		'y'
#define	B2_b		's'

#define	B2_c		' '
#define	B2_d		'.'
#define	B2_e		'0'
#define	B2_f	 	'0'

const char LCD_LOAD[][COLUMN_MAX]={
	//Welcome to
	//Door System
	//0123456789012345
	{"  INITIALIZING  "},
	{" RECEIVING DATA "},
};


const char LCD_MAIN0[][COLUMN_MAX] = {
	//0   1   2   3   4   5    6   7   8   9   0   1   2   3   4   5
	{'S',' ',' ',' ',' ',0xDF,'C',' ',' ','M','R',' ',' ',' ',' ',' '},
	{"                "},
	{"SSR1 : xxx.xV[ ]"},
};
const char LCD_MAIN1[][COLUMN_MAX] = {
	//0   1   2   3   4   5    6   7   8   9   0   1   2   3   4   5
	{'W','A','R','M','-','U','P',' ',' ','R',' ',' ',' ',' ',' ',' '},
	{"                "},
	{"SSR1 : xxx.xV[ ]"},
};
const char EMG_DISPLAY[][COLUMN_MAX]={
	//0123456789012345
//	 "   EMERGENCY    ",
//	 " [Turn off/on]  ",
	 " EMERGENCY-STOP ",
	 " Release Key    ",
};

const uint8_t LCD_RESET0[][COLUMN_MAX]={
	//Welcome to
	//Door System
	//0123456789012345
	{" Welcome To     "},
	{B2_1,B2_2,B2_3,B2_4,B2_5,B2_6,B2_7,B2_8,B2_9,B2_a,B2_b,B2_c,'1',B2_d,B2_e,B2_f,0},
};
uint8_t LCD_BOTTOM_RESET0[COLUMN_MAX]={B2_1,B2_2,B2_3,B2_4,B2_5,B2_6,B2_7,B2_8,B2_9,B2_a,B2_b,B2_c,B2_c,B2_d,B2_e,B2_f,0};
const char MAIN_MENU_TOP[][COLUMN_MAX]={
	//*Select Mode*
	//0123456789012345
	{"* SELECT MODE  *"},
	{"*HEATER SETTING*"},
	{"*MOTOR SETTING *"},
	{"* INITIALIZE   *"},
	{"* FAULT SCAN   *"},
	
};

const char MAIN_MENU_SUB[][COLUMN_MAX]={
	//    Parameter
	//0123456789012345
	{"  Heater        "},
	{"  Motor         "},
	{"  Initialize    "},
	{"  Fault Scan    "},
};

const char HEAT_MODE_SUB_TOP[][COLUMN_MAX]={
	//0123456789012345
	{"* TEMPERATURE  *"},
	{"* WARM-UP SET  *"},
};
const char HEAT_WARMUP_SUB_TOP[][COLUMN_MAX]={
	//0123456789012345
	{"* WARM-UP TIME *"},
	{"* WARM-UP TEMP *"},
};
const char HEAT_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{" Temperature    "},
	{" Warm-up Setting"},
};

const char WARMUP_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{" Warm-up Time   "},
	{" Warm-up Temp   "},
};
const char INV_MODE_SUB_TOP[][COLUMN_MAX]={
	//0123456789012345
	{"* MOTOR SPEED  *"},
	{"* MOTOR DIR    *"},
};
const char INV_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{" Motor Speed    "},
	{" Motor Dir      "},
};

const char INITIALIZE_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	"  Initialize    "
};

const char FAULT_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{"  Fault List    "},
	{"  Fault Clear   "},
	{" Counter Clear  "},
};



//@150423 By Bang
const char FACTORY_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{"    Code : ____ "},
	{" Model No.      "},
	
};

const char UNIT_HEAT[][6]={
//0123456789012345
	{'[',0xDF,'C',']',' ',0x0A},	/*ACC TIME*/
	{'[',0xDF,'C',']',' ',0x0A},	/*ACC TIME*/
	"[%]  ",	/*TORQUE*/
	"[%]  ",	/*UP FREQ*/
	"[%]  ",	/*DOWN FREQ*/
	"[MIN]",	/*ULS TIME*/
	{'[',0xDF,'C',']',' ',0x0A},	
};
const char UNIT[][6]={
//0123456789012345
	"[rpm]",	/*ACC TIME*/
	"[%]  ",	/*DEC TIME*/
	"     ",	/*TORQUE*/
};

const char INI_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{"  Fault List    "},
	{"  Fault Clear   "},
	{" Counter Clear  "},
};

#if 1
const char FAULT_MODE_TOP[][COLUMN_MAX]={
	//0123456789012345
	{"* FAULT LIST 1 *"},
	{"* FAULT LIST 2 *"},
	{"* FAULT LIST 3 *"},
	{"* FAULT LIST 4 *"},
	{"* FAULT LIST 5 *"},
	{"* FAULT LIST 6 *"},
	{"* FAULT LIST 7 *"},
	{"* FAULT LIST 8 *"},
	{"* FAULT LIST 9 *"},
	{"* FAULT LIST 10*"},
};
#endif


const char BLANK[COLUMN_MAX]={
	//0123456789012345
	"                "
};


const char DISP_MODE_FREQ[][COLUMN_MAX]={
	//0123456789012345
	{" WORKING FREQ.  "},
	{"                "},
};

const char DISP_MODE_FAULT_OV[][COLUMN_MAX]={
	//0123456789012345
	{" FAULT MESSAGE  "},
	{" OVER VOLTAGE   "},
};

const char DISP_MODE_FAULT_UV[][COLUMN_MAX]={
	//0123456789012345
	{" FAULT MESSAGE  "},
	{" UNDER VOLTAGE  "},
};

const char DISP_MODE_FAULT_EMG[][COLUMN_MAX]={
	//0123456789012345
	{" EMERGENCY STOP "},
	{" [POWER OFF/ON] "},
};

const char DISP_MODE_FAULT_OVERTEMP[][COLUMN_MAX]={
	//0123456789012345
	{"OVER TEMPERATURE"},
	{" [PRESS RESET]  "},
};

const char DISP_MODE_FAULT_INVERTERCOMM[][COLUMN_MAX]={
	//0123456789012345
	{"DISCONNECT CONV'"},
	{" [PRESS RESET]  "},
};
const char DISP_MODE_FAULT_SSRCOMM[][COLUMN_MAX]={
	//0123456789012345
	{" DISCONNECT SSR "},
	{" [PRESS RESET]  "},
};

const char DISP_MODE_FAULT_MOTORERROR[][COLUMN_MAX]={
	//0123456789012345
	{" Motor Error    "},
	{"Check the device"},
};

const char DISP_MODE_FAULT_UPPERSENSOR[][COLUMN_MAX]={
	//0123456789012345
	{" UPPER TEMP     "},
	{" SENSOR ERROR   "},
};

const char DISP_MODE_FAULT_LOWERSENSOR[][COLUMN_MAX]={
	//0123456789012345
	{" LOWER TEMP     "},
	{" SENSOR ERROR   "},
};
const char DISP_MODE_FAULT_SSRHWERROR[][COLUMN_MAX]={
	//0123456789012345
	{" SSR ERROR      "},
	{"Check the device"},
};
const char DISP_MODE_FAULT_SSRERRORTOP[COLUMN_MAX]={
	//0123456789012345
	 "  SSR ERROR!    "
};
const char DISP_MODE_FAULT_SSRERROR[][COLUMN_MAX]={
	//0123456789012345
	{" No Error       "},
	{"OVER Temperarue "},
	{"SCR or no load  "},
	{"SSR Comm'       "},
	{"Motor Error     "},
	{"Sensor1 Error   "},
	{"Sensor2 Error   "},
};

const char ERRMSG_LIST_EEP[][COLUMN_MAX]={
	//0123456789012345
	{" -No Error !    "},
	{" -Over Temp.    "},
	{" -SSR H/W Error "},
	{" -SSR Comm Error"},
	{" -Motor Error   "},
	{" -Upper Sensor  "},
	{" -Lower Sensor  "},
	
};
const char SCR_L3MODE[][17]={
	//0   1   2   3   4   5   6   7    8   9   0   1   2   3   4    5
	{'U',' ',' ',' ',' ',0xDF,'C',' ',' ','L',' ',' ',' ',' ',0xDF,'C'},
	{"Speed : xxx.xrpm"},
	{"SSR M: xxx.xV[ ]"},
	{'S','S','R',' ','T',':',' ','x','x','x',' ',0xDF,'C','[',' ',']'},
};
const char LCD_SAVEMODE[][COLUMN_MAX]={
	//0123456789012345
	{"  *SAVE  DATA*  "},
	{"  *SAVE  DATA*  "},
};

#if 0
	Error_No_Error= 0,
	Error_Under_Voltage,
	Error_Over_Voltage,
	Error_Over_Temp,
	Error_Over_Load,
	Error_Over_PVT,
	Error_Over_Current,
#endif


const char INVERTER_OP[][COLUMN_MAX] = {
	//0123456789012345
	{"MOTOR RUNNING   "},
	{"MOTOR STOP      "}
};
const char SystemLockFault[COLUMN_MAX] =
	//0123456789012345
	{"  Fault Scan.   "};
#endif
