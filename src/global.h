#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "def.h"
#ifdef _HW_MAIN_
	#define EXT volatile
#else
	#define EXT extern volatile
#endif

#define TRIAC_TOP			((*(RB_BTF*)&P3OUT).p1)
#define TRIAC_BOTTOM		((*(RB_BTF*)&P3OUT).p2)
#define RELAY_OUT			((*(RB_BTF*)&P5OUT).p1)
/**************************************************************/
/*  Fixed Parameter List                                      */
/**************************************************************/



/*===================================================================*/
/* ENUM																 */
/*===================================================================*/
enum{
	STATUS_INIT,
	STATUS_ERR,
	STATUS_IDLE,
	STATUS_READY,
	STATUS_ON
};
/*===================================================================*/
/* Struct, Union													 */
/*===================================================================*/
typedef struct{
	uint16_t OP_Time;
	uint16_t Temp1;
	uint16_t Temp2;
}_Parameter;
typedef struct{
	uint16_t Temp1;
	uint16_t Temp2;
}_AD;
typedef struct{
	uint8_t Triac 		: 1;
	uint8_t SensorErr 	: 1;
	uint16_t RunTime;
	uint16_t Comp;
	uint16_t Temp;	
	uint32_t pre_time;
}_Heater;
typedef struct{
	_Heater Heater_Top;
	_Heater Heater_Bottom;
	uint16_t OpTime;
	uint16_t RunTime;
	uint16_t Temp1;
	uint16_t Temp2;
	uint32_t pre_time;
	uint8_t Mode;
	uint8_t OldStart;
	uint8_t Start;
	uint8_t status;
	uint8_t oldstatus;
}_Status;
typedef struct{
	uint16_t Value;
	uint16_t Max;
	uint16_t Min;
}_Para;
EXT _Status Status;
EXT _Status gStatus;
EXT _Parameter gParameter[6];
EXT _Para gDispParameter;
//EXT _Status Status;
/*===================================================================*/
/* uint8_t Variable													 */
/*===================================================================*/
EXT uint8_t IIC_TX_Complete;
EXT int8_t Versions[8];
EXT uint8_t gMonitorError[16];
EXT uint8_t gRoast_No;
EXT uint8_t gEEP_save;
/*===================================================================*/
/* uint16_t Variable												 */
/*===================================================================*/

/*===================================================================*/
/* uint32_t Variable												 */
/*===================================================================*/
EXT uint32_t gWaffleCount;
EXT uint32_t gEEP_time;
#endif
