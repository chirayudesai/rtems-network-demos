/*
 * Network configuration -- LOOPBACK ONLY!!!
 *
 * See one of the other networkconfig.h files for an
 * example of a system that includes a real NIC and
 * the loopback interface.
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


/* #define RTEMS_USE_BOOTP */

#include <bsp.h>

/*
 * Loopback interface
 */
extern int rtems_bsdnet_loopattach();

/*
 * Default network interface
 */
static struct rtems_bsdnet_ifconfig netdriver_config = {
  "lo0",                    /* name */
  rtems_bsdnet_loopattach,  /* attach function */
  NULL,                     /* No more interfaces */
  "127.0.0.1",              /* IP address */
  "255.0.0.0",              /* IP net mask */
  NULL,                     /* Driver supplies hardware address */
  0                         /* Use default driver parameters */
};

/*
 * Network configuration
 */
struct rtems_bsdnet_config rtems_bsdnet_config = {
  &netdriver_config,
  NULL,                   /* do not use bootp */
  0,                      /* Default network task priority */
  0,                      /* Default mbuf capacity */
  0,                      /* Default mbuf cluster capacity */
};

/*
 * For TFTP test application
 */
#if (defined (RTEMS_USE_BOOTP))
#define RTEMS_TFTP_TEST_HOST_NAME "BOOTP_HOST"
#define RTEMS_TFTP_TEST_FILE_NAME "BOOTP_FILE"
#else
#define RTEMS_TFTP_TEST_HOST_NAME "XXX.YYY.ZZZ.XYZ"
#define RTEMS_TFTP_TEST_FILE_NAME "tftptest"
#endif

/*
 * For NFS test application
 * 
 * NFS server/path to mount and a directory to ls once mounted
 */
#define RTEMS_NFS_SERVER      "192.168.1.210"
#define RTEMS_NFS_SERVER_PATH "/home"
#define RTEMS_NFS_LS_PATH     "/mnt/nfstest"
#endif /* _RTEMS_NETWORKCONFIG_H_ */
