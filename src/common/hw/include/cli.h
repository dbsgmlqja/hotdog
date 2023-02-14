#ifndef SRC_COMMON_HW_INCLUDE_CLI_H_
#define SRC_COMMON_HW_INCLUDE_CLI_H_

#include "hw_def.h"


#ifdef _USE_HW_CLI

#define CLI_CMD_LIST_MAX      HW_CLI_CMD_LIST_MAX
#define CLI_CMD_NAME_MAX      HW_CLI_CMD_NAME_MAX

#define CLI_LINE_HIS_MAX      HW_CLI_LINE_HIS_MAX
#define CLI_LINE_BUF_MAX      HW_CLI_LINE_BUF_MAX




typedef struct
{
  uint16_t   argc;
  char     **argv;

  int32_t  (*getData)(uint8_t index);
  float    (*getFloat)(uint8_t index);
  char    *(*getStr)(uint8_t index);
  uint8_t     (*isStr)(uint8_t index, char *p_str);
} cli_args_t;


uint8_t cliInit(void);
uint8_t cliOpen(uint8_t ch, uint32_t baud);
uint8_t cliOpenLog(uint8_t ch, uint32_t baud);
uint8_t cliMain(void);
void cliPrintf(const char *fmt, ...);
uint8_t cliAdd(const char *cmd_str, void (*p_func)(cli_args_t *));
uint8_t cliKeepLoop(void);
uint32_t cliAvailable(void);
uint8_t  cliRead(void);
uint32_t cliWrite(uint8_t *p_data);
uint8_t gGetCliCh(void);
#endif


#endif /* SRC_COMMON_HW_INCLUDE_CLI_H_ */
