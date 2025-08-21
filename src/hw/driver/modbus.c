#include "hw_def.h"
#include "global.h"
#include "modbus.h"
#include "uart.h"
#include "buzzer.h"
#include "ssr.h"
#include "gpio.h"
#define E485 ((*(RB_BTF*)&P3OUT).p1)
#define MAX_MODBUS_CH 1

extern void pStoreErrorMessage(uint8_t Msg);
//static uint8_t uchCRCHi, uchCRCLo;
//static uint16_t uINDEX;
/*******************************************************************************
  CRC16
  
*******************************************************************************/
typedef struct{
	uint8_t ID;
	uint8_t is_open;
	uint16_t command[3];
	uint16_t monitor[3];
	USART_User_InitTypeDef cm3;
}_modbus_t;
static _modbus_t modbusTBL;

const uint8_t auchCRCH[]  =  {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,0x40
};
 
const uint8_t auchCRCL[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,0x40
};
void modbusInit(void)
{
	//Serial3 Setting
	SIFCLK0 |= 0x80; //Serial 3 : Timer 2
	SC3MD3 &= ~(char)0x30;
	TM02BR = (100-1);//8 - 115200bps,8.681uS, 195 - 9600bps,104uS 
	TM02MD |= 0x40;
	TM02MD &= ~0x40;
	
	SC3MD3 = 0x38;
	P3MD |= 0x000102;
	P3DIR |= 0x01;
//	P2ODC |= 0x08;
	SC3MD0 = 0x18;
	SC3MD2 = 0x88;
	SC3MD1 = 0x03;
	SC3MD1 |= 0x30;
	TM02MD |= 0x80;
	
	G45ICR = 0x4100; //Serial 3 For MODBUS
	P3DIR |= 0x02;
	E485 = 0;
	
	modbusTBL.ID = 3;
	
	modbusTBL.monitor[1] = 0xFFFF;
	
	if(modbusConnectCheck() == 0)modbusTBL.is_open = 1;
	else{
		modbusTBL.is_open = 0;
		gError.bit.ssrCommError = 1;
		buzzer_start();
	}
	
	
}
void CRC16(uint8_t *puchMsg, uint8_t usDataLen)
{
	uint8_t uchCRCHi = 0xFF;
	uint8_t uchCRCLo = 0xFF;
	uint8_t uIndex;
	while(usDataLen--)
	{
		uIndex = uchCRCHi ^ *puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCH[uIndex];
		uchCRCLo = auchCRCL[uIndex];
	}
	modbusTBL.cm3.hi = uchCRCHi;
	modbusTBL.cm3.low = uchCRCLo;
}
void pRbFlush(void);
void UART3_IRQHandler(void)
{
	uint8_t RxBuffer,pMode;
	uint8_t SSROutput;
	uint16_t SSRVoltage;
	static uint8_t n;
	
	
	RxBuffer = SC3RB;
	gModBus.rxTmr[gModBus.no] = 10;
	switch(modbusTBL.cm3.rc){
		case 0:
			if(RxBuffer == modbusTBL.cm3.tb[0])modbusTBL.cm3.rb[modbusTBL.cm3.rc++] = RxBuffer;
			break;
		case 1:	//Function
			switch(RxBuffer)
			{
				case 0x03:
				case 0x04:
				case 0x05:
				case 0x06:
				case 0x10:
					modbusTBL.cm3.rb[modbusTBL.cm3.rc++] = RxBuffer;
					break;
				default:
					modbusTBL.cm3.rc = 0;
			}
			break;
		case 2:
			switch(modbusTBL.cm3.rb[1])
			{
				case 0x03:
				case 0x04:
					if(RxBuffer == (modbusTBL.cm3.tb[5] * 2)){
						modbusTBL.cm3.rb[modbusTBL.cm3.rc++] = RxBuffer;
						n = 4 + modbusTBL.cm3.rb[2];
					}
					else modbusTBL.cm3.rc = 0;
					break;
				
					
				case 0x05:
				case 0x06:
				case 0x10:
					modbusTBL.cm3.rb[modbusTBL.cm3.rc++] = RxBuffer;
					n = 7;
					break;
				default:
					modbusTBL.cm3.rc = 0;
			}
			break;
		case 4:
			modbusTBL.cm3.rb[modbusTBL.cm3.rc++] = RxBuffer;
			if((RxBuffer > 0x00) && (RxBuffer < 0x07))
			{
				CRC16((uint8_t *)&modbusTBL.cm3.rb[0], 5);
				if((modbusTBL.cm3.rb[3] == modbusTBL.cm3.hi) && (modbusTBL.cm3.rb[4] == modbusTBL.cm3.low))
				{
					//통신 에러 발생
					modbusTBL.cm3.deviceError = modbusTBL.cm3.rb[2];
					modbusTBL.cm3.rc = 0;
				}
			}
			break;
		default:
			if(modbusTBL.cm3.rc > n){
				modbusTBL.cm3.rc = 0;
				gModBus.status = MODBUS_READY;
				pRbFlush();
				return;
			}
			else if(modbusTBL.cm3.rc == n)
			{
				modbusTBL.cm3.rb[modbusTBL.cm3.rc] = RxBuffer;
				CRC16((uint8_t *)&modbusTBL.cm3.rb[0], n - 1);
				modbusTBL.cm3.rc = 0;
				if((modbusTBL.cm3.rb[n - 1] == modbusTBL.cm3.hi) && (modbusTBL.cm3.rb[n] == modbusTBL.cm3.low))
				{
					if(gModBus.rxMode == 0xff)pMode = gModBus.mode;
					else pMode = gModBus.rxMode;
					
					//정상 수신
					switch(pMode)
					{
						case MODBUS_MODE_COMMAND:
							gSetCommand( 0, 0, 0);
							break;
						case MODBUS_MODE_MONITOR:
							
							if((modbusTBL.monitor[0] & 0xf000) == 0x3000)
							{
								SSRVoltage = (modbusTBL.cm3.rb[3] << 8) | modbusTBL.cm3.rb[4];
								setSSRVoltage(SSRVoltage);
							}
							else{
								SSRVoltage = (modbusTBL.cm3.rb[3] << 8) | modbusTBL.cm3.rb[4];
								setSSROutVoltage(SSRVoltage);
							}
							SSROutput = modbusTBL.cm3.rb[8];
							setSSROutput(SSROutput);
							break;
						case MODBUS_MODE_ERROR:
							SSROutput = modbusTBL.cm3.rb[4];
							setSSRError(SSROutput);
							SSRVoltage = (modbusTBL.cm3.rb[5] << 8) | modbusTBL.cm3.rb[6];
							setSSRTemperature(SSRVoltage);
							break;
					}
					modbusTBL.cm3.commError = 0x00;
					if(gModBus.rxMode == 0xff){
						if(++gModBus.mode >= MODBUS_MODE_END){
							gModBus.mode = MODBUS_MODE_COMMAND;
						}
					}
					else gModBus.rxMode = 0xff;
					gError.bit.ssrCommError = 0;
					gModBus.status = MODBUS_READY;
					modbusTBL.cm3.deviceError = 0x00;
//					pRbFlush();
					if(modbusTBL.cm3.commError < 0xff)modbusTBL.cm3.commError = 0;
					modbusTBL.cm3.rxTime = millis();
					
				}
			}
			else
			{
				modbusTBL.cm3.rb[modbusTBL.cm3.rc++] = RxBuffer;
			}
	}
	gModBus.p500msFlag = 0;
}
void modbusTransmit(void)
{
	uint8_t i,n,func,qty;
	if(modbusTBL.is_open == 0){
		return;
	}
	if(gModBus.status == MODBUS_RECEIVE) return;
	if(gModBus.p500msFlag == 0) return;
	if(gError.bit.EmergencyOn)return;
	gModBus.p500msFlag = 0;
	if(gModBus.mode == MODBUS_MODE_COMMAND)
	{
		if(modbusTBL.command[2] == 1){
			gModBus.no = i;
		}
	}
	switch(gModBus.mode)
	{
		case MODBUS_MODE_COMMAND:
			if(modbusTBL.command[2] == 1)
			{
				modbusTBL.cm3.tb[0] = modbusTBL.ID;
				modbusTBL.cm3.tb[1] = 0x06;
				modbusTBL.cm3.tb[2] = modbusTBL.command[0] >> 8;
				modbusTBL.cm3.tb[3] = modbusTBL.command[0] & 0xff;
				modbusTBL.cm3.tb[4] = modbusTBL.command[1] >> 8;
				modbusTBL.cm3.tb[5] = modbusTBL.command[1] & 0xFF;
				CRC16((uint8_t *)&modbusTBL.cm3.tb[0],6);
				modbusTBL.cm3.tb[6] = modbusTBL.cm3.hi;
				modbusTBL.cm3.tb[7] = modbusTBL.cm3.low;
				n = 8;
			}
			else n = 0;
			break;
		case MODBUS_MODE_MONITOR:
			func = 0x03;
			qty = 0x03;
			if(modbusTBL.monitor[2] == 1)
			{
				modbusTBL.cm3.tb[0] = modbusTBL.ID;
				modbusTBL.cm3.tb[1] = func;
				func = 0x00;
				modbusTBL.cm3.tb[2] = func;
				modbusTBL.cm3.tb[3] = modbusTBL.monitor[0] & 0xff;
				modbusTBL.cm3.tb[4] = 0x00;
				modbusTBL.cm3.tb[5] = qty;
				CRC16((uint8_t *)&modbusTBL.cm3.tb[0],6);
				modbusTBL.cm3.tb[6] = modbusTBL.cm3.hi;
				modbusTBL.cm3.tb[7] = modbusTBL.cm3.low;
				n = 8;
			}
			else n = 0;
			break;
		case MODBUS_MODE_ERROR:
			
			modbusTBL.cm3.tb[0] = modbusTBL.ID;
			modbusTBL.cm3.tb[1] = 0x04;
			modbusTBL.cm3.tb[2] = 0x00;
			modbusTBL.cm3.tb[3] = 0x03;
			modbusTBL.cm3.tb[4] = 0x00;
			modbusTBL.cm3.tb[5] = 0x02;
			CRC16((uint8_t *)&modbusTBL.cm3.tb[0],6);
			modbusTBL.cm3.tb[6] = modbusTBL.cm3.hi;
			modbusTBL.cm3.tb[7] = modbusTBL.cm3.low;
			n = 8;
			
			break;
	}
	if(n > 0)
	{
		E485 = 1;
		gModBus.status = MODBUS_TRANSMIT;
		for(i=0;i<n;i++)
		{
			uartWrite(_DEF_SERIAL3, modbusTBL.cm3.tb[i]);
		}
		gModBus.status = MODBUS_RECEIVE;
		gModBus.pre_time = millis();
		delay(2);
		E485 = 0;
		modbusTBL.cm3.txTime = millis();
		modbusTBL.cm3.rxTime = millis();
	}
	else
	{
		if(++gModBus.no >= MAX_MODBUS_CH){
			gModBus.no = MODBUS_INVERTER;
			if(++gModBus.mode >= MODBUS_MODE_END)
			{
				gModBus.mode = MODBUS_MODE_COMMAND;
			}
		}
	}
}
void pRbFlush(void)
{
	uint8_t i;
	for(i=0;i<40;i++)modbusTBL.cm3.rb[i] = 0x00;
}
void gSetCommand(uint16_t addr, uint16_t value, uint16_t Flag)
{
	modbusTBL.command[0] = addr;
	modbusTBL.command[1] = value;
	modbusTBL.command[2] = Flag;
}
void gSetMonitor(uint16_t addr, uint16_t Flag)
{
	modbusTBL.monitor[0] = addr;
	modbusTBL.monitor[1] = 0xFFFF;
	modbusTBL.monitor[2] = Flag;
}

uint16_t gGetMonitor(uint16_t addr)
{
	if(modbusTBL.monitor[0] == addr)return modbusTBL.monitor[1];
	else return 0xFFFF;
}
void modbusCheck(void)
{
	uint32_t pre_time;
	static uint32_t preTime = 0;
	if(gStatus.mainLoop == 0)return;
	if(gError.bit.ssrCommError == 1)return;
	pre_time = millis();
	if(pre_time - preTime > 100)
	{
		
		if(modbusTBL.cm3.rxTime == modbusTBL.cm3.txTime)
		{
			if((pre_time > modbusTBL.cm3.rxTime + 50) && (gModBus.status == MODBUS_RECEIVE))
			{
				modbusTBL.cm3.rxTime = modbusTBL.cm3.txTime + 1;
				if(modbusTBL.cm3.commError < 10){
					preTime = pre_time;
					modbusTBL.cm3.commError++;
				}
				else
				{
					modbusTBL.is_open = 0;
					modbusTBL.cm3.commError = 0xff;
					gError.bit.ssrCommError = 1;
					buzzer_start();
					ssrControl(SSR_SSR, OFF);
					pStoreErrorMessage(Error_SSR_COMM_ERROR);
					gStatus.status = STATUS_INIT;
				}
				gModBus.status = MODBUS_READY;
				gModBus.rxMode = 0xff;
			}
		}
	
	}
}
void gReleaseError(uint8_t mode)
{
	SC3MD3 &= ~(char)0x10;
	delay(3);
	SC3MD3 |= 0x10;
	switch(mode)
	{
		case 0:
			modbusTBL.cm3.commError = 0;
			break;
		case MODBUS_SSR_TOP:
		case MODBUS_SSR_BOTTOM:
			//TODO ?
			gStatus.status = STATUS_READY;
	}
}
uint8_t modbusConnectCheck(void)
{
	uint8_t ret = 0,temp;
	uint32_t pre_time;
	
	
	SC3MD3 &= ~(char)0x10;
	delay(3);
	SC3MD3 |= 0x10;
	temp = modbusTBL.is_open;
	pRbFlush();
	
	pre_time = millis();
	while(gModBus.status == MODBUS_RECEIVE)
	{
		if(millis() - pre_time > 30)return 1;
		clear_WDT();
	}
	modbusTBL.is_open = 1;
	gModBus.p500msFlag = 1;
	gModBus.mode = MODBUS_MODE_MONITOR;
	
	gSetMonitor(0x0000,true); 
	
	modbusTransmit();
	delay(30);
	pre_time = millis();
	while(gModBus.status == MODBUS_RECEIVE)
	{
		if(millis() - pre_time > 100)
		{
			modbusTBL.is_open = 0;
			return 1;
		}
		clear_WDT();
	}
	return ret;
}
