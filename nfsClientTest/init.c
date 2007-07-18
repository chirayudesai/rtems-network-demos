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

#include <rtems.h>
#include <librtemsNfs.h>

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

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  int int_status;

  printf("\n\n*** NFS Client TEST ***\n\r" );

  printf( "Free space %d\n", malloc_free_space() );
  /*
   * Load filesystem image
   */
  printf("=============== Loading filesystem image ===============\n");
  status = Untar_FromMemory((void *)(&TARFILE_START), (size_t)&TARFILE_SIZE);
   
  printf("============== Look at Local Filesystem ==============\n");
  printf( "PWD: " );
  pwd(); 
  
  printf( "\n--->ls /\n" );
  ls("/");

  printf( "\n--->ls /etc\n" );
  ls("/etc");

  printf("============== Initializing Network ==============\n");
  rtems_bsdnet_initialize_network ();

  printf("============== Add Route ==============\n");
  rtems_bsdnet_show_inet_routes ();

  printf("============== Initializing RPC ==============\n");
  int_status = rpcUdpInit();
  if ( int_status )
    printf( "RPC UDP Initialization failed\n" );

  printf("============== Initializing NFS Subsystem ==============\n");
  nfsInit( 0, 0 );

  printf("============== Mounting Remote Filesystem ==============\n");
#if 1
  /* This code uses the NFS mount wrapper function */
  int_status = nfsMount("192.168.1.210", "/home", "/home" );
#else
  /* This section does it more explicitly */
  mkdir( "/home", 0777 );
  int_status = mount(
    NULL,                        /* mount_table_entry_pointer */
    &nfs_fs_ops,                 /* filesystem_operations_table_pointer */
    RTEMS_FILESYSTEM_READ_WRITE, /* options */
    "192.168.1.210:/home",       /* device aka remote filesystem */
    "/home"                      /* mount_point */
  );
#endif
  if ( int_status )
    printf( "NFS Mount failed -- %s\n", strerror(errno) );

  printf("============== Look at Remote Filesystem ==============\n");
  printf( "\n---> ls /home\n" );
  ls("/home");
  printf( "\n---> ls /home/nfstest\n" );
  ls("/home/nfstest");

  exit(0);

  status = rtems_task_delete( RTEMS_SELF );
}

