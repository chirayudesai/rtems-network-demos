/*
 * RTEMS configuration/initialization
 * 
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *	1. Leave this author information intact.
 *	2. Document any changes you make.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/tftp.h>

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXECUTIVE_RAM_SIZE	(512*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES	20
#define CONFIGURE_MAXIMUM_TASKS		20

#define CONFIGURE_MICROSECONDS_PER_TICK	10486

#define CONFIGURE_INIT_TASK_STACK_SIZE	(10*1024)
#define CONFIGURE_INIT_TASK_PRIORITY	100
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_INIT
rtems_task Init (rtems_task_argument argument);

#define CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE
rtems_driver_address_table Device_drivers[] = {
  CONSOLE_DRIVER_TABLE_ENTRY,
  CLOCK_DRIVER_TABLE_ENTRY,
  TFTP_DRIVER_TABLE_ENTRY,
};

#include <confdefs.h>

#include <stdio.h>
#include <rtems/rtems_bsdnet.h>
#include "../networkconfig.h"

/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
	const char *hostname, *filename;

	rtems_bsdnet_initialize_network ();

#if (defined (RTEMS_USE_BOOTP))
	hostname = NULL;
	filename = rtems_bsdnet_bootp_boot_file_name;
#else
	hostname = RTEMS_TFTP_TEST_HOST_NAME;
	filename = RTEMS_TFTP_TEST_FILE_NAME;
#endif

	testTFTP (hostname, filename);
	exit (0);
}
