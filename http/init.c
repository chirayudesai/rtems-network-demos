/*  Init
 *
 *  This routine is the initialization task for this test program.
 *
 *  Don't forget to change the IP addresses
 */

#define USE_HTTPD
#define USE_FTPD
#define TEST_INIT

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS	20
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

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

#include <rtems/confdefs.h>
#include <stdio.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/ftpd.h>
#include <rtems/untar.h>

     
#include <rtems/error.h>
#include <rpc/rpc.h>
#include <netinet/in.h>
#include <time.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include "../networkconfig.h"

#include <rtems_webserver.h>

#define ARGUMENT 0

/*
 *  The tarfile is built automatically externally so we need to account
 *  for the leading symbol on the names.
 */
#if defined(__sh__)
  #define SYM(_x) _x
#else
  #define SYM(_x) _ ## _x
#endif

extern int SYM(binary_tarfile_start);
extern int SYM(binary_tarfile_size);
#define TARFILE_START SYM(binary_tarfile_start)
#define TARFILE_SIZE SYM(binary_tarfile_size)

#if defined(USE_FTPD)
struct rtems_ftpd_configuration rtems_ftpd_configuration = {
   10,                     /* FTPD task priority            */
   1024,                   /* Maximum buffersize for hooks  */
   21,                     /* Well-known port     */
   NULL                    /* List of hooks       */
};
#endif
rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  printf("\n\n*** HTTP TEST ***\n\r" );

  status = Untar_FromMemory((void *)(&TARFILE_START), (size_t)&TARFILE_SIZE);
   
  rtems_bsdnet_initialize_network ();
#if defined(USE_FTPD)
  rtems_initialize_ftpd();
#endif

#if defined(USE_HTTPD)
  rtems_initialize_webserver();
#endif

  status = rtems_task_delete( RTEMS_SELF );
}




