#ifndef _KEY_H_
#define _KEY_H_

#include "hw_def.h"
#include "FND.h"
#define KEY_MAX_CH _HW_KEY_MAX_CH
typedef struct
{
  uint8_t  ch;
  uint8_t  state;
  uint32_t repeat_time;
  uint32_t pre_time;
} key_obj_t;

void keyInit(void);
void keyObjCreate(key_obj_t *p_obj, uint8_t ch, uint32_t repeat_time);
uint8_t keyObjGetPressed(key_obj_t *p_obj, uint32_t pressed_time);
uint8_t keyGetPressed(uint8_t ch);
void gModeKeyScan(void);
void gStartKeyScan(void);
void pDecKeyScan(void);

void pIncKeyScan(void);
void gKeyScan(void);
void gModeKeyReleaseCheck(void);
uint8_t gAllKeyReleaseCheck(void);
#endif
