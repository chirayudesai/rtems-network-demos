/*  Init
 *
 *  This routine is the initialization task for this test program.
 *
 *  Don't forget to change the IP addresses
 */

#define TEST_INIT

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXECUTIVE_RAM_SIZE	(512*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES	20
#define CONFIGURE_MAXIMUM_TASKS		20

#define CONFIGURE_MICROSECONDS_PER_TICK	10000

#define CONFIGURE_INIT_TASK_STACK_SIZE	(10*1024)
#define CONFIGURE_INIT_TASK_PRIORITY	120
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_INIT

#include "system.h"

#include <errno.h>
#include <time.h>

#include <confdefs.h>
#include <stdio.h>
#include <rtems/rtems_bsdnet.h>
#include <ftpd.h>

     
#include <rtems/error.h>
#include <rpc/rpc.h>
#include <netinet/in.h>
#include <time.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include "../networkconfig.h"

#include <rtems_webserver.h>

#define ARGUMENT 0

extern int _binary_tarfile_start;
extern int _binary_tarfile_size;

struct rtems_ftpd_configuration rtems_ftpd_configuration = {
   10,                     /* FTPD task priority            */
   1024,                   /* Maximum buffersize for hooks  */
   80,                     /* Well-known port     */
   NULL                    /* List of hooks       */
};
rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  printf("\n\n*** HTTP TEST ***\n\r" );

  /* init_paging(); */

  rtems_bsdnet_initialize_network ();

  rtems_initialize_ftpd();

  status = Untar_FromMemory((unsigned char *)(&_binary_tarfile_start),
			    &_binary_tarfile_size);
   
  rtems_initialize_webserver();

  status = rtems_task_delete( RTEMS_SELF );
}




