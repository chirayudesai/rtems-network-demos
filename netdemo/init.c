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
 * $Id$
 */

/*
 *  Include user provided information
 */

#include "../usercfg.h"

#include <bsp.h>

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXECUTIVE_RAM_SIZE	(512*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES	20
#define CONFIGURE_MAXIMUM_TASKS		20
#define CONFIGURE_MAXIMUM_TIMERS	10
#define CONFIGURE_MAXIMUM_PERIODS	1

#define CONFIGURE_MICROSECONDS_PER_TICK	10486

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
#include <rtems_ka9q.h>


#if (defined (USE_BOOTP))
#include <bootp.h>
#endif

/*
 * Suspend execution for the specified number of seconds
 */
static void
delay_task (int seconds)
{
	rtems_interval ticksPerSecond;

	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticksPerSecond);
	rtems_task_wake_after (seconds * ticksPerSecond);
}

/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
	printf( "\n\n*** ETHERNET DEMO TEST ***\n" );

	/*
	 * Start KA9Q
	 */
	puts( "Starting KA9Q" );
	rtems_ka9q_start (50);

	/*
	 * Hook up drivers
	 */
	puts( "Attaching to the network" );
#if (defined (USE_BOOTP))
	if (rtems_ka9q_execute_command ("attach rtems broadcast y"
					" ether " MY_ETHERNET_ADDRESS))
#else
	if (rtems_ka9q_execute_command ("attach rtems broadcast y"
					" ip " MY_IP_ADDRESS
					" ether " MY_ETHERNET_ADDRESS))
#endif
		rtems_panic ("Can't attach Ethernet driver.\n");

#if (defined (TRACE_NETWORK_DRIVER))
	/*
	 * Turn on debugging
	 */
	puts( "Enabling debug mode of KA9Q" );
	if (rtems_ka9q_execute_command ("trace rtems input <stdout>")
	 || rtems_ka9q_execute_command ("trace rtems output <stdout>")
	 || rtems_ka9q_execute_command ("trace rtems ascii <stdout>"))
		rtems_panic ("Can't set tracing for Ethernet driver.\n");
#endif

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

#if (defined (USE_BOOTP))
	{
	int i;
	/*
	 * Get BOOTP information
	 */
	for (i = 0 ; ; ) {
		if (rtems_ka9q_execute_command ("bootp") == 0)
			break;
		if (++i == 10)
			rtems_panic ("Can't get information from BOOTP server.\n");
		delay_task (i);
	}
	if (BootpFileName)
		printf ("BOOTP filename: `%s'\n", BootpFileName);
	else
		printf ("BOOTP -- No filename!\n");
	}
#else
	if (rtems_ka9q_execute_command ("ifconfig rtems netmask 255.255.255.0"))
		rtems_panic ("Can't set netmask.\n");
	if (rtems_ka9q_execute_command ("route add default rtems"))
		rtems_panic ("Can't add default route.\n");
	printf ("Routing table after adding default route\n");
	rtems_ka9q_execute_command ("route");
#endif

	/*
	 * Issue a gratuitous ARP request to update tables in
	 * other hosts on this network.
	 */
	if (rtems_ka9q_execute_command ("arp gratuitous rtems"))
		rtems_panic ("Can't send gratuitous ARP.\n");
	
	/*
	 * Everything is now running
	 */
	printf ("NETWORK INITIALIZED!\n");

	/*
	 * See if sockets work properly
	 */
	doSocket ();

	/*
	 * Wind things up
	 */
	delay_task (2);
	rtems_ka9q_execute_command ("detach rtems");
	exit (0);
}
