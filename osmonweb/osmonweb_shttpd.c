/*===============================================================*\
| Project: OSMONWEB SHTTPD Adapter Layer                          |
+-----------------------------------------------------------------+
| File: osmonweb_shttpd.c                                         |
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
#if defined(USE_MONITOR)
  #include <rtems/monitor.h>
#endif

#if defined(USE_SIMPLE_HTTPD)
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

void osmonweb_toInt(
  const char *from,
  int        *to
)
{
  int v;

  v = strtol( from, NULL, 10 );
  if ( (v == 0 && errno == EINVAL) ||
       (v == LONG_MIN && errno == ERANGE) ||
       (v == LONG_MAX && errno == ERANGE) )
   return;
  *to = v;
}

void *osmonweb_getCommonOptions(
  osmonweb_ctx_t            wp,        /* web handle */
  osmonweb_common_option_t *c
)
{
  const char *query_arg;
  char *query;
  char *state;
  char *lhs, *rhs;

  if ( !c )
    return NULL;

  query_arg = shttpd_get_env((struct shttpd_arg *)wp, "QUERY_STRING" );

  c->api_type          = NULL;
  c->obj_type          = NULL;
  c->action            = NULL;
  c->multi_node_enable = 0;
  c->node_curr         = 0;
  c->multi_obj_enable  = 1;
  c->obj_curr          = 0;
  c->auto_rfsh_enable  = 0;
  c->auto_rfsh_time    = 0;
  c->sort_dir          = 0; /* -1: falling, +1: rising; 0: no sort */
  c->sort_select       = 0;
  c->hdr_rep_enable    = 0;
  c->hdr_repcnt        = 0;
  c->clean_query       = NULL;

  if ( !query_arg )
    return NULL;

  query = strdup( query_arg );
  state = query;

  while ( 1 ) {
    lhs = strtok_r( state, "?=&", &state );
    if ( !lhs )
      break;
    rhs = strtok_r( state, "?=&", &state );
    #if 0
      fprintf(
	stderr,
	"lhs=%s  \trhs=%s\n", lhs, ((rhs)? rhs : "NULL")
      );
    #endif

    if ( !strcmp( lhs, "api" ) )
      c->api_type = rhs;
    else if ( !strcmp( lhs, "obj_type" ) )
      c->obj_type = rhs;
    else if ( !strcmp( lhs, "action" ) )
      c->action = rhs;
    else if ( !strcmp( lhs, "multi_node_enable" ) )
      osmonweb_toInt( rhs, &c->multi_node_enable );
    else if ( !strcmp( lhs, "node_curr" ) )
      osmonweb_toInt( rhs, &c->node_curr );
    else if ( !strcmp( lhs, "multi_obj_enable" ) )
      osmonweb_toInt( rhs, &c->multi_obj_enable );
    else if ( !strcmp( lhs, "obj_curr" ) )
      osmonweb_toInt( rhs, &c->obj_curr );
    else if ( !strcmp( lhs, "auosmonweb_toInt" ) )
      osmonweb_toInt( rhs, &c->auto_rfsh_enable );
    else if ( !strcmp( lhs, "auto_rfsh_time" ) )
      osmonweb_toInt( rhs, &c->auto_rfsh_time );
    else if ( !strcmp( lhs, "sort_dir" ) )
      osmonweb_toInt( rhs, &c->sort_dir );
    else if ( !strcmp( lhs, "sort_select" ) )
      osmonweb_toInt( rhs, &c->sort_select );
    else if ( !strcmp( lhs, "hdr_rep_enable" ) )
      osmonweb_toInt( rhs, &c->hdr_rep_enable );
    else if ( !strcmp( lhs, "hdr_repcnt" ) )
      osmonweb_toInt( rhs, &c->hdr_repcnt );
  }

  return query;
}

void osmonweb_header(
  osmonweb_ctx_t  wp         /* web handle */
)
{
  struct shttpd_arg *arg = (struct shttpd_arg *) wp;

  shttpd_printf(arg, "%s",
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n\r\n"
    "<html><body>"
  );
}

void osmonweb_footer(
  osmonweb_ctx_t  wp         /* web handle */
)
{
  struct shttpd_arg *arg = (struct shttpd_arg *) wp;

  shttpd_printf( arg, "%s", "</body></html>" );
  arg->flags |= SHTTPD_END_OF_OUTPUT;
}

int osmonweb_WriteBlock(
  osmonweb_ctx_t  wp,        /* web handle */
  char           *buf,
  int             nChars
)
{
  int i;
  char cbuf[2];

  for ( i = 0 ; i < nChars ; i++ ) {
    sprintf( cbuf, "%c", buf[i]);
    shttpd_printf((struct shttpd_arg *)wp, "%s", cbuf);
  }
  return nChars;
}

const char *osmonweb_httpdname(void)
{
  return "Simple HTTPD";
}

const char *osmonweb_logoname(void)
{
  return "SimpleHTTPD.png";
}

#endif
