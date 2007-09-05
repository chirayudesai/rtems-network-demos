/*===============================================================*\
| Project: osmonweb                                               |
+-----------------------------------------------------------------+
| File: htmlprintf.c                                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2003 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file contains a function to generate output based on a     |
| format string and various parameters                            |
| This file is intended to be used in conjunction with the        |
| GNU C Compiler                                                  |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 27.05.03  creation                                         doe  |
|*****************************************************************|
|*RCS information:                                                |
|*(the following information is created automatically,            |
|*do not edit here)                                               |
|*****************************************************************|
|* $Log$
|* Revision 1.2  2003/09/26 22:33:16  thomas
|* RTEID tasks and queues work fine
|*
|* Revision 1.1  2003/09/23 17:01:05  thomas
|* first version running with web browser
|*
|*****************************************************************|
\*===============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "htmlprintf.h"

#ifndef MIN
#define MIN(a,b) (((a) > (b)) ? (b) : (a))
#endif

#define DBG_LEVEL 5
/* #define TEST */

#define HTML_PRINTF_TOKEN_START "<!--%"
#define HTML_PRINTF_TOKEN_END   "-->"
#define HTML_PRINTF_SEPARATOR   '|'

#if !defined(FALSE)
#define FALSE 0
#define TRUE (!FALSE)
#endif
typedef enum {
  hp_print_int,
  hp_print_str,
  hp_print_name,
  hp_print_interpolate,
  hp_print_icompress,
  hp_print_select
} html_printf_fncsel_t;

typedef struct {
  const char *fnc_selstr;    /* string to identify function */
  html_printf_fncsel_t fnc_sel; 
  int parnum;                  /* number of parameters        */
  int par_is_int[HP_MAXPAR]; /* Flags: param is a integer, not string */
} html_printf_fncdef_t;

static html_printf_fncdef_t html_printf_fncdef[] = {
  {"printint",hp_print_int,2,
   {FALSE,TRUE}},
  {"printstr",hp_print_str,2,
   {FALSE,TRUE}},
  {"printname",hp_print_name,2,
   {FALSE,TRUE}},
  {"printipol",hp_print_interpolate,6,
   {FALSE,TRUE ,TRUE ,TRUE ,TRUE ,TRUE}},
  {"printicompress",hp_print_icompress,3,
   {FALSE,TRUE,TRUE}},
  {"printselect",hp_print_select,0,
   {FALSE,TRUE, TRUE /* rest is FALSE->string */}},
  {NULL,0,0,{FALSE}}
};

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int html_printf_get_parameters
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   send output to out_fnc either between two pointers or from start to NUL |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const char *start,                     /* start of parameter string       */
 const char *end,                       /* end of parameter string         */
 html_printf_fncsel_t *fncsel,          /* function selector               */
 html_printf_param_t *params,           /* parameters to read in           */
 int hp_params_arraycnt,                /* maximum count of params         */
 int *hp_params_cnt                     /* params actually read            */
 
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    >= 0 = ok, else error in output function                               |
\*=========================================================================*/
{
  int ret_status = 0;
  html_printf_fncdef_t *fncdef;
  char *tmp_ptr;
  const char *fncsel_beg;
  const char *next_sep;
  
  *hp_params_cnt = 0;
  /*
   * skip blanks 
   */
  while ((start < end) && 
         (isspace(*start))) {
    start++;
  }
  fncsel_beg = start;
  /*
   * find end of function selector
   */
  while ((start < end) && 
         (!isspace(*start))) {
    start++;
  }

  /*
   * find function definition
   */
  fncdef = html_printf_fncdef;
  while ((fncdef->fnc_selstr != NULL) &&
         (0 != strncmp(fncsel_beg,
                       fncdef->fnc_selstr,
                       strlen(fncdef->fnc_selstr)))) {
    fncdef++;
  }
  if (fncdef->fnc_selstr == NULL) {
    ret_status = -1;
  }
  else {
    *fncsel = fncdef->fnc_sel;
  }

  /*
   * skip any whitespaces
   */
  while ((start < end) && 
         (isspace(*start))) {
    start++;
  }

  /*
   * for all params in string
   */
  while ((ret_status == 0) &&
         (start < end)     &&
         ((fncdef->parnum == 0) ||
          (fncdef->parnum > *hp_params_cnt)) &&
         (*hp_params_cnt < hp_params_arraycnt)) {
    next_sep = strchr(start,HTML_PRINTF_SEPARATOR);
    if ((next_sep == NULL) || (next_sep > end)) {
      next_sep = end;
    }
    params[*hp_params_cnt].strarg.beg = start;
    params[*hp_params_cnt].strarg.len = next_sep - start;
    if (fncdef->par_is_int[*hp_params_cnt]) {
      params[*hp_params_cnt].intarg     = strtol(start,&tmp_ptr,0);
      if (tmp_ptr == start) {
        /*
         * non-integer parameter string
         */
        ret_status = -2;
      }
    }
    else {
      params[*hp_params_cnt].intarg = -1;
    }
    (*hp_params_cnt)++;
    start = next_sep + 1;
  }
  if ((fncdef->parnum > 0) && 
      (fncdef->parnum != *hp_params_cnt)) {
    /*
     * too few parameters 
     */
    ret_status = -3;
  }
  return ret_status;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int html_printf_emmit
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   send output to out_fnc either between two pointers or from start to NUL |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const char *start,                     /* start of text to send           */
 const char *end,                       /* end of text to send or NULL     */
 html_printf_outfnc_t  outfnc,          /* function to gather output       */
 void                 *outfnc_ctxt      /* context of outfnc               */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    >= 0 = ok, else error in output function                               |
\*=========================================================================*/
{
  int ret_status = 0;
  ssize_t out_len;
  int tot_out = 0;

  if (end == NULL) {
    /*
     * original string already NUL-terminated? then no copy needed
     */
    out_len = strlen(start);
  }
  else {
    out_len = end - start;
  }
  do {
    ret_status = outfnc(outfnc_ctxt,start,out_len);
    if (ret_status > 0) {
      tot_out += ret_status;
      out_len -= ret_status;
      start   += ret_status;
    }
  } while ((ret_status > 0) && (out_len > 0));
  return (ret_status < 0) ? ret_status : tot_out;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static uint32_t html_printf_bit_compress
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   compress integer according to given bit masks: only those bit positions |
|   are copied from input to output, that are set in the bit mask.          |
|   Example: input: 0x12345678                                              | 
|            mask:  0xFF00F00F                                              | 
|                     ||  |  |                                              | 
|                     ||  +-+|                                              | 
|                     |+---+||                                              | 
|                     +---+|||                                              | 
|                         ||||                                              | 
|                         VVVV                                              | 
|  output =         0x00001258                                              | 
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 uint32_t val,
 uint32_t mask
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    result of bit compression                                              |
\*=========================================================================*/
{
  uint32_t bit_mask = 0;
  uint32_t bit_pos  = 1;
  while (bit_pos != 0) {
    if ((mask & bit_pos) == 0) {
      val = ( ( (val     ) & ( bit_mask))    /* bits left of current bit position  */
              |((val >> 1) & (~bit_mask)));  /* bits right of current bit position */      
    }
    else {
      bit_mask = (bit_mask << 1) | 1;        /* shift result bitmask left, fill in with 1 */
    }
    bit_pos <<= 1;                           /* test next bit...                   */
  }
  return val;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int html_printf
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   generate output based on a format string and parameters                 |
|   the main purpose is like printf, but with more features like            |
|   - string selection                                                      |
|   - value interpolation                                                   |
|   - independence of parameter index and usage position                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const char *format,                    /* format string                   */
 int          argc,                     /* number of arguments             */
 const html_printf_arg_t *arg,          /* array of arguments              */
 html_printf_outfnc_t     outfnc,       /* function to gather output       */
 void                    *outfnc_ctxt   /* context of outfnc               */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    ???                                                                    |
\*=========================================================================*/
{
  int ret_status = 0;
  const char *token_start;
  const char *token_end;
#define HP_PARAMS_ARRAYCNT 20
  html_printf_param_t hp_params[HP_PARAMS_ARRAYCNT];
  html_printf_fncsel_t fnc_sel;
  int hp_param_cnt;
  char fmt_str[40];
  char hp_tmpstr[512];
  char sel_str[80];
  int val;
  int i;

  while ((format != NULL) &&
         (*format != '\0') &&
         (ret_status >= 0)) {
    /*
     * find next replacement token in format
     */
    token_start = strstr(format,HTML_PRINTF_TOKEN_START);
    /*
     * if found, 
     * then send text 'til this token as normal string
     * else send rest of text
     */
    if (token_start != format) {
      ret_status = html_printf_emmit(format,   /* start of string to send */
                                    token_start,/* either end of str or NULL*/
                                    outfnc,outfnc_ctxt);      
    }
    /*
     * if token found, set token_start to end of start token
     * and find end of token
     */
    if (token_start == NULL) {      
      format = NULL;
    }
    else {
      token_start += sizeof(HTML_PRINTF_TOKEN_START)-1;
      token_end = strstr(token_start,HTML_PRINTF_TOKEN_END);      
    /*
     * if token and end found, 
     * then replace it with replacement text
     * else simply send rest of string
     */
      if (token_end != NULL) {
        /*
         * FIXME: process token, emmit corresponding text
         */
        /*
         * FIXME: split following parameters:
         * - type identifier, 
         * - format string,
         * - list of (integer) parameter values, 
         *   meaning is dependent on type
         * perform plausibility checks (like proper number of args)
         */
        ret_status = html_printf_get_parameters(token_start,token_end,
                                                &fnc_sel,
                                                hp_params,
                                                HP_PARAMS_ARRAYCNT,
                                                &hp_param_cnt);
        if (ret_status == 0) {
          /*
           * copy format string to get NUL-termination
           */
          strncpy(fmt_str,hp_params[0].strarg.beg,
                  MIN(sizeof(fmt_str),hp_params[0].strarg.len));
          if ((sizeof(fmt_str)-1) > hp_params[0].strarg.len) {
            fmt_str[hp_params[0].strarg.len] = '\0';
          }
          else {
            fmt_str[sizeof(fmt_str)-1] = '\0';
          }
          
          switch(fnc_sel) {
          case hp_print_int:
            /*
             * print integer formatted
             */
            if (hp_params[1].intarg > argc) {
              ret_status = HTML_PRINTF_ERROR_FEW_ARGS;
            }
            else {
              ret_status = sprintf(hp_tmpstr,fmt_str,
                                   arg[hp_params[1].intarg].intarg);
            }
            break;
          case hp_print_str:
            /*
             * print string formatted
             */
            if (hp_params[1].intarg > argc) {
              ret_status = HTML_PRINTF_ERROR_FEW_ARGS;
            }
            else {
              ret_status = sprintf(hp_tmpstr,fmt_str,
                                   arg[hp_params[1].intarg].strarg);
            }
            break;
          case hp_print_name:
            /*
             * print 32 bit value as RTEID name
             */
            if (hp_params[1].intarg > argc) {
              ret_status = HTML_PRINTF_ERROR_FEW_ARGS;
            }
            else {
              char name_str[5];
              uint32_t name_val;
              uint8_t  name_chr;

              name_val = arg[hp_params[1].intarg].intarg;
              for (i = 0;i < sizeof(name_val);i++) {
                name_chr = 0xff & (name_val >> 24);
                if (!isgraph(name_chr)) {
                  name_chr = '.';
                }
                name_str[i] = name_chr;
                name_val <<= 8;
              }
              name_str[sizeof(name_val)] = '\0';
              ret_status = sprintf(hp_tmpstr,fmt_str,
                                   name_str);
            }
            break;
          case hp_print_interpolate:
            /*
             * print interpolated integer formatted
             */
            if (hp_params[1].intarg > argc) {
              ret_status = HTML_PRINTF_ERROR_FEW_ARGS;
            }
            else if (hp_params[3].intarg == hp_params[2].intarg) {
              ret_status = HTML_PRINTF_ERROR_ILL_ARGS;
            }
            else {
              /*
               * limit given argument to bounds
               */
              val = arg[hp_params[1].intarg].intarg;
              if (val < hp_params[2].intarg) {
                val = hp_params[2].intarg;
              }
              else if (val > hp_params[3].intarg) {
                val = hp_params[3].intarg;
              }
              /*
               * interpolate given argument between bounds
               */
              val = (((int64_t)
                      (  val                -hp_params[2].intarg)
                      * (hp_params[5].intarg-hp_params[4].intarg)
                      / (hp_params[3].intarg-hp_params[2].intarg))
                     + hp_params[4].intarg);
              /*
               * print result according to format
               */
              ret_status = sprintf(hp_tmpstr,fmt_str,
                                   (int32_t)val);
            }
            break;
          case hp_print_icompress:          
            /*
             * print integer formatted
             * but compress it first...
             * param 0 (str): printf format to be used for output
             * param 1 (int): select argument to be printed as integer
             * param 2 (int): compression mask for argument
             */
            if (hp_params[1].intarg > argc) {
              ret_status = HTML_PRINTF_ERROR_FEW_ARGS;
            }
            else {
              /*
               * get value, compress it first
               */
              val = arg[hp_params[1].intarg].intarg;
              val = html_printf_bit_compress(val,hp_params[2].intarg);
              
              ret_status = sprintf(hp_tmpstr,fmt_str,val);
            }
            break;
          case hp_print_select:     
            /*
             * print string selected by given compressed integer
             * param 0 (str): printf format to be used for output
             * param 1 (int): select argument to be used as selector
             * param 2 (int): compression mask for argument
             * param 3 (str): default output, if invalid input value
             * param 4 (str): output for compressed value = 0
             * param 5 (str): output for compressed value = 1
             */
            if (hp_params[1].intarg > argc) {
              ret_status = HTML_PRINTF_ERROR_FEW_ARGS;
            }
            else {
            /*
             * get value, compress it first
             */
              val = arg[hp_params[1].intarg].intarg;
              val = html_printf_bit_compress(val,hp_params[2].intarg);
              /*
               * detect overflow, set to (first) default argument
               */
              if ((val < 0) || (val >= hp_param_cnt - 4)) {
                val = -1;
              }
              /*
               * copy selected string to get NUL-termination
               */
              strncpy(sel_str,hp_params[val+4].strarg.beg,
                      MIN(sizeof(sel_str),hp_params[val+4].strarg.len));
              if ((sizeof(sel_str)-1) > hp_params[val+4].strarg.len) {
                sel_str[hp_params[val+4].strarg.len] = '\0';
              }
              else {
                sel_str[sizeof(sel_str)-1] = '\0';
              }
          
              /*
               * print selected string according to format
               */
              ret_status = sprintf(hp_tmpstr,fmt_str,
                                   sel_str);
            }
            break;
          }
          if (ret_status >= 0) {
            ret_status = html_printf_emmit(hp_tmpstr,
                                           NULL,      
                                           outfnc,outfnc_ctxt);
          }
        }
        /*
         * advance format behind token end
         */
        format = token_end + sizeof(HTML_PRINTF_TOKEN_END)-1;
      }
      else {
        /*
         * no token end found, send rest of format string unchanged
         */
        ret_status = html_printf_emmit(format,   /* start of string to send */
                                       NULL,      /* send to end of format   */
                                       outfnc,outfnc_ctxt);
        /*
         * trick: terminate loop now
         */
        format = NULL;
      }
    }
  }
  return ret_status;
}

#ifdef TEST
int hp_test_puts(void *outfile,const char *outstr)
{
  return fputs(outstr,outfile);
}

int main(int argc,char **argv)
{
  int ret_val;
  html_printf_arg_t hp_args[50];
  int i;

  if (argc < 2) {
    fprintf(stderr,"Usage: \n %s <teststring> [args]",argv[0]);
    exit(1);
  }
  /*
   * convert arguments
   */
  if (argc > 50) {
    argc = 50;
  }
  for (i = 2;i < argc;i++) {
    hp_args[i-2].strarg = argv[i];
    hp_args[i-2].intarg = strtol(argv[i],NULL,0);
  }
  /*
   * call html_printf
   */
  ret_val = html_printf(argv[1],argc-2,
                        hp_args,
                        hp_test_puts,
                        stdout);
  printf("\nhtml_printf returned %d\n",ret_val);

}
#endif /* TEST */
