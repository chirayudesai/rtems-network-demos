/*
 * RTEMS configuration/initialization
 */
 
#include <bsp.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <stdio.h>

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define STACK_CHECKER_ON 1
#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS   2
#define CONFIGURE_MAXIMUM_SEMAPHORES	    20
#define CONFIGURE_MAXIMUM_TASKS		    12

#define CONFIGURE_MICROSECONDS_PER_TICK       20000

#define CONFIGURE_INIT_TASK_PRIORITY	99
#define CONFIGURE_INIT_TASK_STACK_SIZE	(16*1024)
#define CONFIGURE_INIT

rtems_task Init(rtems_task_argument argument);

#include <confdefs.h>

/*
 * Network configuration
 */
#define NETWORK_TASK_PRIORITY	50
static struct rtems_bsdnet_ifconfig netdriver_config = {
	  RTEMS_BSP_NETWORK_DRIVER_NAME,
	  RTEMS_BSP_NETWORK_DRIVER_ATTACH,
};

struct rtems_bsdnet_config rtems_bsdnet_config = {
	&netdriver_config,	/* Network interface */
	rtems_bsdnet_do_bootp,  /* Use BOOTP to get network configuration */
	NETWORK_TASK_PRIORITY,  /* Network task priority */
};

#ifdef SERVER
#define T(x)            __TXT(x)
#define __TXT(s)        #s
static char *av[] = {
	"RPC client",
	T(SERVER),
	"Test Message",
	NULL
};
#else
static char *av[] = {
	"RPC server",
	NULL
};
#endif
static int ac = (sizeof av / sizeof av[0]) - 1;

extern int rtems_main (int argc, char **argv);

rtems_task
Init (rtems_task_argument ignored)
{
	rtems_bsdnet_initialize_network ();
	rtems_bsdnet_synchronize_ntp (0, 0);

	rtems_main (ac, av);
	printf ("*** RPC Test Finish ***\n");
	exit (0);
}

/*
 * Dummy portmapper routines
 */
pmap_set () { ; }
pmap_unset () { ; }
