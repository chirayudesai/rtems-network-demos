/*  Init
 *
 *  This routine is the initialization task for this test program.
 *
 *  Don't forget to change the IP addresses
 */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS	20
#define CONFIGURE_MAXIMUM_PTYS                          1
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MEMORY_OVERHEAD       256
#define CONFIGURE_MESSAGE_BUFFER_MEMORY 32 * 1024
#define CONFIGURE_MAXIMUM_SEMAPHORES	40
#define CONFIGURE_MAXIMUM_TASKS		20
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES	20

#define CONFIGURE_MICROSECONDS_PER_TICK	1000

#define CONFIGURE_INIT_TASK_STACK_SIZE	(64*1024)
#define CONFIGURE_INIT_TASK_PRIORITY	120
#define CONFIGURE_INIT_TASK_ATTRIBUTES    RTEMS_FLOATING_POINT
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_MAXIMUM_DRIVERS 10
#define CONFIGURE_INIT

#include <stdlib.h>
#include <rtems.h>
#include <rtems/telnetd.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

/* configuration information */

#include <rtems/confdefs.h>
#include <bsp.h>

#include <errno.h>
#include <time.h>

#include <rtems/confdefs.h>
#include <stdio.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/telnetd.h>
#include <rtems/shell.h>

     
#include <rtems/error.h>
#include <rpc/rpc.h>
#include <netinet/in.h>
#include <time.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include "../networkconfig.h"

/*
 *  Number of sessions
 */
int session = 0;

/*
 *  Macro to printf and printk the same arguments
 */

#define printKF( ... ) \
  do { \
    printf( __VA_ARGS__ ); \
    printk( __VA_ARGS__ ); \
  } while (0)
 

/*
 *  echo shell
 */
void echoShell(
  char *pty_name,
  void *cmd_arg
)
{
  int cmds = 0;
  char line[256];
  char *c;
  int l;

  ++session;
  printKF( "Connected to %s with argument %p for session %d\n",
          pty_name, cmd_arg, session );

  while (1) {
    cmds++;
    printf( "> " );
    c = fgets( line, 256, stdin );
    if ( !c ) {
      printKF( "Connection terminated\n");
      return;
    }
    l = strlen( line );
    if ( line[l-1] == '\n' ) {
      line[l-1] = '\0';
    }
    if ( !strcmp( line, "bye" ) ) {
      printKF( "%s", "Terminating connection\n");
      return;
    }
    if ( !strcmp( line, "exit" ) ) {
      printKF("\n\n*** End of Telnetd Server Test ***\n\r" );
      exit(0);
    }
    printKF( "echo %d-%d> %s\n", session, cmds, line );
  }
}

/*
 *  Init task
 */
rtems_task Init(
  rtems_task_argument argument
)
{
  printf("\n\n*** Telnetd Server Test ***\n\r" );

  printf("============== Initializing Network ==============\n");
  rtems_bsdnet_initialize_network ();

  printf("============== Add Route ==============\n");
  rtems_bsdnet_show_inet_routes ();

  printf("============== Start Telnetd ==============\n");
  printk( "Starting echoShell via telnetd -- default password is rtems\n" );
  rtems_telnetd_initialize(
    echoShell,                 /* "shell" function */
    NULL,                      /* no context necessary for echoShell */
    FALSE,                     /* spawn a new thread */
    RTEMS_MINIMUM_STACK_SIZE,  /* default stack size please */
    1                          /* priority .. we feel important today */
  ); 

  printf("============== Deleting Init Task ==============\n");
  rtems_task_delete(RTEMS_SELF);
}

