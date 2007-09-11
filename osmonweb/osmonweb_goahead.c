/*===============================================================*\
| Project: OSMONWEB GoAhead Webserver Adapter                     |
+-----------------------------------------------------------------+
| File: osmonweb_goahead.c                                        |
+-----------------------------------------------------------------+
|                    Copyright (c) 2003 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file contains the top-level functions to visualize the     |
| RTEMS OS structures with a web browser                          |
| GNU C Compiler                                                  |
|                                                                 |
+-----------------------------------------------------------------+
  $Id$
\*===============================================================*/

#include "osmonweb.h"
#include "osmonweb_int.h"

#if defined(USE_GOAHEAD_HTTPD)
  #include <stdlib.h>
  #include <string.h>
  #include <stdio.h>
  #include <limits.h>
  #include <errno.h>

  #include <goahead/uemf.h>

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
uint32_t osmonweb_getvar_u32
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   get variable from URL                                                   |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 osmonweb_ctx_t wp,             /* web handle               */
 char_t *key,           /* key/name of variable     */
 uint32_t default_val /* default value, if absent */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    value of variable                                                      |
\*=========================================================================*/
{
  const char *tmp_var;
  char *end_ptr;
  uint32_t ret_val;

  ret_val = default_val;
  tmp_var = websGetVar(wp, key,NULL);
  if (tmp_var != NULL) {
    ret_val = strtoul(tmp_var,&end_ptr,0);
    if (end_ptr == NULL) {
      ret_val = default_val;
    }
  }
  return ret_val;
}

void *osmonweb_getCommonOptions(
  osmonweb_ctx_t wp,        /* web handle */
  osmonweb_common_option_t *c
)
{
  c->api_type          = websGetVar(wp,T("api"),NULL);
  c->obj_type          = websGetVar(wp,T("obj_type"),NULL);
  c->action            = websGetVar(wp,T("action"),NULL);
  c->multi_node_enable = osmonweb_getvar_u32(wp, T("multi_node_enable"), 0);
  c->node_curr         = osmonweb_getvar_u32(wp, T("node_curr"), 0);
  c->multi_obj_enable  = osmonweb_getvar_u32(wp, T("multi_obj_enab"), 1);
  c->obj_curr          = osmonweb_getvar_u32(wp, T("obj_curr"), 0);
  c->auto_rfsh_enable  = osmonweb_getvar_u32(wp, T("auto_rfsh_enab"), 0);
  c->auto_rfsh_time    = osmonweb_getvar_u32(wp, T("auto_rfsh_time"), 0);
  c->sort_dir          = osmonweb_getvar_u32(wp, T("sort_dir"), 0);
  c->sort_select       = osmonweb_getvar_u32(wp, T("sort_select"), 0);
  c->hdr_rep_enable    = osmonweb_getvar_u32(wp, T("hdr_rep_enable"), 0);
  c->hdr_repcnt        = osmonweb_getvar_u32(wp, T("hdr_rep_cnt"), 0);
  return NULL;
}

int osmonweb_WriteBlock(
  osmonweb_ctx_t  wp,        /* web handle */
  char           *buf,
  int             nChars
)
{
  return websWriteBlock( wp, buf, nChars );
}

void osmonweb_header(
  osmonweb_ctx_t  wp         /* web handle */
)
{
  websHeader(wp);
}

void osmonweb_footer(
  osmonweb_ctx_t  wp         /* web handle */
)
{
  websFooter(wp);
  websDone(wp, 200);
}

const char *osmonweb_httpdname(void)
{
  return "GoAhead Webserver";
}

const char *osmonweb_logoname(void)
{
  return "webserver_logo2.gif";
}

#endif
