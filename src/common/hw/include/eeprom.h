#ifndef _EEPROM_H_
#define _EEPROM_H_
#include "hw_def.h"
#ifdef _USE_HW_EEPROM
enum{
	EEP_TOP_TEMP,
	EEP_WARMUP_TIME,
	EEP_WARMUP_TEMP,
	EEP_END
};
enum{
	EEP_INV_F_FREQ,
	EEP_INV_R_FREQ,
	EEP_INV_DIR,
	EEP_INV_END
};
uint8_t eepromInit(void);
uint8_t eepromIsInit(void);
uint8_t eepromValid(uint32_t addr);
uint8_t eepromReadByte(uint32_t addr);
uint8_t pn_EepromWriteByte(uint32_t addr, uint8_t data_in);
uint8_t eepromWrite(uint32_t addr, uint8_t *p_data, uint8_t length);
uint32_t eepromGetLength(void);
uint8_t eepromFormat(void);
void pn_EEP_LoadCount(void);
void pn_EEP_OptionRead(void);
void pn_Option_Setting(uint8_t OPT);
void pn_EEP_Load(void);
void pn_EepromSetParameter(uint8_t Addr, uint8_t Data);
void pn_EEP_Init_Factory_Parameters(void);
void pn_EEP_Read_Parameters(void);
void pn_Load_Parameters(void);
void pEEPROM_Backup(void);
void pStoreErrorMessage(uint8_t ErrorMsg);
void pEEP_Save_Encoder(unsigned char EEP_ADDR,int Value);
void pn_EEP_Init_Parameters(void);
void pEEP_Save_All_Position(void);
uint8_t pn_EepromSparaAddr(void);
uint8_t pn_EepromUparaAddr(void);
void pn_EEP_SaveCheck(void);
void pn_EEP_Write_Parameter(void);
uint8_t pn_EepromPageWrite(uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);
uint8_t pn_EEP_WriteBytes(uint16_t addr, uint8_t *p_data, uint32_t length);
void gEraseFault(void);
uint16_t getFWPwm(void);
#endif
#endif
