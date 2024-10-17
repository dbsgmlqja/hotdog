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

#define	B2_c		'.'
#define	B2_d		'7'
#define	B2_e		'5'
#define	B2_f	 	'C'

const char LCD_LOAD[][COLUMN_MAX]={
	//Welcome to
	//Door System
	//0123456789012345
	{"  INITIALIZING  "},
	{" RECEIVING DATA "},
};

const char LCD_DEBUG_KEY[][COLUMN_MAX]={
	//Welcome to
	//Door System
	//0123456789012345
	{"KEY ADC0 : xxx  "},
	{"KEY ADC1 : xxx  "},
};

const char LCD_DEBUG_CODE[][COLUMN_MAX]={
	//Welcome to
	//Door System
	//0123456789012345
	{" KEY INPUT CODE "},
	{"CODE VALUE : xx "},
};


const char LCD_MAIN0[][COLUMN_MAX] = {
	//0123456789012345
	{"T :     B :     "},
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
	{B2_1,B2_2,B2_3,B2_4,B2_5,B2_6,B2_7,B2_8,B2_9,B2_a,B2_b,'6',B2_c,B2_d,B2_e,B2_f,0},
};
uint8_t LCD_BOTTOM_RESET0[COLUMN_MAX]={B2_1,B2_2,B2_3,B2_4,B2_5,B2_6,B2_7,B2_8,B2_9,B2_a,B2_b,B2_c,B2_c,B2_d,B2_e,B2_f,0};
const char MAIN_MENU_TOP[][COLUMN_MAX]={
	//*Select Mode*
	//0123456789012345
	{"* SELECT MODE  *"},
	{"*HEATER SETTING*"},
	{"*  IVERTER     *"},
	{"* INITIALIZE   *"},
	{"* FAULT SCAN   *"},
	
};

const char MAIN_MENU_SUB[][COLUMN_MAX]={
	//    Parameter
	//0123456789012345
	{"  Heater        "},
	{"  Inverter      "},
	{"  Initialize    "},
	{"  Fault Scan    "},
};

const char HEAT_MODE_SUB_TOP[][COLUMN_MAX]={
	//0123456789012345
	{"* TOP TEMP     *"},
	{"* BOTTOM TEMP  *"},
	{"* WARM-UP SET  *"},
};
const char HEAT_WARMUP_SUB_TOP[][COLUMN_MAX]={
	//0123456789012345
	{"* WARM-UP TIME *"},
	{"* WARM-UP TEMP *"},
};
const char HEAT_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{" Top Heater     "},
	{" Bottom Heater  "},
	{" Warm-up Setting"},
};

const char WARMUP_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{" Warm-up Time   "},
	{" Warm-up Temp   "},
};
const char INV_MODE_SUB_TOP[][COLUMN_MAX]={
	//0123456789012345
	{"*INVERTER SPEED*"},
//	{"* FOWARD SPEED *"},
	{"* REVERSE SPEED*"},
};
const char INV_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{" Inverter Speed "},
//	{"  Foward Speed  "},
	{"  Reverse Speed "},
};

const char INITIALIZE_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{"  Initialize 1  "}, //@150427 by Bang
	{"  Initialize 2  "},//@150427 by Bang
};

const char FAULT_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{"  Fault List    "},
	{"  Fault Clear   "},
	{" Counter Clear  "},
};


const char OPTION_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{"  Encoder       "},
	{"  Limit Switch  "},
	{" Encoder + Limit"},
};

//@150423 By Bang
const char FACTORY_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{"    Code : ____ "},
	{" Model No.      "},
	
};

//@090826
const char POS_INIT_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{"Pos. Initialize "},
	{"Pos.Initializing"},
};
const char UNIT_HEAT[][6]={
//0123456789012345
	{'[',0xDF,'C',']',' ',0x0A},	/*ACC TIME*/
	{'[',0xDF,'C',']',' ',0x0A},	/*ACC TIME*/
	"[%]  ",	/*TORQUE*/
	"[HZ] ",	/*UP FREQ*/
	"[HZ] ",	/*DOWN FREQ*/
	"[MIN]",	/*ULS TIME*/
	{'[',0xDF,'C',']',' ',0x0A},	
};
const char UNIT[][6]={
//0123456789012345
	"[Hz] ",	/*ACC TIME*/
	"[Hz] ",	/*DEC TIME*/
	"[%]  ",	/*TORQUE*/
	"[HZ] ",	/*UP FREQ*/
	"[HZ] ",	/*DOWN FREQ*/
	"[SEC]",	/*ULS TIME*/
	"[%]  ",	/*UP TORQUE*/
	"[%]  ",	/*DOWN TORQUE*/
	"[%]  ",	/*OVER LOAD*/
	"[HP] ",	/*OVER CURR */
	"[%]  ",	/*OVER VOLT*/
	"[%]  ",	/*UNDER VOLT*/
	"[%]  ",	/*BREAK VOLT*/
	"[HZ] ",	/* STOP FREQ*/
	"[pls]",  /*Half Position*/
	"[%]  ",  /*SS SKIP*/
	"[SEC]",  /*HSENS TIMEOUT*/
	"[SEC]",  /*LSENS TIMEOUT*/
	"     ",  /*MOTOR Direction*/
	"     ",  /*Encoder Phase */
	"[pls]",  /*Encoder Brake*/
	"     ",  /*SS SELECT*/
	"[S]  ",  /*AIR CURTAIN time*/
	"     ",  /*PRESS SELECT*/
	"[HZ] ",	/* FREQ*/
};

#if 0
const char LOCK_MODE_SUB[][COLUMN_MAX]={
	//0123456789012345
	{" INTERLOCK OFF  "},
};
#endif

//@150423 By Bang
const char FACTORY_MODE_TOP[][COLUMN_MAX]={
	//0123456789012345
	{"* FACTORY MODE *"},
	{"* MODEL NO.    *"},
	
};
const char POWER_FAIL_TOP[][COLUMN_MAX]={
	{" All Data Saved "},
	{" Power Off      "},
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
const char DISP_MODE_FAULT_SSR1COMM[][COLUMN_MAX]={
	//0123456789012345
	{"DISCONNECT SSR1 "},
	{" [PRESS RESET]  "},
};
const char DISP_MODE_FAULT_SSR2COMM[][COLUMN_MAX]={
	//0123456789012345
	{"DISCONNECT SSR2 "},
	{" [PRESS RESET]  "},
};
const char DISP_MODE_FAULT_SSR1ERRORTOP[COLUMN_MAX]={
	//0123456789012345
	 " SSR1 ERROR!    "
};
const char DISP_MODE_FAULT_SSR2ERRORTOP[COLUMN_MAX]={
	//0123456789012345
	 " SSR2 ERROR!    "
};
const char DISP_MODE_FAULT_SSRERROR[][COLUMN_MAX]={
	//0123456789012345
	{" No Error       "},
	{"OVER Temperarue "},
	{"SCR or no load  "},
	{"Over Voltage    "},
	{"Low Voltage     "},
	{"Abnormal Input  "},
	{"Input Disconnect"},
};
const char DISP_MODE_FAULT_FTIMEHIGH[][COLUMN_MAX]={
	//0123456789012345
	{"OVER HSENSE TIME"},
	{" [POWER OFF/ON] "},
};

const char DISP_MODE_FAULT_FTIMELOW[][COLUMN_MAX]={
	//0123456789012345
	{"OVER LSENSE TIME"},
	{" [POWER OFF/ON] "},
};


const char SCR_MODE0[][5]={
	{"STOP"},
	{" UP "},
	{"DOWN"},
	{"WAIT"},
	
	{"LL01"},
	{"LL02"},
	{"UL01"},
	{"UL02"},

	{"ESTP"},
	{"LSTP"},
	{"HSTP"},
};

const char SCR_MODE1[][5]={
	{"AUTO"},
	{"MANU"},
	{"TEST"},
	{"SET "},
};

const char SCR_MODE2[][3]={
	{"RD"},
	{"ON"},
};

const char SCR_POS[][5]={
	{"LL01"},
	{"LL02"},
	{"UL01"},
	{"UL02"},
};


const char SCR_L3MODE[][17]={
	//0123456789012345
	{"[ ]     [ ]     "},
	{"Freq. : xx.xx Hz"},
	{"SSR2 : xxx.xV[ ]"},
	{"SSR1 : xxx.xV[ ]"},
};
const char LCD_TESTMODE[][COLUMN_MAX]={
	//0123456789012345
	{"   TEST  MODE   "},
	{"COUNT:          "},
};

const char LCD_SAVEMODE[][COLUMN_MAX]={
	//0123456789012345
	{"  *SAVE  DATA*  "},
	{"  *SAVE  DATA*  "},
};

const char ERRMSG_OVERLOAD[][COLUMN_MAX]={
	//0123456789012345
	{" CURRENT ERROR  "},
	{"   OVERLAOD     "},
};

/*@180224 */
const char ERRMSG_OVERLOAD2[][COLUMN_MAX]={
	//0123456789012345
	{" CURRENT ERROR  "},
	{"   OVERLAOD 2   "},
};


const char ERRMSG_OVERPVT[][COLUMN_MAX]={
	//0123456789012345
	{" CURRENT ERROR  "},
	{"   OVER PVT     "},
};

const char ERRMSG_OVERCURRENT[][COLUMN_MAX]={
	//0123456789012345
	{" CURRENT ERROR  "},
	{"  OVER CURRENT  "},
};

const char ERRMSG_OVERVOLTAGE[][COLUMN_MAX]={
	//0123456789012345
	{" VOLTAGE ERROR  "},
	{"  OVER VOLTAGE  "},
};
const char ERRMSG_UNDERVOLTAGE[][COLUMN_MAX]={
	//0123456789012345
	{" VOLTAGE ERROR  "},
	{" UNDER VOLTAGE  "},
};
/*@150429 by Bang */
const char ERRMSG_INPUTERROR[][COLUMN_MAX]={
	//0123456789012345
	{"CHECK THE SWITCH"},
	{"[ PRESS  RESET ]"},
};
/*@090115*/
const char ERRMSG_IGBT_FO[][COLUMN_MAX]={
	//0123456789012345
	{" IGBT FO !!!!   "},
	{" PWM OUTPUT OFF "},
};

const char ENCODER_WIRE_ERROR[][COLUMN_MAX]={
	//0123456789012345
	{" ENCODER ERROR  "},
	{" [POWER OFF/ON] "},
};
/*@161119 by Bang */
const char ERRMSG_BRAKE_ERROR[][COLUMN_MAX]={
	//0123456789012345
	{"  BRAKE ERROR   "},
	{"[ PRESS  RESET ]"},
};
/*@210607 by Bang */
const char MOTOR_OVERHEAT_ERROR[][COLUMN_MAX]={
	//0123456789012345
	{" MOTOR OVERHEAT "},
	{" [PRESS RESET]  "},
};
/*@210607 by Bang */
const char DOOR_FELL_ERROR[][COLUMN_MAX]={
	//0123456789012345
	{"DOOR FELL ERROR "},
	{" [PRESS  RESET] "},
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

const char ERRMSG_LIST[][COLUMN_MAX]={
	//0123456789012345
	{" -No Error !    "},
	{" -Under Voltage "},
	{" -Over Voltage  "},
	{" -Over Temp.    "},
	{" -Over Load     "},
	{" -Timeout Error "},
	{" -Over Current  "},
	{" -Input Error   "},
	{" -Brake Error   "},
	{" -IGBT Fault    "}, //@171014
	{" -Over Load 2   "},	//@180224
	{" -Encoder Error "},	//@210604
	{" -Motor Overheat"},	//@210604
	{" -Door Fell     "},	//@210604
};


const char SystemLock_Top[][COLUMN_MAX]={
	//0123456789012345
	{"** KEY LOCKED **"},
	{" CODE : _ _ _ _ "},
};
const char INVERTER_OP[][COLUMN_MAX] = {
	//0123456789012345
	{"FOWARD RUNNING  "},
	{"BACKWARD RUNNING"},
	{"INVERTER STOP   "}
};
const char SystemLockFault[COLUMN_MAX] =
	//0123456789012345
	{"  Fault Scan.   "};
const char EncoderSet_Text[COLUMN_MAX] = 
	{"Set up Encoder  "};

const char InverterErrorString[][COLUMN_MAX] = {
	//0123456789012345
	{" - No Error     "},	//0
	{" - Over Current "},	//1
	{" - Over Voltage "},	//2
	{" - EXT A Trip   "},	//3
	{" - Emergency    "},	//4
	{" - In Phase Open"},	//5
	{" - Ground Trip  "},	//6
	{" - Inv OverHeat "},	//7
	{" - MotorOverHeat"},	//8
	{" - Over Load    "},	//9
	{" - H/W Diag     "},	//10
	{" - EXT B Trip   "},	//11
	{" - EEPROM Error "},	//12
	{" - FAN Error    "},	//13
	{" - OutPhase Open"},	//14
	{" - Inv Over Load"},	//15
	{" - Low Voltage  "},	//16
};
#endif
