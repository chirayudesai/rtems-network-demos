/*===============================================================*\
| Project: OSMONWEB web interface to RTEMS monitor information    |
+-----------------------------------------------------------------+
| File: osmonweb_RTEID.c                                          |
+-----------------------------------------------------------------+
|                    Copyright (c) 2003 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file contains the RTEID-specific functions to visualize the|
| RTEMS OS structures with a web browser                          |
| GNU C Compiler                                                  |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 12.06.03  creation                                         doe  |
|*****************************************************************|
|*RCS information:                                                |
|*(the following information is created automatically,            |
|*do not edit here)                                               |
|*****************************************************************|
|* $Log$
|* Revision 1.3  2003/10/13 19:25:35  thomas
|* ballcart demo works fine (more or less)
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

#include <rtems.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <rtems/untar.h>

#include "osmonweb.h"
#include "osmonweb_int.h"

#include  <signal.h>
#include  <sys/time.h>
#include  <pthread.h>

#include        <rtems/error.h>

#include "osmonweb_RTEID_objs.h"
#include "osmonweb_RTEID_queues.h"
#include "osmonweb_RTEID_semas.h"
#include "osmonweb_RTEID_onesema.h"
#include "osmonweb_RTEID_tasks.h"
#include "osmonweb_RTEID_onetask.h"

#include "htmlprintf.h"
#if defined(USE_MONITOR)
  #include <rtems/monitor.h>
#endif


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
uint32_t osmonweb_rteid_task_entry
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   get next RTEID task entry                                               |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 osmonweb_common_option_t *common_options,
 uint32_t *cookie_ptr,
 uint32_t result_cnt,
 html_printf_arg_t *results
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    next cookie                                                            |
\*=========================================================================*/
{
#if defined(USE_MONITOR)
  rtems_id curr_id = *cookie_ptr;
  rtems_id next_id = RTEMS_OBJECT_ID_FINAL;
  rtems_monitor_union_t canonical;
  rtems_monitor_object_info_t *info = NULL;
  rtems_interval curr_tick,ticks_per_sec;

  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_sec);
  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &curr_tick);

  if (curr_id == 0) {
    curr_id = RTEMS_OBJECT_ID_INITIAL(
      OBJECTS_CLASSIC_API,
      OBJECTS_RTEMS_TASKS,
      rtems_monitor_default_node
    );
  }
  info = rtems_monitor_object_lookup(RTEMS_MONITOR_OBJECT_TASK);
  if (info != NULL) {
    next_id = rtems_monitor_object_canonical_next(info,
                                                  curr_id,
                                                  &canonical);
    results[ 0].intarg = canonical.task.id;
    results[ 1].intarg = canonical.task.name;
    results[ 2].intarg = (uint32_t)canonical.task.entry;
    results[ 3].intarg = canonical.task.argument;
    results[ 4].intarg = (uint32_t)canonical.task.stack;
    results[ 5].intarg = canonical.task.stack_size;
    results[ 6].intarg = canonical.task.priority;

    if (STATES_READY == (canonical.task.state & STATES_ALL_SET)) {
      results[ 7].intarg = 0;
    }
    else if (0 != (canonical.task.state & STATES_DORMANT)) {
      results[ 7].intarg = 1;
    }
    else if (0 != (canonical.task.state & STATES_SUSPENDED)) {
      results[ 7].intarg = 2;
    }
    else if (0 != (canonical.task.state & STATES_DELAYING)) {
      results[ 7].intarg = 3;
    }
    else {
      results[ 7].intarg = 4;
    }
    results[ 8].intarg = canonical.task.events;
    results[ 9].intarg = canonical.task.modes;
    results[10].intarg = canonical.task.attributes;
    results[11].intarg = canonical.task.wait_id;
    results[12].intarg = canonical.task.wait_args;
    results[13].intarg = 0; /* XXX canonical.task.ticks */;
    results[14].intarg = 0; /* ((uint64_t)1000)*canonical.task.ticks / curr_tick; */
    results[15].strarg = common_options->clean_query;

    *cookie_ptr = next_id;
  }
  return (next_id != RTEMS_OBJECT_ID_FINAL);
#else
  uint32_t cookie = *cookie_ptr;
  /*
   * FIXME: this is dummy data for now...
   * see monitor.h, rtems_monitor_task_t
   */
  if (result_cnt < 11) {
    cookie = 0;
  }
  else {
    results[ 0].intarg = 0x10000000+cookie;  /* Task ID */
    results[ 1].intarg = 
      rtems_build_name('T','S','K','a'+cookie);  /* Task Name */
    results[ 2].intarg = 0x00000000+(cookie*0x100);     /* task entry      */
    results[ 3].intarg = 0x01000000+(cookie*0x200);     /* task arg        */
    results[ 4].intarg = 0x02000000+(cookie*0x300);     /* task stack      */
    results[ 5].intarg = 0x00010000+(cookie*0x300);     /* task stack size */
    results[ 6].intarg = 0x00000001+((cookie*60)%256);  /* task priority   */
    results[ 7].intarg = 0;                             /* task state      */
    results[ 8].intarg = 0x12340000+cookie;             /* task event set  */
    results[ 9].intarg = 0;                             /* task mode       */
    results[10].intarg = 0;                             /* task attributes */
    /* Notepads, wait_id,wait_args: ommitted */
    cookie += 1; /* get next task... */
  }
  *cookie_ptr = cookie;
  return ((cookie > 0) && (cookie <= 10));
#endif
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
uint32_t osmonweb_rteid_task_action
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   process a selected action item                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 osmonweb_ctx_t wp,                                    /* web handle               */
 const char *act_str,                          /* action string            */
 osmonweb_common_option_t *common_options
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;

  if ((common_options->obj_curr != 0) &&
      (0 == strcmp(act_str,"suspend"))) {
    rc = rtems_task_suspend(common_options->obj_curr);
  }
  else if ((common_options->obj_curr != 0) &&
      (0 == strcmp(act_str,"resume"))) {
    rc = rtems_task_resume(common_options->obj_curr);
  }
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
uint32_t osmonweb_rteid_queue_entry
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   get next RTEID message queue entry                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 osmonweb_common_option_t *common_options,
 uint32_t *cookie_ptr,
 uint32_t result_cnt,
 html_printf_arg_t *results
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    next cookie                                                            |
\*=========================================================================*/
{
  rtems_id curr_id = *cookie_ptr;
  rtems_id next_id = RTEMS_OBJECT_ID_FINAL;
  rtems_monitor_union_t canonical;
  rtems_monitor_object_info_t *info = NULL;

  if (curr_id == 0) {
    curr_id = RTEMS_OBJECT_ID_INITIAL(
      OBJECTS_CLASSIC_API,
      RTEMS_MONITOR_OBJECT_QUEUE, 
      rtems_monitor_default_node);
  }
  info = rtems_monitor_object_lookup(RTEMS_MONITOR_OBJECT_QUEUE);
  if (info != NULL) {
    next_id = rtems_monitor_object_canonical_next(info,
                                                  curr_id,
                                                  &canonical);
    results[ 0].intarg = canonical.queue.id;
    results[ 1].intarg = canonical.queue.name;
    results[ 2].intarg = 0;
    results[ 3].intarg = canonical.queue.number_of_pending_messages;
    results[ 4].intarg = canonical.queue.maximum_message_size;
    results[ 5].intarg = canonical.queue.maximum_pending_messages;
    results[ 6].intarg = 0;
    results[ 7].intarg = 0;
    results[ 8].intarg = 0;
    results[ 9].intarg = 0;
    results[10].intarg = canonical.queue.attributes;
    if (canonical.queue.maximum_pending_messages <= 0) {
      results[11].intarg = 0;
    }
    else {
      results[11].intarg 
        = (((uint64_t)10000)
           *canonical.queue.number_of_pending_messages
           /canonical.queue.maximum_pending_messages);
    }
    results[12].intarg = 0;
    results[13].intarg = 0;
    results[14].intarg = 0;
    results[15].strarg = common_options->clean_query;

    *cookie_ptr = next_id;
  }
  return (next_id != RTEMS_OBJECT_ID_FINAL);
}

#if defined(JOEL)
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
uint32_t osmonweb_rteid_sema_entry
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   get next RTEID semaphore entry                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 osmonweb_common_option_t *common_options,
 uint32_t *cookie_ptr,
 uint32_t result_cnt,
 html_printf_arg_t *results
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    next cookie                                                            |
\*=========================================================================*/
{
  rtems_id curr_id = *cookie_ptr;
  rtems_id next_id = RTEMS_OBJECT_ID_FINAL;
  rtems_monitor_union_t canonical;
  rtems_monitor_object_info_t *info = NULL;

  if (curr_id == 0) {
    curr_id = RTEMS_OBJECT_ID_INITIAL(
      OBJECTS_CLASSIC_API,
      RTEMS_MONITOR_OBJECT_SEMAPHORE, 
      rtems_monitor_default_node);
  }
  info = rtems_monitor_object_lookup(RTEMS_MONITOR_OBJECT_SEMAPHORE);
  if (info != NULL) {
    next_id = rtems_monitor_object_canonical_next(info,
                                                  curr_id,
                                                  &canonical);
    results[ 0].intarg = canonical.sema.id;
    results[ 1].intarg = canonical.sema.name;
    results[ 2].intarg = 0;
    results[ 3].intarg = 0;
    results[ 4].intarg = 0;
    results[ 5].intarg = canonical.sema.max_count;
    results[ 6].intarg = canonical.sema.cur_count;
    results[ 7].intarg = 0;
    results[ 8].intarg = 0;
    results[ 9].intarg = canonical.sema.priority_ceiling;
    results[10].intarg = canonical.sema.attribute;
    results[11].intarg = canonical.sema.holder_id;
    results[12].intarg = 0;
    results[13].intarg = 0;
    results[14].intarg = 0;
    results[15].strarg = common_options->clean_query;

    *cookie_ptr = next_id;
  }
  return (next_id != RTEMS_OBJECT_ID_FINAL);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
uint32_t osmonweb_rteid_sema_action
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   process a selected action item                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 osmonweb_ctx_t wp,                                    /* web handle               */
 const char *act_str,                          /* action string            */
 osmonweb_common_option_t *common_options
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;

  if ((common_options->obj_curr != 0) &&
      (0 == strcmp(act_str,"obtain"))) {
    rc = rtems_semaphore_obtain(common_options->obj_curr,
                                RTEMS_NO_WAIT,0);
  }
  else if ((common_options->obj_curr != 0) &&
      (0 == strcmp(act_str,"release"))) {
    rc = rtems_semaphore_release(common_options->obj_curr);
  }
  return rc;
}
#endif

/* 
 * control data structures
 */

const osmonweb_fragment_ctrl_t osmonweb_RTEID_objs_fragment[] = {     
    {osmonweb_RTEID_objs_title,   NULL                        , FALSE},
    {osmonweb_RTEID_objs_header,  NULL                        , FALSE},
    {osmonweb_RTEID_objs_footer,  NULL                        , FALSE},
    {osmonweb_RTEID_objs_trailer, NULL                        , FALSE},
    {NULL,FALSE}
};

const osmonweb_fragment_ctrl_t osmonweb_RTEID_task_fragment[] = {     
    {osmonweb_RTEID_task_title,   NULL                        , FALSE},
    {osmonweb_RTEID_task_header,  NULL                        , FALSE},
    {osmonweb_RTEID_task_tabhead, NULL                        , FALSE},
    {osmonweb_RTEID_task_tabentry,NULL/*osmonweb_RTEID_task_format*/  , TRUE },
    {osmonweb_RTEID_task_tabfoot, NULL                        , FALSE},
    {osmonweb_RTEID_task_footer,  NULL                        , FALSE},
    {osmonweb_RTEID_task_trailer, NULL                        , FALSE},
    {NULL,FALSE}
};

const osmonweb_fragment_ctrl_t osmonweb_RTEID_onetask_fragment[] = {     
    {osmonweb_RTEID_onetask_title,   NULL                     , FALSE},
    {osmonweb_RTEID_onetask_header,  NULL                     , FALSE},
    {osmonweb_RTEID_onetask_tabhead, NULL                     , FALSE},
    {osmonweb_RTEID_onetask_tabentry,NULL                     , TRUE },
    {osmonweb_RTEID_onetask_tabfoot, NULL                     , FALSE},
    {osmonweb_RTEID_onetask_footer,  NULL                     , FALSE},
    {osmonweb_RTEID_onetask_trailer, NULL                     , FALSE},
    {NULL,FALSE}
};

const osmonweb_fragment_ctrl_t osmonweb_RTEID_queue_fragment[] = {     
    {osmonweb_RTEID_queue_title,   NULL                        , FALSE},
    {osmonweb_RTEID_queue_header,  NULL                        , FALSE},
    {osmonweb_RTEID_queue_tabhead, NULL                        , FALSE},
    {osmonweb_RTEID_queue_tabentry,NULL/*osmonweb_RTEID_queue_format*/  , TRUE },
    {osmonweb_RTEID_queue_tabfoot, NULL                        , FALSE},
    {osmonweb_RTEID_queue_footer,  NULL                        , FALSE},
    {osmonweb_RTEID_queue_trailer, NULL                        , FALSE},
    {NULL,FALSE}
};

const osmonweb_fragment_ctrl_t osmonweb_RTEID_onequeue_fragment[] = {     
    {osmonweb_RTEID_queue_title,   NULL                     , FALSE},
    {osmonweb_RTEID_queue_header,  NULL                     , FALSE},
    {osmonweb_RTEID_queue_tabhead, NULL                     , FALSE},
    {osmonweb_RTEID_queue_tabentry,NULL                     , TRUE },
    {osmonweb_RTEID_queue_tabfoot, NULL                     , FALSE},
    {osmonweb_RTEID_queue_footer,  NULL                     , FALSE},
    {osmonweb_RTEID_queue_trailer, NULL                     , FALSE},
    {NULL,FALSE}
};

#if defined(JOEL)
const osmonweb_fragment_ctrl_t osmonweb_RTEID_sema_fragment[] = {
    {osmonweb_RTEID_sema_title,   NULL                        , FALSE},
    {osmonweb_RTEID_sema_header,  NULL                        , FALSE},
    {osmonweb_RTEID_sema_tabhead, NULL                        , FALSE},
    {osmonweb_RTEID_sema_tabentry,NULL/*osmonweb_RTEID_sema_format*/, TRUE },
    {osmonweb_RTEID_sema_tabfoot, NULL                        , FALSE},
    {osmonweb_RTEID_sema_footer,  NULL                        , FALSE},
    {osmonweb_RTEID_sema_trailer, NULL                        , FALSE},
    {NULL,FALSE}
};

const osmonweb_fragment_ctrl_t osmonweb_RTEID_onesema_fragment[] = {
    {osmonweb_RTEID_onesema_title,   NULL                        , FALSE},
    {osmonweb_RTEID_onesema_header,  NULL                        , FALSE},
    {osmonweb_RTEID_onesema_tabhead, NULL                        , FALSE},
    {osmonweb_RTEID_onesema_tabentry,NULL/*osmonweb_RTEID_sema_format*/, TRUE },
    {osmonweb_RTEID_onesema_tabfoot, NULL                        , FALSE},
    {osmonweb_RTEID_onesema_footer,  NULL                        , FALSE},
    {osmonweb_RTEID_onesema_trailer, NULL                        , FALSE},
    {NULL,FALSE}
};
#endif

osmonweb_objtype_t RTEID_objtypes[] = {
  {"task",
   osmonweb_rteid_task_entry, NULL, osmonweb_rteid_task_action, 
   {osmonweb_RTEID_task_fragment, osmonweb_RTEID_onetask_fragment}
  }
  ,{"queue",
   osmonweb_rteid_queue_entry, NULL, NULL, 
   {osmonweb_RTEID_queue_fragment,osmonweb_RTEID_onequeue_fragment}
  }
#if defined(JOEL)
  ,{"sema",
    osmonweb_rteid_sema_entry, NULL, osmonweb_rteid_sema_action,
    {osmonweb_RTEID_sema_fragment,osmonweb_RTEID_onesema_fragment}
  }
#endif
  ,{NULL,NULL,NULL,NULL,{NULL,NULL}}
};

