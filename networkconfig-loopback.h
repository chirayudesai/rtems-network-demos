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
 */

#ifndef _RTEMS_NETWORKCONFIG_H_
#define _RTEMS_NETWORKCONFIG_H_


/* #define RTEMS_USE_BOOTP */

#include <bsp.h>

/*
 * Network configuration
 */
struct rtems_bsdnet_config rtems_bsdnet_config = {
  NULL,                /* Loop back interface only */
  NULL,                /* do not use bootp */
  0,                   /* Default network task priority */
  0,                   /* Default mbuf capacity */
  0,                   /* Default mbuf cluster capacity */
  "rtems",             /* Host name */
  "nodomain.com",      /* Domain name */
  "127.0.0.1",         /* Gateway */
  "127.0.0.1",         /* Log host */
  {"127.0.0.1" },      /* Name server(s) */
  {"127.0.0.1" },      /* NTP server(s) */
  0,                   /* sb_efficiency */
  0,                   /* udp_tx_buf_size */
  0,                   /* udp_rx_buf_size */
  0,                   /* tcp_tx_buf_size */
  0                    /* tcp_rx_buf_size */

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
