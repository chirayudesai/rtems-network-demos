/*
 * Test RTEMS/KA9Q TFTP device driver
 *
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *      1. Leave this author information intact.
 *      2. Document any changes you make.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 */

#include "../usercfg.h"

#include <bsp.h>
#include <rtems/error.h>
#include <tftp.h>
#include <rtems_ka9q.h>
#include <stdio.h>

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXECUTIVE_RAM_SIZE	(128*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES	10
#define CONFIGURE_MAXIMUM_TIMERS	5
#define CONFIGURE_MAXIMUM_PERIODS	1

#define CONFIGURE_MICROSECONDS_PER_TICK	10486

#define CONFIGURE_INIT
rtems_task Init (rtems_task_argument argument);

#define CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE
rtems_driver_address_table Device_drivers[] = {
  CONSOLE_DRIVER_TABLE_ENTRY,
  CLOCK_DRIVER_TABLE_ENTRY,
  TFTP_DRIVER_TABLE_ENTRY,
};

#include <confdefs.h>

#include <bootp.h>
extern void testTFTP (void);

/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
	int i;
	rtems_task_priority oldPri;
	rtems_interval ticksPerSecond;

	/*
	 * Get some timing information
	 */
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticksPerSecond);

	/*
	 * Start KA9Q
	 */
	rtems_task_set_priority (RTEMS_SELF, 30, &oldPri);
	rtems_ka9q_start (20);

	/*
	 * Hook up drivers
	 */
	if (rtems_ka9q_execute_command ("attach rtems broadcast n ether " MY_ETHERNET_ADDRESS))
		rtems_panic ("Can't attach Ethernet driver.\n");

	/*
	 * Configure the driver
	 */
	if (rtems_ka9q_execute_command ("ifconfig rtems broadcast 255.255.255.255"))
		rtems_panic ("Can't configure Ethernet driver.\n");

	/*
	 * Add the ethernet broadcast address to the ARP table.
	 */
	if (rtems_ka9q_execute_command ("arp add 255.255.255.255 ether FF:FF:FF:FF:FF:FF"))
		rtems_panic ("Can't add broadcast entry to ARP table.\n");

	/*
	 * Get BOOTP information
	 */
	for (i = 0 ; ; ) {
		if (rtems_ka9q_execute_command ("bootp") == 0)
			break;
		if (++i == 10)
			rtems_panic ("Can't get information from BOOTP server.\n");
		rtems_task_wake_after (i * ticksPerSecond);
	}

	/*
	 * Test TFTP driver
	 */
	testTFTP ();

	/*
	 * Wind things up
	 */
	rtems_task_wake_after (2 * ticksPerSecond);
	rtems_ka9q_execute_command ("detach rtems");
	exit (0);
}
