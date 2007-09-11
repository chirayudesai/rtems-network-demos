/*===============================================================*\
| Project: OSMONWEB web interface to RTEMS monitor information    |
+-----------------------------------------------------------------+
| File: osmonweb.c                                                |
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

#include <rtems.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <rtems/untar.h>
#include <sys/stat.h>      /* for mkdir */
#include <sys/types.h>

#include "osmonweb.h"
#include "osmonweb_int.h"
#if defined(USE_MONITOR)
  #include <rtems/monitor.h>
#endif


#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

#include <rtems/error.h>

#include "osmonweb_tar.h"

#include "osmonweb_POSIX_objs.h"
#include "osmonweb_ITRON_objs.h"


static char osmonweb_hostname[128];

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
  )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    -1: a < b; 0: a == b; +1: a > b                                        |
\*=========================================================================*/
{
  const osmonweb_obj_data_t *oa = (const osmonweb_obj_data_t *)a;
  const osmonweb_obj_data_t *ob = (const osmonweb_obj_data_t *)b;
  uint32_t cmp_idx = oa->common_options->sort_select;
  int result;

  if (cmp_idx >= ARRAY_COUNT(ob->data)) {
    cmp_idx = 0;
  }
  if ((oa->data[cmp_idx].strarg != NULL) &&
      (ob->data[cmp_idx].strarg != NULL)) {
    result = strcmp(oa->data[cmp_idx].strarg,ob->data[cmp_idx].strarg);
  }
  else {
    result = oa->data[cmp_idx].intarg - ob->data[cmp_idx].intarg;
  }
  if (oa->common_options->sort_dir < 0) {
    result = -result;
  }
  return result;
}


const osmonweb_fragment_ctrl_t osmonweb_POSIX_objs_fragment[] = {
    {osmonweb_POSIX_objs_title,   NULL                        , FALSE},
    {osmonweb_POSIX_objs_header,  NULL                        , FALSE},
    {osmonweb_POSIX_objs_footer,  NULL                        , FALSE},
    {osmonweb_POSIX_objs_trailer, NULL                        , FALSE},
    {NULL,FALSE}
};

const osmonweb_fragment_ctrl_t osmonweb_ITRON_objs_fragment[] = {
    {osmonweb_ITRON_objs_title,   NULL                        , FALSE},
    {osmonweb_ITRON_objs_header,  NULL                        , FALSE},
    {osmonweb_ITRON_objs_footer,  NULL                        , FALSE},
    {osmonweb_ITRON_objs_trailer, NULL                        , FALSE},
    {NULL,FALSE}
};

osmonweb_objtype_t POSIX_objtypes[] = {
  {NULL,NULL,NULL,NULL,{NULL,NULL}}
};

osmonweb_objtype_t ITRON_objtypes[] = {
  {NULL,NULL,NULL,NULL,{NULL,NULL}}
};

osmonweb_api_entry_t osmonweb_api_entry[]={
  { "RTEID",osmonweb_RTEID_objs_fragment,RTEID_objtypes}
  ,{"ITRON",osmonweb_ITRON_objs_fragment,ITRON_objtypes}
  ,{"POSIX",osmonweb_POSIX_objs_fragment,POSIX_objtypes}
  ,{NULL   ,osmonweb_RTEID_objs_fragment,NULL} /* default... */
  ,{NULL,NULL,NULL}
};

/*
 * interface functions
 */

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void osmonweb_cleanup_query
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   process form request for osmon URL                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 osmonweb_common_option_t *option_ptr,
 char *query
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  sprintf(query,
    "api=%s"
    "&obj_type=%s"
    "&multi_node_enable=%d"
    "&node_curr=%d"
    "&auto_rfsh_enab=%d"
    "&auto_rfsh_time=%d"
    "&sort_dir=%d"
    "&sort_select=%d"
    "&hdr_rep_enable=%d"
    "&hdr_rep_cnt=%d",
    option_ptr->api_type
    ,option_ptr->obj_type
    ,option_ptr->multi_node_enable
    ,option_ptr->node_curr
    ,option_ptr->auto_rfsh_enable
    ,option_ptr->auto_rfsh_time
    ,option_ptr->sort_dir
    ,option_ptr->sort_select
    ,option_ptr->hdr_rep_enable
    ,option_ptr->hdr_repcnt);

  if (option_ptr->obj_curr != 0) {
    sprintf(query+strlen(query),"&obj_curr=0x%08x",
      option_ptr->obj_curr);
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void osmonweb_form
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   process form request for osmon URL                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 osmonweb_ctx_t wp,        /* web handle */
 char *path,
 char *query
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  int data_items,data_cnt;
  uint32_t collect_key;
  boolean finished = FALSE;
  boolean nosort_warning_sent = FALSE;
  boolean collect_success;
  int output_idx;
  int api_idx;
  osmonweb_common_option_t common_options;
  html_printf_arg_t  common_opt_args[13];
  osmonweb_obj_data_t  obj_data[32];
  const osmonweb_objtype_t *osmonweb_api_obj;
  const osmonweb_fragment_ctrl_t *fragment_list
    = osmonweb_RTEID_objs_fragment;
  boolean display_single_object = FALSE;
  char clean_query[256];
  void *toFree;
  /*
   * get standard fields (or set them to defaults)
   */
  /*
   * options:
   * - multi-node display
   * - default node (?)
   * - API type selector    (RTEID,POSIX,ITRON,OSEK...)
   * - object type selector (task,sema,...)
   * - multi object enable
   * - object selector
   * - auto-refresh enable
   * - auto-refresh-interval
   * - table header repetition
   */
  toFree = osmonweb_getCommonOptions( wp, &common_options );

  /*
   * collect common options into a standard parameter list
   */
  common_opt_args[ 0].strarg = common_options.api_type;
  common_opt_args[ 1].strarg = common_options.obj_type;
  common_opt_args[ 2].intarg = common_options.multi_node_enable;
  common_opt_args[ 3].intarg = common_options.node_curr;
  common_opt_args[ 4].strarg = osmonweb_hostname;
  common_opt_args[ 5].intarg = common_options.obj_curr;
  common_opt_args[ 6].intarg = common_options.auto_rfsh_enable;
  common_opt_args[ 7].intarg =
    (common_options.auto_rfsh_enable
     ? common_options.auto_rfsh_time
     : 365*24*60*60);
  common_opt_args[ 8].intarg = common_options.sort_dir;
  common_opt_args[ 9].intarg = common_options.sort_select;
  common_opt_args[10].intarg = common_options.hdr_rep_enable;
  common_opt_args[11].intarg = common_options.hdr_repcnt;

  osmonweb_cleanup_query(&common_options,clean_query);
  common_options.clean_query = clean_query;
  common_opt_args[12].strarg = clean_query;

  display_single_object = (common_options.obj_curr != 0);
  /*
   * init web output
   */
  osmonweb_header( wp );

  /*
   * find selected api
   */
  api_idx = 0;
  osmonweb_api_obj = NULL;
  while ((osmonweb_api_entry[api_idx].name != NULL) &&
         (0 != strcmp(osmonweb_api_entry[api_idx].name,
                      common_options.api_type))) {
    api_idx++;
  }

  /*
   * find selected object type
   */
  if (osmonweb_api_entry[api_idx].name != NULL) {
    fragment_list = osmonweb_api_entry[api_idx].default_framents;
    osmonweb_api_obj = osmonweb_api_entry[api_idx].objtypes;
    while ((osmonweb_api_obj->name != NULL) &&
           (0 != strcmp(osmonweb_api_obj->name,common_options.obj_type))) {
      osmonweb_api_obj++;
    }
  }
  if ((osmonweb_api_obj != NULL) &&
      (osmonweb_api_obj->name == NULL)) {
    osmonweb_api_obj = NULL;
  }
  if (osmonweb_api_obj == NULL) {
    /*
     * send default fragment list
     */
    while ((fragment_list != NULL)  &&
           (fragment_list->form != NULL)) {
      html_printf(fragment_list->form,
                  ARRAY_COUNT(common_opt_args),
                  common_opt_args,
                  (html_printf_outfnc_t)osmonweb_WriteBlock,
                  wp);
      fragment_list++;
    }
  }
  else {
    /*
     * perform an action, if needed
     */
    if ((common_options.action != NULL) &&
	(osmonweb_api_obj->action_fnc != NULL)) {
      osmonweb_api_obj->action_fnc(wp,common_options.action,&common_options);
    }
    fragment_list =
      osmonweb_api_obj->fragment[display_single_object ? 1 : 0];
    /*
     * output page title and header
     * include common info and control values
     */
    html_printf(fragment_list[FRAGCTRL_TITLE].form,
                ARRAY_COUNT(common_opt_args),
                common_opt_args,
                (html_printf_outfnc_t)osmonweb_WriteBlock,
                wp);

    html_printf(fragment_list[FRAGCTRL_HEADER].form,
                ARRAY_COUNT(common_opt_args),
                common_opt_args,
                (html_printf_outfnc_t)osmonweb_WriteBlock,
                wp);

    data_items = 0;
    collect_key = common_options.obj_curr;
    while (!finished) {
      data_cnt = 0;
      /*
       * fill object data array with zero
       */
      memset(obj_data,0,sizeof(obj_data));
      while (!finished &&
             (data_cnt < (display_single_object
                          ? 1
                          : ARRAY_COUNT(obj_data)))) {
        /*
         * collect data into data vector
         * FIXME: adapt call to real data structures
         */
        collect_success =
          osmonweb_api_obj->collect_fnc(&common_options,
                                        &collect_key,
                                        ARRAY_COUNT(obj_data[data_cnt].data),
                                        obj_data[data_cnt].data);
        obj_data[data_cnt].common_options = &common_options;
        /*
         * FIXME: add reference to common options into each data entry
         * to allow access to these options inside sort compare function
         */
        if (collect_success) {
          data_cnt++;
        }
        if (!collect_success || display_single_object) {
          finished = TRUE;
        }
      }
      if (data_cnt > 0) {
        /*
         * if data does not fit into vector and
         * sorting is active, output warning and do not sort
         */
        if ((data_cnt >= ARRAY_COUNT(obj_data)) &&
            (common_options.sort_dir != 0) &&
            !nosort_warning_sent) {
          /*
           * FIXME: print "not sorted" warning
           */
          nosort_warning_sent = TRUE;
        }
        /*
         * if requested and possible (no overflow), sort vector
         */
        if ((common_options.sort_dir != 0) &&
            !nosort_warning_sent) {
          qsort(obj_data,data_cnt,sizeof(obj_data[0]),
                osmonweb_compare);
        }

        for (output_idx = 0;
             output_idx < data_cnt;
             output_idx++) {
          /*
           * output table info, insert table header at top
           * and in between, if requested
           */
          if ((data_items == 0) ||
              ((common_options.hdr_repcnt     > 0) &&
               common_options.hdr_rep_enable       &&
               ((data_items % common_options.hdr_repcnt) == 0))) {
            /*
             * print table header row
             */
            html_printf(fragment_list[FRAGCTRL_TABHEAD].form,
                        ARRAY_COUNT(common_opt_args),
                        common_opt_args,
                        (html_printf_outfnc_t)osmonweb_WriteBlock,
                        wp);
          }
          /*
           * print table info row
           */
          if (fragment_list[FRAGCTRL_TABENTRY].output == NULL) {
            html_printf(fragment_list[FRAGCTRL_TABENTRY].form,
                        ARRAY_COUNT(obj_data[output_idx].data),
                        obj_data[output_idx].data,
                        (html_printf_outfnc_t)osmonweb_WriteBlock,
                        wp);
          }
          else {
            fragment_list[FRAGCTRL_TABENTRY].output
              (fragment_list[FRAGCTRL_TABENTRY].form,
               ARRAY_COUNT(obj_data[output_idx].data),
               obj_data[output_idx].data,
               (html_printf_outfnc_t)osmonweb_WriteBlock,
               wp);
          }
          data_items++;
        }
        /*
         * call cleanup to deallocate any allocated memory in obj_data
         */
        while (data_cnt > 0) {
          if (osmonweb_api_obj->cleanup_fnc != NULL) {
            osmonweb_api_obj->cleanup_fnc(ARRAY_COUNT(obj_data[data_cnt].data),
                                          obj_data[data_cnt].data);
          }
          data_cnt--;
        }
      }
    }
    if (!collect_success) {
      /*
       * FIXME: print warning about error during collect
       */
    }
    /*
     * send table footer
     */
    html_printf(fragment_list[FRAGCTRL_TABFOOT].form,
                ARRAY_COUNT(common_opt_args),
                common_opt_args,
                (html_printf_outfnc_t)osmonweb_WriteBlock,
                wp);

    /*
     * send page footer
     */
    html_printf(fragment_list[FRAGCTRL_FOOTER].form,
                ARRAY_COUNT(common_opt_args),
                common_opt_args,
                (html_printf_outfnc_t)osmonweb_WriteBlock,
                wp);

    /*
     * send trailor
     */
    html_printf(fragment_list[FRAGCTRL_TRAILER].form,
                ARRAY_COUNT(common_opt_args),
                common_opt_args,
                (html_printf_outfnc_t)osmonweb_WriteBlock,
                wp);
  }

  /*
   * Print footer and terminate this transaction
   */
  osmonweb_footer( wp );

  /* free whatever memory parsing the options required */
  if ( toFree )
    free( toFree );
}

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
 const char *web_root_dir,   /* web root directory */
 int phase
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  char dir[512];
  int rc;

  if (phase == 1) {
    /*
     * init monitor as far as needed...
     */
    #if defined(USE_MONITOR)
      printf("Calling monitor_init\n");
      rtems_monitor_init(RTEMS_MONITOR_NOTASK);
      printf("Returned from monitor_init\n");
    #endif

    /*
     * untar graphics elements needed
     */
    getcwd(dir,sizeof(dir));
    printf("previous working directory was %s\n",dir);

    rc = chdir(web_root_dir);
    printf("chdir %s returned %d\n",web_root_dir,rc);

    rc = mkdir("./osmonweb",S_IRWXU | S_IRWXG | S_IRWXO);
    printf("mkdir(./osmonweb) returned %d\n",rc);
    rc = chdir("./osmonweb");
    printf("chdir(./osmonweb) returned %d\n",rc);

    printf("Calling UntarFromMemory\n");
    rc = Untar_FromMemory((void *)osmonweb_tar, osmonweb_tar_size);
    printf("UntarFromMemory returns %d\n",rc);
    rc = chdir(dir);
    printf("chdir(%s) returned %d\n",dir,rc);
  }
  if (phase == 2) {
    /*
     * try to get hostname
     */
    if (gethostname(osmonweb_hostname, sizeof(osmonweb_hostname)) < 0) {
      strcpy(osmonweb_hostname,"RTEMS Target");
    }
    /*
     * define form handler
     */
    #if defined(USE_GOAHEAD_HTTPD)
      websFormDefine(T("osmonweb"), osmonweb_form);
    #endif
  }
  return RTEMS_SUCCESSFUL;
}

#if defined(USE_SIMPLE_HTTPD)
  void osmonweb_register(
    struct shttpd_ctx *ctx
  )
  {
    shttpd_register_uri(ctx, "/goforms/osmonweb*", (void *)osmonweb_form, NULL);
  }
#endif
