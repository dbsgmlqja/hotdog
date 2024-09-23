#include "global.h"
#include "i2c.h"
#include "eeprom.h"
#include "led.h"
#include "uart.h"
#include "cli.h"
#ifdef _USE_HW_CLI
void cliEeprom(cli_args_t *args);
#endif
#define EEP_ID 0xCD
#define EEPROM_MAX_SIZE   2048
#define PAGE_SIZE	16
#define COUNTER_ADDR 0x60
#define PARAMETER_ADDR 0x10
#define IPARAMETER_ADDR 0x30
#define FAULT_ADDR 0x90
static uint8_t gTest;
static uint8_t is_init = 0;

const uint16_t EEP_HEAT_ParaTBL[][3]={ 
	{2000,	35000,	35000},		/* P0	FP0	TEMP1*/
	{2000,	35000,	35000},		/* P1	FP1	TEMP2*/
	{60,	 1800,	  600},		/* 2 Operating Time */
};
const uint16_t EEP_Inverter_ParaTBL[][3]={ 
	{5,	6000,	6000},			/* P0	FP0	Foward Frequency */
//	{5,	6000,	3000},			/* P1	FP1	Reverse Frequncy */
};
const uint8_t EEP_ERASE_DATA[16] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
extern void pKey_StopAction(void);
uint8_t eepromInit(void)
{
#ifdef _USE_HW_CLI
  cliAdd("eeprom", cliEeprom);
#endif
	i2cInit();
	is_init = 1;
	return is_init;
}
uint8_t eepromIsInit(void)
{
	return is_init;
}
uint8_t eepromValid(uint32_t addr)
{
	uint8_t ret;
	uint8_t data;
	if(addr >= EEPROM_MAX_SIZE)
	{
		return false;
	}
	ret = i2cReadByte(DEVICE_ADDR, addr & 0xFF, &data, 2);
	
	return ret;
}
uint8_t pn_EepromWriteByte(uint32_t addr,uint8_t data_in)
{
	uint8_t ret = 0;
	
	ret = i2cWriteByte(DEVICE_ADDR, addr, data_in, 10);
	return ret;
}
uint8_t eepromRead(uint32_t addr, uint8_t *p_data, uint8_t length)
{
	uint8_t ret = true;
	uint32_t i;
	uint8_t temp_REG;
	
	temp_REG = SC0IICMD3;
	temp_REG &= ~0x20;	// Stop Condition Disable
	temp_REG &= ~0x08;	// TX
	temp_REG &= ~0x01;	// ACK
	SC0IICMD3 = temp_REG;
	
	SC0IICMD0 |= 0x08;		// Start Condition Enable
	if(pIIC_SendByte(DEVICE_ADDR) == 0x01)	return 1;	// Slave Address
	SC0IICMD0 &= ~0x08;		// Start Condition Disable
	if(pIIC_SendByte(addr >> 8) == 0x01)		return 1;	// Sub Address High
	if(pIIC_SendByte(addr & 0xff) == 0x01)		return 1;	// Sub Address Low
	SC0IICMD0 |= 0x08;		// Start Condition Enable
	if(pIIC_SendByte(DEVICE_ADDR | 0x01) == 0x01)	return 1;	// Slave Address

	SC0IICMD0 &= ~0x08;		// Start Condition Disable
	
	temp_REG = SC0IICMD3;
	temp_REG |= 0x08;	// RX
	SC0IICMD3 = temp_REG;
	for(i=0; i<length -1; i++)
	{
		if(pIIC_SendByte(0x00) == 1)return 1;
		p_data[i] = SC0IICRB;
	}
	//----------------------------------------------------
	// Read Last Byte
	//----------------------------------------------------
	if(pIIC_SendByte_NACK(0x00) == 0x01)	return 1;	// Dummy Write(Read)
	p_data[i] = SC0IICRB;

	return ret;
}
uint8_t eepromWrite(uint32_t addr, uint8_t *p_data, uint8_t length)
{
	uint8_t ret;
	uint32_t i;
	
	for(i=0; i<length; i++)
	{
		ret = pn_EepromWriteByte(addr, p_data[i]);
		if(ret == false)
		{
			break;
		}
	}
	return ret;
}
uint32_t eepromGetLength(void)
{
	return EEPROM_MAX_SIZE;
}
uint8_t eepromFormat(void)
{
	return true;
}
void pn_EEP_LoadCount(void)
{
	
	i2cReadBytes(DEVICE_ADDR, 0xd0,(uint8_t *)&gBakeryCount, 4,10);

}
void pStoreErrorMessage(uint8_t ErrorMsg)
{
	uint8_t i;
	
	//Search Blank byte
	for(i=0;i!=10;i++){
		if(gMonitorError[i] == 0) break;
	}
	//Push Error Message
	if(i == 10){
		for(i=0;i!=10-1;i++){
			gMonitorError[i] = gMonitorError[i+1];
		}
		gMonitorError[9] = ErrorMsg;
	}
	else{
		gMonitorError[i] = ErrorMsg;
	}

	//Save EEPROM
	i2cWriteBytes(DEVICE_ADDR, 0x90,(uint8_t *)gMonitorError, 10,10);
	
}
void pn_EEP_Load(void)
{
	uint8_t i,ret;
	uint32_t data32;
	uint16_t *Ptr;
	ret = i2cReadByte(DEVICE_ADDR, 0x00, &gTest, 2);
	delay(10);

	/*@081021_4*/
	if(gTest == EEP_ID)
	{
		
		/*******************************************************/
		/* Read Door Counter                                                                */
		/* Address : 0xd0~0xd3                                                             */
		/*******************************************************/
		pn_EEP_LoadCount();
		if(gBakeryCount > 999999){
			gBakeryCount = 0;
			ret = i2cWriteBytes(DEVICE_ADDR, COUNTER_ADDR, (uint8_t *)&gBakeryCount, 4, 10);
		}
		i2cReadBytes(DEVICE_ADDR, FAULT_ADDR, (uint8_t *)&gMonitorError, 16, 10);
		pn_EEP_Read_Parameters();

	}
	
	/*******************************************************/
	/*                                                                                             */
	/* EEPROM INITIALIZE                                                               */
	/*                                                                                             */
	/*******************************************************/
	else{
		//Format memory
//		for(i=0;i!=0x0f;i++)i2cWriteBytes(DEVICE_ADDR, i<<4,(uint8_t *)&EEP_ERASE_DATA[0], 16, 10);
		//Counter
		data32 = 0;
		i2cWriteBytes(DEVICE_ADDR, COUNTER_ADDR, (uint8_t *)&data32, 4, 10);
		//Fault
		i2cWriteBytes(DEVICE_ADDR, FAULT_ADDR, (uint8_t *)&gMonitorError[0], 16, 10);
		//Parameter
		
		Ptr = (uint16_t *)&gParameter;
		for(i=0;i!=EEP_END;i++){
			*Ptr++ = EEP_HEAT_ParaTBL[i][2];
		}
		i2cWriteBytes(DEVICE_ADDR, PARAMETER_ADDR, (uint8_t *)&gParameter, EEP_END * 2, 10);
		
		Ptr = (uint16_t *)&gIParameter;
		for(i=0;i!=EEP_END;i++){
			*Ptr++ = EEP_Inverter_ParaTBL[i][2];
		}
		i2cWriteBytes(DEVICE_ADDR, IPARAMETER_ADDR, (uint8_t *)&gIParameter, EEP_INV_END * 2, 10);
		i2cWriteByte(DEVICE_ADDR, 0x00, EEP_ID,10);
	}
	pn_Load_Parameters();
}
static uint8_t Arrs[10];
static uint16_t Arr16[4];
uint8_t pn_EEP_WriteBytes(uint16_t addr, uint8_t *p_data, uint32_t length)
{
	return i2cWriteBytes(DEVICE_ADDR, addr, (uint8_t *)p_data, length, 10);
}
void pn_EEP_Write_Parameter(void)
{
	i2cWriteBytes(DEVICE_ADDR, PARAMETER_ADDR, (uint8_t *)&gParameter, EEP_END *2, 10);
}
void pn_EEP_Read_Parameters(void)
{
	uint8_t i,result = 0;
	uint16_t *Ptr,Min,Max;
	
	i2cReadBytes(DEVICE_ADDR, PARAMETER_ADDR, (uint8_t *)&gParameter, EEP_END *2, 10);
	Ptr = (uint16_t *)&gParameter;
	for(i=0;i!=EEP_END;i++)
	{
		Min = EEP_HEAT_ParaTBL[i][0];
		Max = EEP_HEAT_ParaTBL[i][1];
		if((*Ptr < Min) || (*Ptr > Max)){
			*Ptr = EEP_HEAT_ParaTBL[i][2];
			result |= 1;
		}
		*Ptr++;
	}
	i2cReadBytes(DEVICE_ADDR, IPARAMETER_ADDR, (uint8_t *)&gIParameter, EEP_INV_END *2, 10);
	Ptr = (uint16_t *)&gIParameter;
	for(i=0;i!=EEP_INV_END;i++)
	{
		Min = EEP_Inverter_ParaTBL[i][0];
		Max = EEP_Inverter_ParaTBL[i][1];
		if((*Ptr < Min) || (*Ptr > Max)){
			*Ptr = EEP_Inverter_ParaTBL[i][2];
			result |= 1;
		}
		*Ptr++;
	}
}
void pn_Load_Parameters(void)
{
	uint8_t index;
	uint16_t *Ptr;
	
	Ptr = (uint16_t *)&gParameter;
	Ptr += gStatus.Mode;
	index = gStatus.Mode;
	gDispParameter.Value = *Ptr;
	
	gDispParameter.Min = EEP_HEAT_ParaTBL[index][0];
	gDispParameter.Max = EEP_HEAT_ParaTBL[index][1];
	gStatus.Heater_Top.Comp = gParameter.TopTemp;
	gStatus.Heater_Bottom.Comp = gParameter.BottomTemp;
}

void pn_EEP_Init_Parameters(void){
	

	pn_EEP_Read_Parameters();
	
}
void pn_EEP_SaveCheck(void)
{
	uint32_t pre_time;
	pre_time = millis();
	if(gEEP_save == 1)
	{
		if(pre_time - gEEP_time > 2000){
			gParameter.TopTemp = gStatus.Heater_Top.Comp;
			gParameter.BottomTemp = gStatus.Heater_Bottom.Comp;
			i2cWriteBytes(DEVICE_ADDR, PARAMETER_ADDR, (uint8_t *)&gParameter, EEP_END *2, 10);
			gEEP_save = 0;
		}
	}
}
void pn_EEP_SaveCount(void)
{
	unsigned int TempCnt;

	
	TempCnt = gBakeryCount;
	
}

void pEEPROM_Backup(void)
{
	
}

uint8_t pn_EepromPageWrite(uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
	uint8_t i,j,len,pCnt = 0;
	uint8_t mod = reg_addr % PAGE_SIZE;
	uint8_t PageCnt = (length + mod - 1) / PAGE_SIZE + 1;
	uint8_t First,Last;
	uint16_t address = reg_addr;
	First = mod;
	len = length + mod;
	
	for(j=0;j<PageCnt;j++)
	{
		if(len > PAGE_SIZE)Last = PAGE_SIZE;
		else Last = len;
		i = Last - First;
		if(i2cWriteBytes(DEVICE_ADDR, address, (uint8_t *)&p_data[pCnt], i, timeout) == 1)return 1;
		pCnt += i;
		
		if(pCnt >= length)return 0;
		len -= PAGE_SIZE;
		First = 0;
		address = reg_addr + pCnt;
	}

	return 0;	// OK

}
#ifdef _USE_HW_CLI
void cliEeprom(cli_args_t *args)
{
  uint8_t ret = true,data8;
  uint32_t i;
  uint32_t addr;
  uint32_t length;
  uint16_t  data;
  uint32_t pre_time;
  uint8_t eep_ret;


  if (args->argc == 1)
  {
    if(args->isStr(0, "info") == true)
    {
      uartPrintf(_DEF_SERIAL1, "eeprom init   : %d\n", eepromIsInit());
      uartPrintf(_DEF_SERIAL1, "eeprom length : %d bytes\n", eepromGetLength());
    }
    else if(args->isStr(0, "format") == true)
    {
      if (eepromFormat() == true)
      {
        uartPrintf(_DEF_SERIAL1, "format OK\n");
      }
      else
      {
        uartPrintf(_DEF_SERIAL1, "format Fail\n");
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (args->argc == 3)
  {
    if(args->isStr(0, "read") == true)
    {
      addr   = (uint32_t)args->getData(1);
      length = (uint32_t)args->getData(2);

      if (length > eepromGetLength())
      {
        uartPrintf(_DEF_SERIAL1,  "length error\n");
      }
      for (i=0; i<length; i++)
      {
 //       eepromRead(addr+i,&data, 1);
		i2cReadByte(DEVICE_ADDR, addr+i, (uint8_t *)&data8, 2);
        uartPrintf(_DEF_SERIAL1,  "addr : 0x%03X\t 0x%02X\n", addr+i, data8);
      }
    }
    else if(args->isStr(0, "write") == true)
    {
      addr = (uint32_t)args->getData(1);
      data = (uint8_t )args->getData(2);

      pre_time = millis();
      eep_ret = pn_EepromWriteByte(addr, data);

      uartPrintf(_DEF_SERIAL1,  "addr : %d\t 0x%X %dms\n", addr, data, millis()-pre_time);
      if (!eep_ret)
      {
        uartPrintf(_DEF_SERIAL1, "OK\n");
      }
      else
      {
        uartPrintf(_DEF_SERIAL1, "FAIL\n");
      }
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    uartPrintf(_DEF_SERIAL1,  "eeprom info\n");
    uartPrintf(_DEF_SERIAL1,  "eeprom format\n");
    uartPrintf(_DEF_SERIAL1,  "eeprom read  [addr] [length]\n");
    uartPrintf(_DEF_SERIAL1,  "eeprom write [addr] [data]\n");
  }

}
#endif 
