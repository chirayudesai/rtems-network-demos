/*
 * RTEMS configuration/initialization
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

/*#define TRACE_SCC1 1 */
#include <bsp.h>

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXECUTIVE_RAM_SIZE    (512*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES    20
#define CONFIGURE_MAXIMUM_TASKS         20
#define CONFIGURE_MAXIMUM_TIMERS        10
#define CONFIGURE_MAXIMUM_PERIODS       1
#define CONFIGURE_MICROSECONDS_PER_TICK 10486

#define CONFIGURE_INIT
rtems_task Init (rtems_task_argument argument);

#include <confdefs.h>

#include <stdio.h>
#include <rtems_ka9q.h>

/*
 * Board ethernet address
 * REPLACE THIS WITH YOUR OWN VALUE BEFORE TRYING TO USE THIS PROGRAM!
 */
#define MY_ETHERNET_ADDRESS "3B:1D:3E:21:E2:D5"
#define MY_ETHERNET_ADDRESS "prom"

/*
 * Use BOOTP to get information about me?
 */
#define USE_BOOTP	1

#if (defined (USE_BOOTP))
#include <bootp.h>
#else
/*
 * Information about me if BOOTP isn't used
 * CHOOSE A VALUE APPROPRIATE TO YOUR NETWORK!
 */
#define MY_IP_ADDRESS	"128.233.14.68"
#endif

/*
 * Suspend execution for the specified number of seconds
 */
static void
delay_task (int seconds)
{
	rtems_task_wake_after ((seconds * 1000000) / BSP_Configuration.microseconds_per_tick);
}

/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
	rtems_task_priority oldPri;
	rtems_mode old_mode;

#if (defined (m68040))
	/*
	 * Hook up FPSP
	 */
	M68kFPSPInstallExceptionHandlers ();

	/*
	 * Turn on instruction cache
	 */
	asm volatile ("cinva bc");
	asm volatile ("movec %0,itt0" : : "d" (0x00ffc004));
	asm volatile ("movec %0,dtt0" : : "d" (0x00ffc040));
	asm volatile ("cinva bc\n\t"
			"movec %0,cacr" : : "d" (0x80008000));
#endif

	/*
	 * Start KA9Q
	 */
	rtems_task_set_priority (RTEMS_SELF, 30, &oldPri);
	rtems_ka9q_start (30);

	/*
	 * Hook up drivers
	 */
#if (defined (USE_BOOTP))
	if (rtems_ka9q_execute_command ("attach rtems"
					" rbuf 24 tbuf 5"
					" ether " MY_ETHERNET_ADDRESS))
#else
	if (rtems_ka9q_execute_command ("attach rtems" 
					" rbuf 24 tbuf 5"
					" ip " MY_IP_ADDRESS
					" ether " MY_ETHERNET_ADDRESS))
#endif
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

	rtems_ka9q_execute_command ("tcp window");
	rtems_ka9q_execute_command ("tcp window 4096");
	rtems_ka9q_execute_command ("tcp window");

	/*
	 * Whew!
	 */
	printf ("NETWORK INITIALIZED!\n");

	/*
	 * Let other tasks preempt this one
	 */
	rtems_task_mode (RTEMS_PREEMPT, RTEMS_PREEMPT_MASK, &old_mode);

	/*
	 * See if sockets work properly
	 */
	test_network ();
	exit (0);
}
