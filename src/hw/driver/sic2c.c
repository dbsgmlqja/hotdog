#include "si2c.h"

#define	EEP_SCL_OUT	((*(RB_BTF*)&P0OUT).p2)
#define	EEP_SDA_OUT	((*(RB_BTF*)&P0OUT).p1)
#define EEP_SDA_IN  ((*(RB_BTF*)&P0IN).p1)
static device_addr = 0xa0;
uint8_t gEEP_ADDR;
void si2cInit(void)
{
	G42ICR &= ~0x0300;
	P0MD &= ~0x0330;
	P0DIR |= 0x06;
}
void SI2C_Delay(void)
{
	uint8_t i;
	for(i=0;i!=5;++i){
		asm(" nop");
		/* WDT reset */
		WDCTR |= 0x40;			/* reset WDT binary counter */
	}
}
void si2c_Start(void)
{
	P0DIR |= 0x06;
	EEP_SCL_OUT = 1;
	EEP_SDA_OUT = 1;
	
	SI2C_Delay();
	EEP_SDA_OUT = 0;
	SI2C_Delay();
	EEP_SCL_OUT = 0;
	SI2C_Delay();
}
void si2cStop(void)
{
	P0DIR |= 0x06;
	EEP_SCL_OUT = 0;
	EEP_SDA_OUT = 0;
	SI2C_Delay();
	EEP_SCL_OUT = 1;
	SI2C_Delay();
	EEP_SDA_OUT = 1;
	SI2C_Delay();
}
void si2cWrite(uint8_t Data)
{
	uint8_t i;
	for(i=0; i!=8; i++)
	{
		if(Data & 0x80) EEP_SDA_OUT = 1;
		else 			EEP_SDA_OUT = 0;
		SI2C_Delay();
		EEP_SCL_OUT = 1;
		SI2C_Delay();
		EEP_SCL_OUT = 0;
		asm(" nop");
		asm(" nop");
		Data <<= 1;
	}
}
uint8_t SI2C_Read(void)
{
	uint8_t i,Data = 0;
	P0DIR &= ~0x02;
	asm(" nop");
	asm(" nop");
	
	for(i=0; i!=8;i++)
	{
		Data <<= 1;
		asm(" nop");
		asm(" nop");
		EEP_SCL_OUT = 1;
		SI2C_Delay();
		if(EEP_SDA_IN) Data |= 0x01;
		EEP_SCL_OUT = 0;
		SI2C_Delay();
	}
	return Data;
}
uint8_t si2cAck(void)
{
	uint8_t Err = 0x00;
	uint32_t pre_time = millis();
	
	P0DIR &= ~0x02;
	P0DIR |= 0x04;
	
	SI2C_Delay();
	EEP_SCL_OUT = 1;
	while(EEP_SDA_IN){
		asm(" nop");
		P2DIR &= ~0x02;
		if(millis() - pre_time > 50)
		{
			Err = 0xff;
			break;
		}
		WDCTR |= 040;
	}
	P0DIR |= 0x06;
	EEP_SCL_OUT = 0;
	EEP_SDA_OUT = 0;
	SI2C_Delay();
	
	return Err;
	
}
uint8_t SI2C_ReadByte(uint8_t addr, uint8_t *data, uint8_t length)
{
	uint8_t Err,ret = false;
	si2c_Start();
	si2cWrite(device_addr);
	Err = si2cAck();
	if(Err == 0x00)
	{
		si2cWrite(addr);
		si2cAck();
		si2c_Start();
		si2cWrite(device_addr | 0x01);
		si2cAck();
		*data = SI2C_Read();
		si2cStop();
		delay(1);
		
		ret = true;
	}
	else
	{
		delay(1);
	}
	return ret;
}
void si2cWriteByte(uint8_t addr1, uint8_t *Data, uint8_t length)
{
	uint8_t i;
	for(i = 0; i != length; i++)
	{
		si2c_Start();
		si2cWrite(device_addr);
		si2cAck();
		si2cWrite(addr1);
		si2cAck();
		si2cWrite(*Data++);
		si2cAck();
		si2cStop();
		delay(20);
	}
}
void SI2C_Write_Verify(uint8_t addr, uint8_t Data)
{
	uint8_t VData, i, ret;
	for(i=addr; i!=0x80;i++)
	{
		si2cWriteByte(i, &Data, 1);
		delay(2);
		ret = SI2C_ReadByte(i,&VData, 1);
		if(VData == Data)break;
		delay(2);
	}
	if(gEEP_ADDR != addr)
	{
		gEEP_ADDR = addr;
		si2cWriteByte(0x02, &gEEP_ADDR, 1);
	}
}
