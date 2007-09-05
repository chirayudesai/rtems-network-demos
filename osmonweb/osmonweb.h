/*
| Project: OSMONWEB web interface to RTEMS monitor information    |
+-----------------------------------------------------------------+
| File: osmonweb.h                                                |
+-----------------------------------------------------------------+
|                    Copyright (c) 2001 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file declares the functions to maintain the ramlog via web |
| GNU C Compiler                                                  |
|                                                                 |
+-----------------------------------------------------------------+
 * $Id$
*/
#ifndef _OSMONWEB_H
#define _OSMONWEB_H

#include <rtems.h>
/* 
 * interface function
 */

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code osmonweb_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize osmonweb module                                              |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const char *web_root_dir,    /* web root directory */
 int phase
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    status code                                                            |
\*=========================================================================*/
#endif /* _OSMONWEB_H */
