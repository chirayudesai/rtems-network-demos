/*===============================================================*\
| Project: OSMONWEB web interface to RTEMS monitor information    |
+-----------------------------------------------------------------+
| File: osmonweb_int.h                                            |
+-----------------------------------------------------------------+
|                    Copyright (c) 2001 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file declares the functions to maintain the ramlog via web |
| GNU C Compiler                                                  |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 23.09.03  creation                                         doe  |
|*****************************************************************|
|*RCS information:                                                |
|*(the following information is created automatically,            |
|*do not edit here)                                               |
|*****************************************************************|
|* $Log$
|* Revision 1.3  2007/09/11 18:25:42  joel
|* 2007-09-11	Joel Sherrill <joel.sherrill@oarcorp.com>
|*
|* 	* .cvsignore, Makefile, init.c, osmonweb.c, osmonweb_int.h: Now works
|* 	ok with both GoAhead and Simple HTTPD.
|* 	* osmonweb_ITRON_objs.c, osmonweb_POSIX_objs.c, osmonweb_RTEID_objs.c,
|* 	osmonweb_goahead.c, osmonweb_shttpd.c, osmonweb_tar.c: New files.
|*
|* Revision 1.2  2007/09/05 23:46:07  joel
|* 2007-09-05	Joel Sherrill <joel.sherrill@oarcorp.com>
|*
|* 	* Makefile, osmonweb.c, osmonweb_RTEID.c, osmonweb_int.h: Remove JOEL
|* 	conditionals which disabled parts of the code dependent on monitor
|* 	enhancements which Thomas just merged. Clean up various warnings.
|* 	* .cvsignore, html/.cvsignore: New files.
|*
|* Revision 1.1.1.1  2007/09/05 13:42:17  joel
|* initial import.
|*
|* Revision 1.2  2003/10/06 22:56:04  thomas
|* task resume/suspend works
|*
|* Revision 1.1  2003/10/06 20:27:39  thomas
|* moved RTEID functions to separate file
|* changed names of HTML fragments
|*
 *
|*****************************************************************|
\*===============================================================*/
#ifndef _OSMONWEB_INT_H
#define _OSMONWEB_INT_H

#include <rtems.h>
#include "htmlprintf.h"

/*
 * data structure to hold common display options like sorting, API/OBJ etc
 */
typedef struct {
  const char *api_type;
  const char *obj_type;
  const char *action;
  int         multi_node_enable;
  int         node_curr;
  int         multi_obj_enable;
  int         obj_curr;
  int         auto_rfsh_enable;
  int         auto_rfsh_time;
  int         sort_dir; /* -1: falling, +1: rising; 0: no sort */
  int         sort_select;
  int         hdr_rep_enable;
  int         hdr_repcnt;
  const char *clean_query;
} osmonweb_common_option_t;

/*
 * data structure to keep data of individual object and
 * pointer to common options (for sorting)
 */
typedef struct {
  html_printf_arg_t        data[16];
  osmonweb_common_option_t *common_options;
} osmonweb_obj_data_t;

/*
 * Webserver adapter interface used by osmonweb
 */
#if defined(USE_SIMPLE_HTTPD)
  #include <shttpd/shttpd.h>
  typedef struct shttpd_ctx *osmonweb_ctx_t;
#endif
#if defined(USE_GOAHEAD_HTTPD)
  #include <goahead/wsIntrn.h>
  typedef webs_t osmonweb_ctx_t;
#endif

void *osmonweb_getCommonOptions(
  osmonweb_ctx_t            wp,        /* web handle */
  osmonweb_common_option_t *c
);

void osmonweb_header(
  osmonweb_ctx_t  wp                   /* web handle */
);;

void osmonweb_footer(
  osmonweb_ctx_t  wp                   /* web handle */
);

int osmonweb_WriteBlock(
  osmonweb_ctx_t  wp,                  /* web handle */
  char           *buf,
  int             nChars
);

const char *osmonweb_httpdname(void);

const char *osmonweb_logoname(void);

/*
 * Helper macros to determine number of elements in an array
 */
#ifndef ARRAY_COUNT
#define ARRAY_COUNT(a) (sizeof(a)/sizeof(a[0]))
#endif /* ARRAY_COUNT */

/*
 * data structure to control generation of one html fragment
 */
typedef struct {
  const char *form;
  int   (*output)(const char *format,int argc,const html_printf_arg_t *arg,
                  html_printf_outfnc_t outfnc,void *outfnc_ctxt);
  boolean use_obj_info;  
} osmonweb_fragment_ctrl_t;

/*
 * FIXME: find a better way to make flexible 
 * assignment of fragment entries 
 */
#define FRAGCTRL_TITLE    0
#define FRAGCTRL_HEADER   1
#define FRAGCTRL_TABHEAD  2
#define FRAGCTRL_TABENTRY 3
#define FRAGCTRL_TABFOOT  4
#define FRAGCTRL_FOOTER   5
#define FRAGCTRL_TRAILER  6

/*
 * data structure to control handling of an object type
 */
typedef struct {
  const char *name;
  uint32_t (*collect_fnc)(
    osmonweb_common_option_t *common_options,
    uint32_t   *cookie,
    uint32_t   result_cnt,
    html_printf_arg_t *results); 
  uint32_t (*cleanup_fnc)(
    uint32_t   result_cnt,
    html_printf_arg_t *results
  ); 
  uint32_t (*action_fnc)(
    osmonweb_ctx_t wp,
    const char *act_str,
    osmonweb_common_option_t *common_options
  ); 
  const osmonweb_fragment_ctrl_t *fragment[2];
} osmonweb_objtype_t;

typedef struct {
  const char                     *name;
  const osmonweb_fragment_ctrl_t *default_framents;
  osmonweb_objtype_t             *objtypes;
} osmonweb_api_entry_t;

extern osmonweb_objtype_t RTEID_objtypes[];
extern const osmonweb_fragment_ctrl_t osmonweb_RTEID_objs_fragment[];

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int osmonweb_compare
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   compare two object data sets according to sort criterium                |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const void *a,
  const void *b
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    -1: a < b; 0: a == b; +1: a > b                                        |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
uint32_t   osmonweb_getvar_u32
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   get variable from URL                                                   |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 osmonweb_ctx_t wp,             /* web handle               */
 char *key,           /* key/name of variable     */
 uint32_t   default_val /* default value, if absent */
);
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    value of variable                                                      |
\*=========================================================================*/

#endif /* _OSMONWEB_INT_H */
