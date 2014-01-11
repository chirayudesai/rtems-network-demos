/*===============================================================*\
| Project: osmonweb                                               |
+-----------------------------------------------------------------+
| File: htmlprintf.h                                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2003 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file declares the functions to maintain the ramlog via web |
| GNU C Compiler                                                  |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 19.09.03  creation                                         doe  |
|*****************************************************************|
|*RCS information:                                                |
|*(the following information is created automatically,            |
|*do not edit here)                                               |
|*****************************************************************|
|* $Log$
|* Revision 1.1  2003/09/23 17:01:05  thomas
|* first version running with web browser
|*
 *
|*****************************************************************|
\*===============================================================*/
#ifndef _HTMLPRINTF_H
#define _HTMLPRINTF_H

#include <stdint.h>

#define HTML_PRINTF_ERROR_FEW_ARGS -1
#define HTML_PRINTF_ERROR_ILL_ARGS -2

typedef struct {
  const char *strarg;
  int32_t     intarg;
} html_printf_arg_t;

typedef struct {
  struct {
    const char *beg;
    size_t      len;
  } strarg;
  int32_t     intarg;
} html_printf_param_t;

#define HP_MAXPAR 20

typedef int (* html_printf_outfnc_t)(void *outfnc_context,
                     const char *out_str, int len);

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
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    ???                                                                    |
\*=========================================================================*/

#endif /* _HTMLPRINT_H */
