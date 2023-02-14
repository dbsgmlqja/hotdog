#include "cli.h"
#include "uart.h"
#include <string.h>
#include <stdlib.h>
#ifdef _USE_HW_CLI


#define CLI_KEY_BACK              0x08
#define CLI_KEY_DEL               0x7F
#define CLI_KEY_ENTER             0x0D
#define CLI_KEY_ESC               0x1B
#define CLI_KEY_LEFT              0x44
#define CLI_KEY_RIGHT             0x43
#define CLI_KEY_UP                0x41
#define CLI_KEY_DOWN              0x42
#define CLI_KEY_HOME              0x31
#define CLI_KEY_END               0x34

#define CLI_PROMPT_STR            "cli# "

#define CLI_ARGS_MAX              32
#define CLI_PRINT_BUF_MAX         256

enum
{
  CLI_RX_IDLE,
  CLI_RX_SP1,
  CLI_RX_SP2,
  CLI_RX_SP3,
  CLI_RX_SP4
};


typedef struct
{
  char   cmd_str[CLI_CMD_NAME_MAX];
  void (*cmd_func)(cli_args_t *);
} cli_cmd_t;


typedef struct
{
  uint8_t buf[CLI_LINE_BUF_MAX];
  uint8_t buf_len;
  uint8_t cursor;
  uint8_t count;
} cli_line_t;


typedef struct
{
  uint8_t  ch;
  uint32_t baud;
  uint8_t     is_open;
  uint8_t     is_log;
  uint8_t  log_ch;
  uint32_t log_baud;
  uint8_t  state;
  char     print_buffer[CLI_PRINT_BUF_MAX];
  uint16_t  argc;
  char     *argv[CLI_ARGS_MAX];


  uint8_t        hist_line_new;
  int8_t      hist_line_i;
  uint8_t     hist_line_last;
  uint8_t     hist_line_count;

  cli_line_t  line_buf[CLI_LINE_HIS_MAX];
  cli_line_t  line;

  uint16_t    cmd_count;
  cli_cmd_t   cmd_list[CLI_CMD_LIST_MAX];
  cli_args_t  cmd_args;
} cli_t;



cli_t cli_node;

cli_t cli_node2;

static uint8_t cliUpdate(cli_t *p_cli, uint8_t rx_data);
static void cliLineClean(cli_t *p_cli);
static void cliLineAdd(cli_t *p_cli);
static void cliLineChange(cli_t *p_cli, int8_t key_up);
static void cliShowPrompt(cli_t *p_cli);
static void cliToUpper(char *str);
static uint8_t cliRunCmd(cli_t *p_cli);
static uint8_t cliParseArgs(cli_t *p_cli);

static int32_t  cliArgsGetData(uint8_t index);
static float    cliArgsGetFloat(uint8_t index);
static char    *cliArgsGetStr(uint8_t index);
static uint8_t     cliArgsIsStr(uint8_t index, char *p_str);


void cliShowList(cli_args_t *args);
void cliMemoryDump(cli_args_t *args);


uint8_t cliInit(void)
{
	cli_node.is_open = false;
	cli_node.is_log  = false;
	cli_node.state   = CLI_RX_IDLE;
	cli_node.ch = 1;
	cli_node.hist_line_i     = 0;
	cli_node.hist_line_last  = 0;
	cli_node.hist_line_count = 0;
	cli_node.hist_line_new   = false;
	
	cli_node.cmd_args.getData  = cliArgsGetData;
	cli_node.cmd_args.getFloat = cliArgsGetFloat;
	cli_node.cmd_args.getStr   = cliArgsGetStr;
	cli_node.cmd_args.isStr    = cliArgsIsStr;
	
	cliLineClean(&cli_node);
		
	cliAdd("help", cliShowList);
	cliAdd("md"  , cliMemoryDump);
	
	return true;
}

uint8_t cliOpen(uint8_t ch, uint32_t baud)
{
	cli_node.ch = ch;
	cli_node.baud = baud;
	cli_node.log_ch = ch;
	cli_node.is_open = 1;//uartOpen(ch, baud);
	return cli_node.is_open;
}
char *strtok_r (char *s, const char *delim, char **save_ptr)
{   
  char *end;
  
  if (s == NULL)
    s = *save_ptr;

  if (*s == '\0')
    {
      *save_ptr = s;
      return NULL;
    }
    
  /* Scan leading delimiters.  */
  s += strspn (s, delim);
  if (*s == '\0')
    {
      *save_ptr = s;
      return NULL;
    }

  /* Find the end of the token.  */
  end = s + strcspn (s, delim);
  if (*end == '\0')
    {
      *save_ptr = end;
      return s;
    }

  /* Terminate the token and make *SAVE_PTR point past it.  */
  *end = '\0';
  *save_ptr = end + 1;
  return s;
}
uint8_t cliOpenLog(uint8_t ch, uint32_t baud)
{
  uint8_t ret;

  cli_node.log_ch = ch;
  cli_node.log_baud = baud;

  ret = 1;//uartOpen(ch, baud);

  if (ret == true)
  {
    cli_node.is_log = true;
  }
  return ret;
}

uint8_t cliLogClose(void)
{
  cli_node.is_log = false;
  return true;
}

void cliShowLog(cli_t *p_cli)
{
  int i;
  if (cli_node.is_log == true)
  {
    uartPrintf(p_cli->log_ch, "Cursor  : %d\n", p_cli->line.cursor);
    uartPrintf(p_cli->log_ch, "Count   : %d\n", p_cli->line.count);
    uartPrintf(p_cli->log_ch, "buf_len : %d\n", p_cli->line.buf_len);
    uartPrintf(p_cli->log_ch, "buf     : %s\n", p_cli->line.buf);
    uartPrintf(p_cli->log_ch, "line_i  : %d\n", p_cli->hist_line_i);
    uartPrintf(p_cli->log_ch, "line_lt : %d\n", p_cli->hist_line_last);
    uartPrintf(p_cli->log_ch, "line_c  : %d\n", p_cli->hist_line_count);

    for (i=0; i<p_cli->hist_line_count; i++)
    {
      uartPrintf(p_cli->log_ch, "buf %d   : %s\n", i, p_cli->line_buf[i].buf);
    }
    uartPrintf(p_cli->log_ch, "\n");
  }
}

void cliShowPrompt(cli_t *p_cli)
{
  uartPrintf(p_cli->ch, "\n\r");
  uartPrintf(p_cli->ch, CLI_PROMPT_STR);
}

uint8_t cliMain(void)
{
  if (cli_node.is_open != true)
  {
    return false;
  }

  if (uartAvailable(cli_node.ch) > 0)
  {
    cliUpdate(&cli_node, uartRead(cli_node.ch));
  }

  return true;
}

uint32_t cliAvailable(void)
{
  return uartAvailable(cli_node.ch);
}

uint8_t cliRead(void)
{
  return uartRead(cli_node.ch);
}

uint32_t cliWrite(uint8_t *p_data)
{
  return uartWrite(cli_node.ch, (uint8_t)p_data);
}

uint8_t cliUpdate(cli_t *p_cli, uint8_t rx_data)
{
  uint8_t ret = false;
  uint8_t tx_buf[8];
  cli_line_t *line;
	int i;
  line = &p_cli->line;


  if (p_cli->state == CLI_RX_IDLE)
  {
    switch(rx_data)
    {
      // ????
      //
      case CLI_KEY_ENTER:
        if (line->count > 0)
        {
          cliLineAdd(p_cli);
          cliRunCmd(p_cli);
        }

        line->count = 0;
        line->cursor = 0;
        line->buf[0] = 0;
        cliShowPrompt(p_cli);
        break;


      case CLI_KEY_ESC:
        p_cli->state = CLI_RX_SP1;
        break;


      // DEL
      //
      case CLI_KEY_DEL:
        if (line->cursor < line->count)
        {
          uint8_t mov_len;

          mov_len = line->count - line->cursor;
          for (i=1; i<mov_len; i++)
          {
            line->buf[line->cursor + i - 1] = line->buf[line->cursor + i];
          }

          line->count--;
          line->buf[line->count] = 0;

          uartPrintf(p_cli->ch, "\x1B[1P");
        }
        break;


      // ???????
      //
      case CLI_KEY_BACK:
        if (line->count > 0 && line->cursor > 0)
        {
          if (line->cursor == line->count)
          {
            line->count--;
            line->buf[line->count] = 0;
          }

          if (line->cursor < line->count)
          {
            uint8_t mov_len;

            mov_len = line->count - line->cursor;

            for (i=0; i<mov_len; i++)
            {
              line->buf[line->cursor + i - 1] = line->buf[line->cursor + i];
            }

            line->count--;
            line->buf[line->count] = 0;
          }
        }

        if (line->cursor > 0)
        {
          line->cursor--;
          uartPrintf(p_cli->ch, "\b \b\x1B[1P");
        }
        break;


      default:
        if ((line->count + 1) < line->buf_len)
        {
          if (line->cursor == line->count)
          {
            uartWrite(p_cli->ch, rx_data);

            line->buf[line->cursor] = rx_data;
            line->count++;
            line->cursor++;
            line->buf[line->count] = 0;
          }
          if (line->cursor < line->count)
          {
            uint8_t mov_len;

            mov_len = line->count - line->cursor;
            for (i=0; i<mov_len; i++)
            {
              line->buf[line->count - i] = line->buf[line->count - i - 1];
            }
            line->buf[line->cursor] = rx_data;
            line->count++;
            line->cursor++;
            line->buf[line->count] = 0;

            uartPrintf(p_cli->ch, "\x1B[4h%c\x1B[4l", rx_data);
          }
        }
        break;
    }
  }

  switch(p_cli->state)
  {
    case CLI_RX_SP1:
      p_cli->state = CLI_RX_SP2;
      break;

    case CLI_RX_SP2:
      p_cli->state = CLI_RX_SP3;
      break;

    case CLI_RX_SP3:
      p_cli->state = CLI_RX_IDLE;

      if (rx_data == CLI_KEY_LEFT)
      {
        if (line->cursor > 0)
        {
          line->cursor--;
          tx_buf[0] = 0x1B;
          tx_buf[1] = 0x5B;
          tx_buf[2] = rx_data;
          uartWrite(p_cli->ch, (uint8_t)tx_buf);
        }
      }

      if (rx_data == CLI_KEY_RIGHT)
      {
        if (line->cursor < line->count)
        {
          line->cursor++;

          tx_buf[0] = 0x1B;
          tx_buf[1] = 0x5B;
          tx_buf[2] = rx_data;
          uartWrite(p_cli->ch, (uint8_t)tx_buf);
        }
      }

      if (rx_data == CLI_KEY_UP)
      {
        cliLineChange(p_cli, true);
        uartPrintf(p_cli->ch, (char *)p_cli->line.buf);
      }

      if (rx_data == CLI_KEY_DOWN)
      {
        cliLineChange(p_cli, false);
        uartPrintf(p_cli->ch, (char *)p_cli->line.buf);
      }

      if (rx_data == CLI_KEY_HOME)
      {
        uartPrintf(p_cli->ch, "\x1B[%dD", line->cursor);
        line->cursor = 0;

        p_cli->state = CLI_RX_SP4;
      }

      if (rx_data == CLI_KEY_END)
      {
        uint16_t mov_len;

        if (line->cursor < line->count)
        {
          mov_len = line->count - line->cursor;
          uartPrintf(p_cli->ch, "\x1B[%dC", mov_len);
        }
        if (line->cursor > line->count)
        {
          mov_len = line->cursor - line->count;
          uartPrintf(p_cli->ch, "\x1B[%dD", mov_len);
        }
        line->cursor = line->count;
        p_cli->state = CLI_RX_SP4;
      }
      break;

    case CLI_RX_SP4:
      p_cli->state = CLI_RX_IDLE;
      break;
  }



  cliShowLog(p_cli);

  return ret;
}

void cliLineClean(cli_t *p_cli)
{
  p_cli->line.count   = 0;
  p_cli->line.cursor  = 0;
  p_cli->line.buf_len = CLI_LINE_BUF_MAX - 1;
  p_cli->line.buf[0]  = 0;
}

void cliLineAdd(cli_t *p_cli)
{

  p_cli->line_buf[p_cli->hist_line_last] = p_cli->line;

  if (p_cli->hist_line_count < CLI_LINE_HIS_MAX)
  {
    p_cli->hist_line_count++;
  }

  p_cli->hist_line_i    = p_cli->hist_line_last;
  p_cli->hist_line_last = (p_cli->hist_line_last + 1) % CLI_LINE_HIS_MAX;
  p_cli->hist_line_new  = true;
}

void cliLineChange(cli_t *p_cli, int8_t key_up)
{
  uint8_t change_i;


  if (p_cli->hist_line_count == 0)
  {
    return;
  }


  if (p_cli->line.cursor > 0)
  {
    uartPrintf(p_cli->ch, "\x1B[%dD", p_cli->line.cursor);
  }
  if (p_cli->line.count > 0)
  {
    uartPrintf(p_cli->ch, "\x1B[%dP", p_cli->line.count);
  }


  if (key_up == true)
  {
    if (p_cli->hist_line_new == true)
    {
      p_cli->hist_line_i = p_cli->hist_line_last;
    }
    p_cli->hist_line_i = (p_cli->hist_line_i + p_cli->hist_line_count - 1) % p_cli->hist_line_count;
    change_i = p_cli->hist_line_i;
  }
  else
  {
    p_cli->hist_line_i = (p_cli->hist_line_i + 1) % p_cli->hist_line_count;
    change_i = p_cli->hist_line_i;
  }

  p_cli->line = p_cli->line_buf[change_i];
  p_cli->line.cursor = p_cli->line.count;

  p_cli->hist_line_new = false;
}
uint8_t string_strcmp(char *s1, char *s2)
{
	while((*s1 != '\0') || (*s2 != '\0')){
		if(*s1 > *s2)return 1;
		else if(*s1 < *s2) return -1;
		else {
			s1++;
			s2++;
		}
	}
	return 0;
}
uint8_t cliRunCmd(cli_t *p_cli)
{
  uint8_t ret = false;
	int i;

  if (cliParseArgs(p_cli) == true)
  {
    uartPrintf(p_cli->log_ch,"\r\n");

    cliToUpper(p_cli->argv[0]);

    for (i=0; i<p_cli->cmd_count; i++)
    {
      if (string_strcmp(p_cli->argv[0], p_cli->cmd_list[i].cmd_str) == 0)
      {
        p_cli->cmd_args.argc =  p_cli->argc - 1;
        p_cli->cmd_args.argv = &p_cli->argv[1];
        p_cli->cmd_list[i].cmd_func(&p_cli->cmd_args);
        break;
      }
    }
  }

  return ret;
}

uint8_t cliParseArgs(cli_t *p_cli)
{
  uint8_t ret = false;
  char *tok;
  char *next_ptr;
  uint16_t argc = 0;
  static const char *delim = " \f\n\r\t\v";
  char *cmdline;
  char **argv;

  p_cli->argc = 0;

  cmdline = (char *)p_cli->line.buf;
  argv    = p_cli->argv;

  argv[argc] = '\0';

  for (tok = strtok_r(cmdline, delim, &next_ptr); tok; tok = strtok_r('\0', delim, &next_ptr))
  {
    argv[argc++] = tok;
  }

  p_cli->argc = argc;

  if (argc > 0)
  {
    ret = true;
  }

  return ret;
}

void cliToUpper(char *str)
{
  uint16_t i;
  uint8_t  str_ch;

  for (i=0; i<CLI_CMD_NAME_MAX; i++)
  {
    str_ch = str[i];

    if (str_ch == 0)
    {
      break;
    }

    if ((str_ch >= 'a') && (str_ch <= 'z'))
    {
      str_ch = str_ch - 'a' + 'A';
    }
    str[i] = str_ch;
  }

  if (i == CLI_CMD_NAME_MAX)
  {
    str[i-1] = 0;
  }
}

int32_t cliArgsGetData(uint8_t index)
{
  int32_t ret = 0;
  cli_t *p_cli = &cli_node;


  if (index >= p_cli->cmd_args.argc)
  {
    return 0;
  }

  ret = (int32_t)strtoul((const char * ) p_cli->cmd_args.argv[index], (char **)'\0', (int) 0);

  return ret;
}

float cliArgsGetFloat(uint8_t index)
{
  float ret = 0.0;
  cli_t *p_cli = &cli_node;


  if (index >= p_cli->cmd_args.argc)
  {
    return 0;
  }

//  ret = (float)strtof((const char * ) p_cli->cmd_args.argv[index], (char **)'\0');

  return ret;
}

char *cliArgsGetStr(uint8_t index)
{
  char *ret = '\0';
  cli_t *p_cli = &cli_node;


  if (index >= p_cli->cmd_args.argc)
  {
    return 0;
  }

  ret = p_cli->cmd_args.argv[index];

  return ret;
}

uint8_t cliArgsIsStr(uint8_t index, char *p_str)
{
  uint8_t ret = false;
  cli_t *p_cli = &cli_node;


  if (index >= p_cli->cmd_args.argc)
  {
    return 0;
  }

  if(strcmp(p_str, p_cli->cmd_args.argv[index]) == 0)
  {
    ret = true;
  }

  return ret;
}

uint8_t cliKeepLoop(void)
{
  cli_t *p_cli = &cli_node;


  if (uartAvailable(p_cli->ch) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

uint8_t cliAdd(const char *cmd_str, void (*p_func)(cli_args_t *))
{
  uint8_t ret = true;
  cli_t *p_cli = &cli_node;
  uint16_t index;

  if (p_cli->cmd_count >= CLI_CMD_LIST_MAX)
  {
    return false;
  }

  index = p_cli->cmd_count;

  strcpy(p_cli->cmd_list[index].cmd_str, cmd_str);
  p_cli->cmd_list[index].cmd_func = p_func;

  cliToUpper(p_cli->cmd_list[index].cmd_str);

  p_cli->cmd_count++;

  return ret;
}

void cliShowList(cli_args_t *args)
{
  int i;
  cli_t *p_cli = &cli_node;


  uartPrintf(p_cli->log_ch, "\r\n");
  uartPrintf(p_cli->log_ch, "---------- cmd list ---------\r\n");

  for (i=0; i<p_cli->cmd_count; i++)
  {
    uartPrintf(p_cli->log_ch, p_cli->cmd_list[i].cmd_str);
    uartPrintf(p_cli->log_ch, "\r\n");
  }

  uartPrintf(p_cli->log_ch, "-----------------------------\r\n");
}

void cliMemoryDump(cli_args_t *args)
{
  int idx, size = 16;
  unsigned int *addr;
  int idx1, i;
  unsigned int *ascptr;
  unsigned char asc[4];
	cli_t *p_cli = &cli_node;
	
  int    argc = args->argc;
  char **argv = args->argv;


  if(args->argc < 1)
  {
    uartPrintf(p_cli->log_ch, ">> md addr [size] \n");
    return;
  }

  if(argc > 1)
  {
    size = (int)strtoul((const char * ) argv[1], (char **)'\0', (int) 0);
  }
  addr   = (unsigned int *)strtoul((const char * ) argv[0], (char **)'\0', (int) 0);
  ascptr = (unsigned int *)addr;

  uartPrintf(p_cli->log_ch, "\n   ");
  for (idx = 0; idx<size; idx++)
  {
    if((idx%4) == 0)
    {
      uartPrintf(p_cli->log_ch, " 0x%08X: ", (unsigned int)addr);
    }
    uartPrintf(p_cli->log_ch, " 0x%08X", *(addr));

    if ((idx%4) == 3)
    {
      uartPrintf(p_cli->log_ch, "  |");
      for (idx1= 0; idx1< 4; idx1++)
      {
        memcpy((char *)asc, (char *)ascptr, 4);
        for (i=0;i<4;i++)
        {
          if (asc[i] > 0x1f && asc[i] < 0x7f)
          {
            uartPrintf(p_cli->log_ch, "%c", asc[i]);
          }
          else
          {
            uartPrintf(p_cli->log_ch, ".");
          }
        }
        ascptr+=1;
      }
      uartPrintf(p_cli->log_ch, "|\n   ");
    }
    addr++;
  }
}
uint8_t gGetCliCh(void)
{
	return cli_node.ch;
}
#endif
