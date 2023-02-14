#include "global.h"
#include "i2c.h"

#define PAGESIZE	16
#define	EEP_SLAVE_ADDR	0xA0
static uint32_t pTimeout = 10;
void i2cInit(void)
{
	//@200519 kishida-san 9
	P2OUT = 0x00;

	P2DIR |= 0x03;	//P20, P21 Output

	P2MD |= 0x0003;	//P20 : SCL0_B
	P2MD |= 0x0030;	//P21 : SDA0_B

	P2PLU = 0x00;	//P20,P21 No PullUp 
	
	P2ODC |= 0x03;	//P20, P21 Open Drain
	
	
	/* The following operations is incomplete,please read the LSI instruction */
	G41ICR &= ~(short)0x0100;
	G41ICR &= ~(short)0x0200;
	G42ICR &= ~(short)0x0100;
	G42ICR &= ~(short)0x0200;
	
	
	// P20, P21 All High
	P2OUT |= 0x03;
	
	//Timer 3
	TM03BR = 0x4 - 0x01;
	TM03MD = 0x40;		// b6 : 1 Timer3 Init
						// b2-0 : 000 IOCLK
	TM03MD &= ~0x40;		// Timer3 Init Release
		
	
	TM03MD |= 0x80;		// Timer 3 Start
	
	//Clock Setting
	SIFCLK0 |= 0x03;		// Serial 0 : Timer 3
	
	
	// Serial 0 : IIC
	SC0SEL = 0x01;
	
	// Serial 0 Mode 2
	SC0IICMD2 = 0x00;	// b5 : 0 Reset
	SC0IICMD2 |= 0x08;	// b3 : 1 Prescaler Enable
	SC0IICMD2 |= 0x03;	// b2-b0 : 3 Timer/32

	//Serial 0 Mode 3
	SC0IICMD3 = 0x22;	// b5 Stop Condition : Enable
						// b4 Speed : Standard Mode
						// b3 Tx, Rx Mode Select : Tx
						// b2 : 0
						// b1 : 1
						// b0 : ACK/NACK " ACK transmission
	
	SC0IICMD0 = 0x2F;	// b7-6 : 00
						// b5 : 1 start condition detection : Enable
						// b4 : 0 MSB first
						// b3 : 1 Start Condition generation : Enable
						// b2-b0 : 111
	
	SC0IICMD1 = 0xF4;	// b7-4 : 1111
						// b3 : 0
						// b2 : 1 Master Mode
						// b1-0 : 00
	

//@200518 - modify
//	SC0IICSTR1 &= ~(char)0x01;	// b0 : Error Detection On
	SC0IICDATA_ERR = 0;
	SC0IICABT_LST = 0;
	
	SC0IICAD = 0xA0;


	//G42
	G42ICR = 0x1000;
	G42ICR |= 0x0300;


	// Reset Release
//@200519 - Kishida-san caution 1
//	SC0IICMD2 = 0x28 | 0x03;
	SC0IICMD2 = 0x08 | 0x03;
	SC0IICMD2 |= 0x20;
	
	
}

void i2cReset(void)
{
	
}
/***************************************************************************
 * Function    : Delay(Software)
 ***************************************************************************/
void pDelay_Soft_5ms(void)
{
	uint32_t i;

	for(i=0;i!=17500;i++)
	{
		asm(" nop ");	asm(" nop ");
		asm(" nop ");	asm(" nop ");
		//asm(" nop ");	asm(" nop ");
		clear_WDT();
	}
}
/***************************************************************************
 * Function    : pIIC_SendByte
   Return
     0 - Send OK
     1 - Send fail
 ***************************************************************************/
uint8_t pIIC_SendByte(uint8_t Byte)
{
	uint32_t pre_time = millis();
		
	//----------------------------------------------------
	// Slave Address
	//----------------------------------------------------
	IIC_TX_Complete = 0;
	SC0IICTB = Byte;
	while(IIC_TX_Complete == 0)
	{
		clear_WDT();
		if(millis()-pre_time > pTimeout) return 1;		// Timeout Error
	}
	
	if(SC0IICDATA_ERR != 0) return 1;
	
	return(pCheckACK());
}


/***************************************************************************
 * Function    : pIIC_SendByte_NACK
   Return
     0 - NACK OK
     1 - Send fail
 ***************************************************************************/
uint8_t pIIC_SendByte_NACK(uint8_t Byte)
{
	uint32_t pre_time = millis();
	uint8_t temp_REG;
	
	//@200519 kishida-san 4
//	SC0IICMD3 |= 0x01;	// NACK
	temp_REG = SC0IICMD3;
	temp_REG |= 0x01;	// NACK
	SC0IICMD3 = temp_REG;
	//----------------------------------------------------
	// Slave Address
	//----------------------------------------------------
	IIC_TX_Complete = 0;
	SC0IICTB = Byte;
	while(IIC_TX_Complete == 0)
	{
		if(millis()-pre_time > pTimeout) return 1;		// Timeout Error
		clear_WDT();
	}
	return 0;
}

/***************************************************************************
 * Function    : Page Write
   Return
     0 - Write Success
     1 - Write Fail
 ***************************************************************************/


/***************************************************************************
 * Function    : pIIC_Connect_Check
   Return
     0 - Connect OK
     1 - Connect NG
 ***************************************************************************/
uint8_t pIIC_Connect_Check(void)
{

	uint8_t Err_Cnt = 0;
	uint8_t temp_REG;

	temp_REG = SC0IICMD3;
	temp_REG &= ~0x20;	// Stop Condition Disable
	temp_REG &= ~0x08;	// TX
	temp_REG &= ~0x01;	// ACK
	SC0IICMD3 = temp_REG;

	SC0IICMD0 |= 0x08;		// Start Condition Enable
	if(pIIC_SendByte(EEP_SLAVE_ADDR) == 0x01)	Err_Cnt++;	// Slave Address

	temp_REG = SC0IICMD3;
	temp_REG |= 0x20;	// Stop Condition Enable
	SC0IICMD3 = temp_REG;

	SC0IICMD0 |= 0x08;		// Start Condition Enable
	if(pIIC_SendByte(EEP_SLAVE_ADDR) == 0x01)	Err_Cnt++;	// Slave Address

	temp_REG = SC0IICMD3;
	temp_REG |= 0x20;	// Stop Condition Enable
	SC0IICMD3 = temp_REG;

	SC0IICMD0 |= 0x08;		// Start Condition Enable
	if(pIIC_SendByte(EEP_SLAVE_ADDR) == 0x01)	Err_Cnt++;	// Slave Address

	temp_REG = SC0IICMD3;
	temp_REG |= 0x20;	// Stop Condition Enable
	SC0IICMD3 = temp_REG;



	if(Err_Cnt > 2)	return 1;	//No Connect
	else return 0;
}
uint8_t i2cRecovery(void)
{
	uint8_t ret = true;
	return ret;
}
uint8_t i2cReadByte (uint16_t dev_addr, uint16_t reg_addr, uint8_t *data, uint32_t timeout)
{
	 return i2cReadBytes(dev_addr, reg_addr, data, 1, timeout);
}
uint8_t i2cReadBytes(uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
	uint8_t i = 0;
	//@200519 kishida-san 4
	uint8_t temp_REG;
	pTimeout = timeout;
	temp_REG = SC0IICMD3;
	temp_REG &= ~0x20;	// Stop Condition Disable
	temp_REG &= ~0x08;	// TX
	temp_REG &= ~0x01;	// ACK
	SC0IICMD3 = temp_REG;

	SC0IICMD0 |= 0x08;		// Start Condition Enable
	if(pIIC_SendByte(dev_addr) == 0x01)	return 1;	// Slave Address
	SC0IICMD0 &= ~0x08;		// Start Condition Disable
//	if(pIIC_SendByte(IIC.SubAddrH) == 0x01)		return 1;	// Sub Address High
	if(pIIC_SendByte(reg_addr) == 0x01)		return 1;	// Sub Address Low
	SC0IICMD0 |= 0x08;		// Start Condition Enable
	if(pIIC_SendByte(dev_addr | 0x01) == 0x01)	return 1;	// Slave Address

	SC0IICMD0 &= ~0x08;		// Start Condition Disable
	temp_REG = SC0IICMD3;
	temp_REG |= 0x08;	// RX
	SC0IICMD3 = temp_REG;
	
	for(i = 0;i < length-1;i++)
	{
		if(pIIC_SendByte(0x00) == 0x01)	return 1;	// Dummy Write(Read)
		*p_data++ = SC0IICRB;
//		clear_WDT();
	}
	//----------------------------------------------------
	// Read Last Byte
	//----------------------------------------------------
	if(pIIC_SendByte_NACK(0x00) == 0x01)	return 1;	// Dummy Write(Read)
	*p_data = SC0IICRB;

	temp_REG = SC0IICMD3;
	temp_REG |= 0x20;	// Stop Condition Enable
	SC0IICMD3 = temp_REG;

	return 0;	// OK
}

uint8_t i2cWriteByte (uint16_t dev_addr, uint16_t reg_addr, uint8_t data, uint32_t timeout)
{
	return i2cWriteBytes(dev_addr,reg_addr,&data,1,timeout);
}
uint8_t i2cWriteBytes(uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
	uint8_t i,j,len,pCnt = 0;
	uint8_t mod = reg_addr % PAGESIZE;
	uint8_t PageCnt = (length + mod - 1) / PAGESIZE + 1;
	uint8_t First,Last;
	uint8_t temp_REG;
	uint16_t address = reg_addr;
	pTimeout = timeout;
	First = mod;
	len = length + mod;
	
	
	for(j=0;j<PageCnt;j++)
	{
		if(len > PAGESIZE)Last = PAGESIZE;
		else Last = len;
		temp_REG = SC0IICMD3;
		temp_REG &= ~0x20;	// Stop Condition Disable
		temp_REG &= ~0x08;	// TX
		temp_REG &= ~0x01;	// ACK
		SC0IICMD3 = temp_REG;
	
		SC0IICMD0 |= 0x08;		// Start Condition Enable
		if(pIIC_SendByte(dev_addr | ((address >> 7) & 0x0e)) == 0x01)	return 1;
		SC0IICMD0 &= ~0x08;		// Start Condition Disable
		if(pIIC_SendByte(address & 0xff) == 0x01)							return 1;
		for(i=First;i<Last;i++)
		{
			if(pIIC_SendByte(*p_data++) == 0x01)return 1;
			pCnt++;
		}
		SC0IICMD3 |= 0x20;	// Stop Condition Enable
		pDelay_Soft_5ms();
		if(pCnt >= length)return 0;
		len -= PAGESIZE;
		First = 0;
		address = reg_addr + pCnt;
	}

	return 0;	// OK

}

uint8_t pCheckACK(void)
{
	if(SC0IICACKO_RCV == 0x00)	return 0;	// ACK OK
	else						return 1;	// ACK NG
}
