/*
 * Network configuration
 * 
 ************************************************************
 * EDIT THIS FILE TO REFLECT YOUR NETWORK CONFIGURATION     *
 * BEFORE RUNNING ANY RTEMS PROGRAMS WHICH USE THE NETWORK! * 
 ************************************************************
 *
 *  $Id$
 */

#ifndef _RTEMS_NETWORKCONFIG_H_
#define _RTEMS_NETWORKCONFIG_H_

#define RTEMS_USE_BOOTP

#include <bsp.h>

/*
 * Default network interface
 */
static struct rtems_bsdnet_ifconfig netdriver_config = {
	RTEMS_BSP_NETWORK_DRIVER_NAME,		/* name */
	RTEMS_BSP_NETWORK_DRIVER_ATTACH,	/* attach function */

	NULL,				/* link to next interface */

#if (defined (RTEMS_USE_BOOTP))
	NULL,				/* BOOTP supplies IP address */
	NULL,				/* BOOTP supplies IP net mask */
#else
	"128.233.14.91",		/* IP address */
	"255.255.255.0",		/* IP net mask */
#endif /* !RTEMS_USE_BOOTP */

	NULL,				/* Driver supplies hardware address */
	0				/* Use default driver parameters */
};

/*
 * Network configuration
 */
struct rtems_bsdnet_config rtems_bsdnet_config = {
	&netdriver_config,

#if (defined (RTEMS_USE_BOOTP))
	rtems_bsdnet_do_bootp,
#else
	NULL,
#endif

	0,			/* Default network task priority */
	0,			/* Default mbuf capacity */
	0,			/* Default mbuf cluster capacity */

#if (!defined (RTEMS_USE_BOOTP))
	"rgamon",		/* Host name */
	"usask.ca",		/* Domain name */
	"128.233.14.100",	/* Gateway */
	"128.233.14.1",		/* Log host */
	{"128.233.14.1" },	/* Name server(s) */
#endif /* !RTEMS_USE_BOOTP */

};

/*
 * For TFTP test application
 */
#if (!defined (RTEMS_USE_BOOTP))
#define RTEMS_TFTP_TEST_HOST_NAME "128.233.14.1"
#define RTEMS_TFTP_TEST_FILE_NAME "bootfiles/m68360/RGAmonitor"
#endif

#endif /* _RTEMS_NETWORKCONFIG_H_ */
