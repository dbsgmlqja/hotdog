#include "FlashOption.h"
typedef struct{
	unsigned char Date[8];
	unsigned char ModelName[8];
	unsigned char Version[8];
	unsigned char Secrete[8];
}FRAM_Option;
// ----------------------
// Compile Option
// 1 : Read/Write
// 0 : Read Only
// 2 : Disable
// ----------------------
#define _FlashWriteOn 1
#define	FlashOptionAddr	0x4003BFE0
#define	FlashOptionSize	(4*8)

#if _FlashWriteOn
#pragma _section C=FlashOption
const unsigned char ROM_FlashOption[4][8]=
{
	{'2','0','2','2','1','2','2','7'},		//for Dates. 8bytes
	{'T','2','2','0','0','S','0','1'},		//for ModelName. 8bytes
	{'V','2','.','0','2','S','0','1'},		//for Version 8bytes
	{'*','T','E','L','E','C','S','*'}		//for Security Codes 8bytes
};
#pragma _section C
#else
#pragma _section C=FlashOption
#pragma _section C
#endif

FRAM_Option RAM_FlashOption;
/*
	Read Flash Option
*/
unsigned char pRead_FlashOption(void)
{
	uint8_t *src,*dst;
	uint8_t i, ret;
	
	ret = 0;
//	src = &ROM_FlashOption[0][0];
	src = (uint8_t *)FlashOptionAddr;
	dst = &RAM_FlashOption.Date[0];
	
	for(i=0;i!=FlashOptionSize;i++)
	{
		*dst = *src;
		dst++;
		src++;
	}
	//[User Option] Compare with security code. 
	if(RAM_FlashOption.Secrete[0] != '*') ret |= 1;
	if(RAM_FlashOption.Secrete[1] != 'T') ret |= 1;
	if(RAM_FlashOption.Secrete[2] != 'E') ret |= 1;
	if(RAM_FlashOption.Secrete[3] != 'L') ret |= 1;
	if(RAM_FlashOption.Secrete[4] != 'E') ret |= 1;
	if(RAM_FlashOption.Secrete[5] != 'C') ret |= 1;
	if(RAM_FlashOption.Secrete[7] != '*') ret |= 1;

#if (_FlashWriteOn == 2)
	return 0;
#else
	if(ret == 0){
		return 0;
	}
	else{
		//FAILED
		return 1;
	}
#endif
}
void pRead_FlashVersion(int8_t *Arr)
{
	uint8_t i;
	for(i=0;i!=8;i++)
	{
		Arr[i] = RAM_FlashOption.Version[i];
	}
}


