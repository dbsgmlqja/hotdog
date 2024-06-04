#include "hw_def.h"
#include <stdarg.h>
#include "global.h"
#include "uart.h"
#include "qbuffer.h"

#define UART_RX_BUF_LENGTH	256
extern void init_WDT(void);
typedef struct
{
	uint8_t     is_open;
	uint32_t baud;
	uint8_t rx_buf[UART_RX_BUF_LENGTH];
	uint8_t rx_data;
	
	qbuffer_t qbuffer_rx;


} uart_t;

static uart_t uart_tbl[4];


#define UART_MAX_BUF_SIZE    256

void uartInit(void)
{
	SC1MD3 &= ~(char)0x30;
	TM00BR = (8-1);//8 - 115200bps,8.681uS, 195 - 9600bps,104uS 
	TM00MD |= 0x40;
	TM00MD &= ~0x40;
	
	SC1MD3 = 0x38;
	P2MD |= 0x102000;
	P2DIR |= 0x08;
	P2ODC |= 0x08;
	SC1MD0 = 0x18;
	SC1MD2 = 0x88;
	SC1MD1 = 0x03;
	SC1MD1 |= 0x30;
	TM00MD |= 0x80;
	
	G43ICR = 0x4100;
	uart_tbl[1].is_open = 1;
	qbufferCreate(&uart_tbl[1].qbuffer_rx, (uint8_t *)&uart_tbl[1].rx_buf, UART_RX_BUF_LENGTH);
	
	

	uart_tbl[3].is_open = 1;
//	qbufferCreate(&uart_tbl[1].qbuffer_rx, (uint8_t *)&uart_tbl[1].rx_buf, UART_RX_BUF_LENGTH);
}
uint8_t getDigit(uint32_t Data, uint8_t Notation)
{
	uint8_t ret = 1,i;
	
	switch(Notation)
	{
		case 16:
			for(i=0;i<=7;i++)
			{
				if(Data & (0xf << (28-i*4)))
				{
					ret = 8-i;
					break;
				}
			}
			break;
	}
	return ret;
}
uint32_t uartAvailable(uint8_t ch)
{
  uint32_t ret = 0;

  switch(ch)
  {
    case _DEF_UART1:
     
      break;
	case _DEF_SERIAL1:
		ret = qbufferAvailable(&uart_tbl[ch].qbuffer_rx);
		break;
	case _DEF_SERIAL2:
		
		break;
	case _DEF_SERIAL3:
		ret = qbufferAvailable(&uart_tbl[ch].qbuffer_rx);
		break;

  }

  return ret;
}



uint8_t uartRead(uint8_t ch)
{
  uint8_t ret = 0;

  switch(ch)
  {
    case _DEF_SERIAL0:
     
      break;
	case _DEF_SERIAL1:
     qbufferRead(&uart_tbl[ch].qbuffer_rx, &ret, 1);
      break;
	case _DEF_SERIAL2:
     qbufferRead(&uart_tbl[ch].qbuffer_rx, &ret, 1);
      break;
	case _DEF_SERIAL3:
     qbufferRead(&uart_tbl[ch].qbuffer_rx, &ret, 1);
      break;

  }

  return ret;
}

uint32_t uartWrite(uint8_t ch, uint8_t Data)
{
	uint32_t ret = 0,i;
	uint32_t pre_time;
	pre_time = millis();
	switch(ch)
	{
		case _DEF_SERIAL0:
			//Not Available, IIC Used
			break;
		case _DEF_SERIAL1:
		  	while((SC1STR & 0x20) == 0x20);
			SC1TB = Data;
			break;
		case _DEF_SERIAL2:
			while((SC2STR & 0x20) == 0x20);
			SC2TB = Data;
			break;
		case _DEF_UART4:
			while(SC3STR & 0x20 == 0x20);
			
			SC3TB = Data;
			for(i=0;i<0x6ff;i++)clear_WDT();
			break;
	}
	return ret;
}

void printNum(uint8_t ch, unsigned long num, int base)
{
    /*        */
	if (num == 0)
    {
        return;
    }
    
    /*      */
	printNum(ch, num/base, base);

	/*        */
    uartWrite(ch, "0123456789ABCDEF"[num%base]);
}

void printHex(uint8_t ch,unsigned hex)
{
    if (hex == 0)			//      0   
    {
        uartWrite(ch, '0');
		return;
    }
    else
    {
        printNum(ch, hex,16);	//        
    }
}
void printDeci(uint8_t ch,int dec)
{
    
    /*                */
	if (dec < 0)
    {
        uartWrite(ch,'-');
		dec = -dec;  	   //          :            
    }

    /*       0    */
    if (dec == 0)
    {
        uartWrite(ch,'0');
		return;
    }
    else
    {
        printNum(ch, dec, 10); //       
    }
}
uint32_t  uartPrintf(uint8_t ch, char *fmt, ...)
{
	uint8_t Arr = 0,i,digit = 0,d2=0;
	unsigned Var1 = 0;
	va_list va_ptr;
	va_start(va_ptr,fmt);
	while(*fmt){
		
		if(*fmt == '%'){
			fmt++;
			switch(*fmt){
				case 'c':
					uartWrite(ch, va_arg(va_ptr,int));
					break;
				case 'd':
					printDeci(ch, va_arg(va_ptr,int)); 
					break;
				case '%':
					
					break;
				case 'X':
				//	printDeci(ch, Arr + 0x30);
					printHex(ch, va_arg(va_ptr,unsigned int)); 
					break;
				case '0':
					fmt++;
					if((*fmt >= 0x30) && (*fmt <=0x39))digit = *fmt - 0x30;
					fmt++;
					switch(*fmt){
						case 'c':
							uartWrite(ch, va_arg(va_ptr,int));
							break;
						case 'd':
							printDeci(ch, va_arg(va_ptr,int)); 
							break;
						case '%':
							
							break;
						case 'X':
							Var1 = va_arg(va_ptr, unsigned short);
							Arr = getDigit(Var1, 16);
							d2 = digit - Arr;
							if(d2>8)d2=8;
							else if(d2<1)d2 = 0;
							
							for(i=0;i!=d2;i++)uartWrite(ch, '0');
							
							printHex(ch, Var1); 
							break;
						
					}
					break;
			}
		}
		else{
			uartWrite(ch, *fmt);
		}
		fmt++;
	}
	va_end(va_ptr);
	return Arr;
}
void U_Printf(uint8_t ch, uint8_t *Text)
{
	switch(ch)
	{
		case _DEF_UART1:
		
		break;
		case _DEF_UART2:
			while(SC1STR & 0x20){
				clear_WDT();
			};
			while(*Text != 0){
				SC1TB = *Text;
				while(SC1STR & 0x20);
				++Text;
			}
		break;
		case _DEF_UART4:
			while(SC3STR & 0x20){
				clear_WDT();
			};
			while(*Text != 0){
				SC3TB = *Text;
				while(SC3STR & 0x20);
				++Text;
			}
		break;
	}
}
uint32_t my_pow(uint8_t Base,uint32_t Pow)
{
	int i,j;
	j = 1;
	for(i=0;i<Pow;i++)j=j*Base;
	return j;
}
void U_Printf_Num(uint8_t ch, uint32_t Data)
{
	unsigned char i, Cnt = 0, Cmp = 0;
	unsigned int n;
	unsigned char Value;
	unsigned char Str[11] = {
		0,
	};
	for (i = 0; i != 10; i++) {
		n = my_pow(10, (9 - i));
		Value = Data / n;
		if (Value != 0) {
			Str[Cnt++] = Value + 0x30;
			Cmp |= 0x01;
		} else {
			if (Cmp != 0) {
				Str[Cnt++] = 0x30;
			}
		}
		Data %= n;
		
	}
	//Str[5] = 0x00;
	switch(ch)
	{
		case _DEF_UART1:
		
		break;
		case _DEF_UART2:
			while(SC1STR & 0x20){
				clear_WDT();
			};
			i=0;
			while(Str[i] != 0){
				SC1TB = Str[i];
				while(SC1STR & 0x20);
				++i;
			}
		//	SC1TB = 0x0d;
		break;
		case _DEF_UART4:
			while(SC3STR & 0x20);
			while(Str[i] != 0){
				SC3TB = Str[i];
				while(SC3STR & 0x20);
				++i;
			}
			SC3TB = 0x0d;
			break;
	}


}
uint32_t uartGetBaud(uint8_t ch)
{
  uint32_t ret = 0;


  switch(ch)
  {
    case _DEF_UART1:
      
      break;


    case _DEF_UART2:
    case _DEF_UART3:
      
      break;

  }

  return ret;
}
void uartWriteStatus(uint16_t val,uint16_t Data, uint16_t Data2)
{
	uartPrintf(_DEF_SERIAL1, "%d,%d,%d\n",val, Data,Data2);
}
void UART1_IRQHandler(void)
{
	uint8_t RxBuffer;
	uart_t *p_uart = &uart_tbl[1];
	
	RxBuffer = SC1RB;
	qbufferWrite(&p_uart->qbuffer_rx, &RxBuffer, 1);
}

