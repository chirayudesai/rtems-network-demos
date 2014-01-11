/*  Init
 *
 *  This routine is the initialization task for this test program.
 *
 *  Don't forget to change the IP addresses
 */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS        20
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_EXECUTIVE_RAM_SIZE    (512*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES    20
#define CONFIGURE_MAXIMUM_TASKS         20

#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_INIT_TASK_STACK_SIZE  (10*1024)
#define CONFIGURE_INIT_TASK_PRIORITY    120
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))


#define STACK_CHECKER_ON
#define CONFIGURE_INIT

#include <bsp.h>

#include <errno.h>
#include <time.h>

rtems_task Init(
  rtems_task_argument argument
);

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
#include "networkconfig.h"

#include "FilesystemImage.h"

/*
 *  FTP data
 */
#if defined(USE_FTPD)
  boolean FTPD_enabled = TRUE;
  struct rtems_ftpd_configuration rtems_ftpd_configuration = {
    10,                     /* FTPD task priority            */
    1024,                   /* Maximum buffersize for hooks  */
    21,                     /* Well-known port     */
    NULL                    /* List of hooks       */
 };
#else
 boolean FTPD_enabled = FALSE;
#endif

/*
 *  Some data declarations that are server dependent
 */

#if defined(USE_SIMPLE_HTTPD)
  boolean Simple_HTTPD_enabled = TRUE;

  #include <shttpd/shttpd.h>

  extern void osmonweb_init( const char *, int );

  void my_shttpd_addpages(struct shttpd_ctx *ctx)
  {
    extern void osmonweb_register( struct shttpd_ctx *ctx );
    extern void example_shttpd_addpages(struct shttpd_ctx *ctx);
    osmonweb_register( ctx );
    example_shttpd_addpages( ctx );
  }
  

#else
  boolean Simple_HTTPD_enabled = FALSE;
#endif

#define bool2string(_b) ((_b) ? "true" : "false")

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  printf("\n\n*** OSMONWEB TEST ***\n\r" );
  printf("Simple HTTPD Enabled: %s\n", bool2string(Simple_HTTPD_enabled) );
  printf("FTPD Enabled: %s\n", bool2string(FTPD_enabled) );
  printf("\n");

  /*
   * Load Base Filesystem Image
   */
  printf("Loading filesystem image\n");
  status = Untar_FromMemory( (char *)FilesystemImage, FilesystemImage_size );
   
  printf("Initializing Network");
  rtems_bsdnet_initialize_network ();

  #if defined(START_DEBUG_STUB)
    rtems_gdb_start( 10, NULL );
  #endif

  #if defined(USE_FTPD)
    printf( "Initializing FTPD\n" );
    rtems_initialize_ftpd();
  #endif

  #if defined(USE_SIMPLE_HTTPD)
    printf( "Initializing Simple HTTPD\n" );
    status = rtems_initialize_webserver(
      100,                             /* initial priority */
      48 * 1024,                       /* stack size */
      RTEMS_DEFAULT_MODES,             /* initial modes */
      RTEMS_DEFAULT_ATTRIBUTES,        /* attributes */
      NULL,                            /* init_callback */
      my_shttpd_addpages,              /* addpages_callback */
      "/",                             /* initial priority */
      80                               /* port to listen on */
    );
    if ( status )
      printf( "ERROR -- failed to initialize webserver\n" );

  #endif

  
  osmonweb_init( "/", 1 );
  osmonweb_init( "/", 2 );

  status = rtems_task_delete( RTEMS_SELF );
}



#if defined(START_DEBUG_STUB)
  void _BSP_clear_hostbridge_errors(int x, int y){}
#endif

