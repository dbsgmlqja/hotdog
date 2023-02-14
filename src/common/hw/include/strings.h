#ifndef _STRINGS_H_
#define _STRINGS_H_


#define	COLUMN_MAX	4

#define S_BLK   0x00
#define	S_1		0x06
#define	S_2		0x5B
#define	S_3		0x4F
#define	S_4		0x66
#define	S_5		0x6D
#define	S_6		0x7D
#define	S_7		0x27
#define	S_8		0x7F
#define	S_9		0x67
#define	S_0		0x3F

#define	S_A		0x77
#define	S_B		0x7C
#define	S_C		0x39
#define	S_D		0x5E
#define	S_E		0x79
#define	S_F		0x71
#define	S_G		0x3D
#define	S_H		0x76
#define	S_I		0x30
#define	S_J		0x1E
#define	S_K		0x72
#define	S_L		0x38
#define	S_M		0x55
#define	S_N		0x54
#define	S_O		0x5C
#define	S_P		0x73
#define	S_Q		0x67
#define	S_R		0x50
#define	S_S		0x6D
#define	S_T		0x78
#define	S_U		0x3E
#define	S_V		0x7E
#define	S_W		0x6A
#define	S_X		0x36
#define	S_Y		0x6E
#define	S_Z		0x49

#define S_DS    0x40
#define S_UB    0x08

//const unsigned char Dec2Bcd[10] = {S_0,S_1,S_2,S_3,S_4,S_5,S_6,S_7,S_8,S_9};
const char LCD_LOAD[COLUMN_MAX]={
	//Welcome to
	//Door System
	//0123456789012345
	S_V,0x86,S_0,S_0
};







const unsigned char Dec2Bcd[10] = {S_0,S_1,S_2,S_3,S_4,S_5,S_6,S_7,S_8,S_9};


unsigned char LCD_RESET1[COLUMN_MAX]={
	//Welcome to
	//Door System

	S_S,S_2 | 0x80,S_0,S_1

};





const char MAIN_MENU_TOP[COLUMN_MAX]={
	//*Select Mode*
	//0123456789012345
	S_P,S_A,S_R,S_A

};
const char MAIN_FMENU_TOP[COLUMN_MAX]={
	//*Select Mode*
	//0123456789012345
	S_F,S_P,S_A,S_R

};
const char MAIN_MENU_SUB[][COLUMN_MAX]={
	//    Parameter
	//0123456789012345
//	{"   Parameter    "},
	//@150616 by Bang
	{S_M,S_E,S_N,S_U},
	{S_P,S_A,S_R,S_A},
	
};




const char BLANK[COLUMN_MAX]={
	//0123456789012345
	S_BLK,S_BLK,S_BLK,S_BLK
};

const char EMG_DISPLAY[COLUMN_MAX]={
	//0123456789012345
//	 "   EMERGENCY    ",
//	 " [Turn off/on]  ",
	 S_E | 0x80,S_E,S_M,S_G
};







const char DISP_MODE_FAULT_OVERTEMP[COLUMN_MAX]={
	//0123456789012345
	S_E | 0x80,S_T,S_M,S_P
};


const char DISP_MODE_FAULT_FTIMEHIGH[COLUMN_MAX]={
	//0123456789012345
	S_E | 0x80,S_H,S_S,S_T
};

const char DISP_MODE_FAULT_FTIMELOW[COLUMN_MAX]={
	//0123456789012345
	S_E | 0x80,S_L,S_S,S_T
};


const char SCR_MODE0[][4]={
	{S_S,S_T,S_O,S_P},
	{S_O,S_P,S_E,S_N},
	{S_C,S_L,S_O,S_S},
	{S_W,S_A,S_I,S_T},
	
	{S_L,S_L,S_0,S_1},
	{S_L,S_L,S_0,S_2},
	{S_U,S_L,S_0,S_1},
	{S_U,S_L,S_0,S_2},
};

const char SCR_MODE1[][4]={
	{S_A,S_U,S_T,S_O},
	{S_H,S_A,S_N,S_D},
	{S_T,S_E,S_S,S_T},
	{S_S,S_E,S_T,S_BLK},
};

const char SCR_MODE2[][2]={
	{S_R,S_D},
	{S_O,S_N},
};

const char SCR_POS[][COLUMN_MAX]={
	{S_L,S_L,S_0,S_1},
	{S_L,S_L,S_0,S_2},
	{S_U,S_L,S_0,S_1},
	{S_U,S_L,S_0,S_2},
};

const char SCR_L2MODE[][COLUMN_MAX]={
	//0123456789012345
	{S_1,S_BLK,S_D,S_I},
	{S_2,S_BLK,S_I,S_N},
	{S_3,S_BLK,S_F,S_R},
	{S_4,S_BLK,S_C,S_U},
	{S_5,S_BLK,S_V,S_O},
	{S_6,S_BLK,S_C,S_N},
	{S_7,S_BLK,S_T,S_E},
	
};

const char LCD_TESTMODE[COLUMN_MAX]={
	//0123456789012345
	S_T,S_E,S_S,S_T
};

const char LCD_SAVEMODE[COLUMN_MAX]={
	//0123456789012345
	S_S,S_A,S_V,S_E
};




const char ERRMSG_OVERCURRENT[COLUMN_MAX]={
	//0123456789012345
	S_E,S_R,S_DS,S_C
};

const char ERRMSG_OVERVOLTAGE[COLUMN_MAX]={
	//0123456789012345
	S_E,S_R,S_DS,S_O
};
const char ERRMSG_UNDERVOLTAGE[COLUMN_MAX]={
	//0123456789012345
	S_E,S_R,S_DS,S_U
};
/*@150429 by Bang */
const char ERRMSG_INPUTERROR[COLUMN_MAX]={
	//0123456789012345
	S_E,S_R,S_DS,S_S
};
const char ERRMSG_LIMITERROR[COLUMN_MAX]={
	//0123456789012345
	S_E,S_R,S_DS,S_L
};
/*@090115*/
const char ERRMSG_IGBT_FO[COLUMN_MAX]={
	//0123456789012345
	S_E,S_R,S_DS,S_P
};
const char ERRMSG_P24V[COLUMN_MAX]={
	//0123456789012345
	S_E,S_R,S_DS,S_2
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
	{S_N,S_O,S_E,S_R},
	{S_U,S_V,S_O,S_L},
	{S_O,S_V,S_0,S_L},
	{S_T,S_E,S_M,S_P},
	{S_O,S_BLK,S_L,S_BLK},
	{S_O,S_P,S_V,S_T},
	{S_O,S_C,S_U,S_R},
	{S_E,S_BLK,S_I,S_N},
	{S_I,S_P,S_M,S_F},
};




#endif
