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

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_EXECUTIVE_RAM_SIZE	(512*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES	20
#define CONFIGURE_MAXIMUM_TASKS		20

#define CONFIGURE_MICROSECONDS_PER_TICK	10000

#define CONFIGURE_INIT_TASK_STACK_SIZE	(10*1024)
#define CONFIGURE_INIT_TASK_PRIORITY	100
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_INIT
rtems_task Init (rtems_task_argument argument);

#include <confdefs.h>

#include <stdio.h>
#include <rtems/rtems_bsdnet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../networkconfig.h"

void testTFTP (const char *hostname, const char *filename);

/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
	const char *hostname, *filename;

	rtems_bsdnet_initialize_network ();
	rtems_bsdnet_initialize_tftp_filesystem ();

	hostname = RTEMS_TFTP_TEST_HOST_NAME;
	filename = RTEMS_TFTP_TEST_FILE_NAME;

	testTFTP (hostname, filename);
	exit (0);
}
